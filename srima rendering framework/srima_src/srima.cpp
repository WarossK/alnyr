#include <../srima.hpp>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <comdef.h>
#include <filesystem>
#include <array>

#include <srima_src/srima_render_resource.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

constexpr uint32_t kFrameCount = 2u;

namespace
{
	struct srima_d3d12
	{
		D3D12_VIEWPORT viewport;
		D3D12_RECT scissor_rect;

		ComPtr<ID3D12Device4> device;
		ComPtr<ID3D12CommandQueue> command_queue;
		ComPtr<IDXGISwapChain3> swapchain;
		ComPtr<ID3D12DescriptorHeap> rtv_heap;
		ComPtr<ID3D12Resource> render_targets[kFrameCount];
		ComPtr<ID3D12CommandAllocator> command_allocator[kFrameCount];

		ComPtr<ID3D12GraphicsCommandList> command_list;
		ComPtr<ID3D12Fence> fence;
		HANDLE fence_event;

		uint32_t frame_index;
		uint32_t rtv_descriptor_size;
		uint64_t fence_value[kFrameCount]{ 1u, 1u };
		uint64_t master_fence_value = 1u;
	};


	//std::function<void(std::string_view)> debug_callback_message = [](std::string_view) {};
	std::function<void(std::string_view)> debug_callback_message = [](std::string_view msg)
	{
		std::cout << "[srima] : [info]" << msg << std::endl;
	};

	std::unique_ptr<srima_d3d12> d3d12;
}

bool WaitForGpu();
bool WaitForPreviousFrame();

void CreateDevice(const ComPtr<IDXGIFactory4>& factory)
{

	bool found = false;
	ComPtr<IDXGIAdapter1> hardware_adapter(nullptr);
	ComPtr<IDXGIAdapter1> adapter;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
	{
		DXGI_ADAPTER_DESC1 adapter_desc;
		adapter->GetDesc1(&adapter_desc);
		if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		ComPtr<IDXGIAdapter> warp_adapter;
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)))) throw std::runtime_error("warp adapter enumerate failure.");
		if (FAILED(D3D12CreateDevice(warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12->device)))) throw std::runtime_error("device create failure.");
	}
	else
	{
		hardware_adapter = adapter.Detach();
		if (FAILED(D3D12CreateDevice(hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12->device)))) throw std::runtime_error("device create failure.");
	}
}

void CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(d3d12->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&d3d12->command_queue)))) throw std::runtime_error("command queue create failure.");
}

void CreateSwapChain(const ComPtr<IDXGIFactory4>& factory, alnyr::alnyrRenderingDest * rendering_dest)
{
	{//set scissor-rect
		long width = rendering_dest->GetWidth();
		long height = rendering_dest->GetHeight();
		d3d12->scissor_rect = { 0u, 0u, width, height };
	}

	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
	swapchain_desc.BufferCount = kFrameCount;
	swapchain_desc.Width = rendering_dest->GetWidth();
	swapchain_desc.Height = rendering_dest->GetHeight();
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchain_desc.SampleDesc.Count = 1;

	auto window_handle = rendering_dest->GetHandle<HWND>();

	ComPtr<IDXGISwapChain1>	swapchain;
	if (FAILED(factory->CreateSwapChainForHwnd(
		d3d12->command_queue.Get(),
		window_handle,
		&swapchain_desc,
		nullptr, nullptr, &swapchain))) throw std::runtime_error("swapchain create failure.");

	if (FAILED(factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER))) throw std::runtime_error("MakeWindowAssociation function failure.");

	if (FAILED(swapchain.As(&d3d12->swapchain))) throw std::runtime_error("swapchain pointer swap failure.");
	d3d12->frame_index = d3d12->swapchain->GetCurrentBackBufferIndex();
}

void CreateRenderTargetViewDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = kFrameCount;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(d3d12->device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&d3d12->rtv_heap)))) throw std::runtime_error("rtv descriptor heap create failure.");

	d3d12->rtv_descriptor_size = d3d12->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void CreateFrameResource()
{
	D3D12_CPU_DESCRIPTOR_HANDLE	rtv_handle = {};
	rtv_handle.ptr = d3d12->rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr;

	unsigned int i = 0u;
	for (auto&& rt : d3d12->render_targets)
	{
		if (FAILED(d3d12->swapchain->GetBuffer(i++, IID_PPV_ARGS(&rt)))) throw std::runtime_error("back-buffer get failure.");
		d3d12->device->CreateRenderTargetView(rt.Get(), nullptr, rtv_handle);
		rtv_handle.ptr += d3d12->rtv_descriptor_size;
	}
}

void CreateCommandAllocator()
{
	for (uint32_t i = 0u; i < kFrameCount; ++i)
	{
		if (FAILED(d3d12->device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&d3d12->command_allocator[i]))))
		{
			throw std::runtime_error("command allocator create failure.");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
void CreateCommandList()
{
	if (FAILED(d3d12->device->CreateCommandList(
		1,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		d3d12->command_allocator[d3d12->frame_index].Get(),
		nullptr,
		IID_PPV_ARGS(&d3d12->command_list))))
	{
		throw std::runtime_error("comandlist create failure.");
	}

	d3d12->command_list->Close();
}

void CreateFence()
{
	d3d12->fence_event = CreateEvent(0, FALSE, FALSE, 0);

	if (FAILED(d3d12->device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&d3d12->fence))))
		throw std::runtime_error("fence create failure.");
}

srimaRootSignature srima::helper::CreateEmptyRootSignature()
{
	ComPtr<ID3D12RootSignature> rs;
	D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};

	root_signature_desc.NumParameters = 0;
	root_signature_desc.pParameters = nullptr;
	root_signature_desc.NumStaticSamplers = 0;
	root_signature_desc.pStaticSamplers = nullptr;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	if (FAILED(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) throw std::runtime_error("");
	if (FAILED(d3d12->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rs))))  throw std::runtime_error("");

	return rs;
}
///////////////////////////////////////////////////////////////////////////////////////////

bool srima::Initialize(alnyr::alnyrRenderingDest * rendering_dest)
{
	if (d3d12) throw std::runtime_error("srima::Initialize() >> already initialized.");
	d3d12 = std::make_unique<srima_d3d12>();

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug>	debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
		{
			debug_controller->EnableDebugLayer();
			debug_callback_message("d3d12 debug layer enabled.");
		}
	}
#endif

	debug_callback_message("initialize start.");

	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
	{
		debug_callback_message("d3d12 factory create failure.");
		debug_callback_message("initialize failure.");
		return false;
	}

	d3d12->viewport.TopLeftX = 0.0f;
	d3d12->viewport.TopLeftY = 0.0f;
	d3d12->viewport.Width = static_cast<float>(rendering_dest->GetWidth());
	d3d12->viewport.Height = static_cast<float>(rendering_dest->GetHeight());
	d3d12->viewport.MinDepth = 0.0f;
	d3d12->viewport.MaxDepth = 1.0f;

	try {//Initialize
		CreateDevice(factory);
		debug_callback_message("device create.");
		CreateCommandQueue();
		debug_callback_message("command queue create.");
		CreateSwapChain(factory, rendering_dest);
		debug_callback_message("swapchain create.");
		CreateRenderTargetViewDescriptorHeap();
		debug_callback_message("RTV descriptor heap create.");
		CreateFrameResource();
		debug_callback_message("frame resource create.");
		CreateCommandAllocator();
		debug_callback_message("command allocator create.");
		CreateCommandList();
		debug_callback_message("command list create.");
		CreateFence();
		debug_callback_message("fence create.");
	}
	catch (const std::runtime_error& e)
	{
		debug_callback_message(e.what());
		return false;
	}

	debug_callback_message("initialize scceeded.");
	debug_callback_message("srima rendering framework use d3d12, but running for single threaded...");

	return true;
}

void srima::Clear(Color clear_color)
{
}

void srima::Execute(const std::vector<srimaRenderResource*>& render_resources)
{
	HRESULT result;
	auto current_frame_index = d3d12->frame_index;
	constexpr float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = d3d12->render_targets[current_frame_index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	for (auto&& render_resource : render_resources)
	{
		result = d3d12->command_allocator[current_frame_index]->Reset();
		result = d3d12->command_list->Reset(d3d12->command_allocator[current_frame_index].Get(), render_resource->GetPipelineState().Get());
		d3d12->command_list->SetGraphicsRootSignature(render_resource->GetRootSignature().Get());

		d3d12->command_list->RSSetViewports(1, &d3d12->viewport);
		d3d12->command_list->RSSetScissorRects(1, &d3d12->scissor_rect);

		d3d12->command_list->ResourceBarrier(1, &barrier);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = d3d12->rtv_heap->GetCPUDescriptorHandleForHeapStart();
		rtv_handle.ptr += (d3d12->rtv_descriptor_size * current_frame_index);

		d3d12->command_list->OMSetRenderTargets(1, &rtv_handle, false, nullptr);
		d3d12->command_list->ClearRenderTargetView(rtv_handle, clearColor, 0u, nullptr);

		render_resource->StackCommand(d3d12->command_list);
	}

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	d3d12->command_list->ResourceBarrier(1, &barrier);

	result = d3d12->command_list->Close();

	std::vector<ID3D12CommandList*> command_lists{ d3d12->command_list.Get() };
	uint32_t command_lists_size = static_cast<uint32_t>(command_lists.size());
	d3d12->command_queue->ExecuteCommandLists(command_lists_size, command_lists.data());

	d3d12->swapchain->Present(1, 0);

	if (!WaitForPreviousFrame()) return;
}

void srima::Uninitialize()
{
	WaitForGpu();
	CloseHandle(d3d12->fence_event);
	d3d12.reset();
}

///////////////////////////////////////////////////////

ComPtr<ID3DBlob> CompileShader(std::filesystem::path hlsl_path, std::string shader_version, std::string function_name)
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> shader;

	if (hlsl_path.empty()) return nullptr;
	if (FAILED(D3DCompileFromFile(hlsl_path.c_str(), nullptr, nullptr, function_name.c_str(), shader_version.c_str(), compileFlags, 0, &shader, nullptr)))
	{
		throw std::runtime_error("shader compile error.");
	}

	return shader;
}

ComPtr<ID3DBlob> LoadCompiledShader(std::filesystem::path cso_path)
{
	ComPtr<ID3DBlob> shader;
	if (FAILED(D3DReadFileToBlob(cso_path.c_str(), shader.GetAddressOf())))
	{
		throw std::runtime_error("shader read error.");
	}

	return shader;
}

std::array<srimaShaderBlob, 5> CompileShaders(const srima::srimaShaderFilePaths shader_file_paths)
{
	std::array<srimaShaderBlob, 5> out;
	out[srima::srimaShaderFilePaths::eVertex] = CompileShader(shader_file_paths.VS, "vs_5_0", "main");
	out[srima::srimaShaderFilePaths::ePixel] = CompileShader(shader_file_paths.PS, "ps_5_0", "main");
	out[srima::srimaShaderFilePaths::eDomain] = CompileShader(shader_file_paths.DS, "ds_5_0", "main");
	out[srima::srimaShaderFilePaths::eHull] = CompileShader(shader_file_paths.HS, "hs_5_0", "main");
	out[srima::srimaShaderFilePaths::eGeometry] = CompileShader(shader_file_paths.GS, "gs_5_0", "main");
	return out;
}


void srima::SetDebugCallback(std::function<void(std::string_view)> callback)
{
	debug_callback_message = callback;
}

srima::srimaVertexBuffer srima::CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size)
{
	srimaVertexBuffer vertex_buffer;
	vertex_buffer.vertex_buffer_view_state_.resize(1u);

	const uint32_t vertex_buffer_size = vertex_size * vertex_array_size;

	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resource_desc = {};
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = vertex_buffer_size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(d3d12->device->CreateCommittedResource(
			&heap_properties,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertex_buffer.vertex_buffer_view_state_[0].vertex_buffer_)))) throw std::runtime_error("vertex buffer create failure.");
	}

	uint8_t* vertex_data_begin;
	D3D12_RANGE	read_range = { 0, 0 };
	if (FAILED(vertex_buffer.vertex_buffer_view_state_[0].vertex_buffer_->Map(0, &read_range, reinterpret_cast<void**>(&vertex_data_begin))))
	{
		throw std::runtime_error("vertex buffer mapping failure.");
	}
	memcpy(vertex_data_begin, vertices_start_ptr, vertex_array_size);
	vertex_buffer.vertex_buffer_view_state_[0].vertex_buffer_->Unmap(0, nullptr);

	vertex_buffer.vertex_buffer_view_state_[0].stride_ = vertex_size;
	vertex_buffer.vertex_buffer_view_state_[0].size_in_bytes_ = vertex_buffer_size;

	if (!WaitForPreviousFrame()) throw std::runtime_error("");

	return vertex_buffer;
}

srimaGraphicsPipelineState srima::CreatePipeline(srimaGraphicsPipelineStateDesc * pipeline_state_desc)
{
	ComPtr<ID3D12PipelineState> pipeline_state;
	if (FAILED(d3d12->device->CreateGraphicsPipelineState(pipeline_state_desc, IID_PPV_ARGS(&pipeline_state))))
	{
		throw std::runtime_error("graphics pipeline create failure.");
	}

	return pipeline_state;
}

srima::srimaTexture srima::CreateTexture(std::filesystem::path texture_path)
{
	return srimaTexture();
}

srimaRasterizerStateDesc srima::DefaultRasterizerStateDesc()
{
	srimaRasterizerStateDesc rasterizer_desc;

	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return rasterizer_desc;
}

srimaBlendDesc srima::DefaultBlendDesc()
{
	D3D12_RENDER_TARGET_BLEND_DESC rtv_blend_desc;

	rtv_blend_desc.BlendEnable = FALSE;
	rtv_blend_desc.LogicOpEnable = FALSE;
	rtv_blend_desc.SrcBlend = D3D12_BLEND_ONE;
	rtv_blend_desc.DestBlend = D3D12_BLEND_ZERO;
	rtv_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
	rtv_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
	rtv_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
	rtv_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rtv_blend_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
	rtv_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	srimaBlendDesc blend_desc;

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;

	for (uint32_t i = 0; i < 8; ++i)
	{
		blend_desc.RenderTarget[i] = rtv_blend_desc;
	}

	return blend_desc;
}

srimaDepthStencilStateDesc srima::DefaultDepthStencilDesc()
{
	srimaDepthStencilStateDesc depth_stencil_desc;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_COMPARISON_FUNC_ALWAYS
	};

	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace = defaultStencilOp;
	depth_stencil_desc.BackFace = defaultStencilOp;

	return depth_stencil_desc;
}

srimaGraphicsPipelineState srima::helper::CreatePSO(srimaInputLayoutDesc input_layout_desc, const srimaRootSignature& root_signature, srimaShaderFilePaths shader_file_paths)
{
	srimaGraphicsPipelineState pso;
	srimaGraphicsPipelineStateDesc pso_desc = {};
	D3D12_SHADER_BYTECODE shader_bytecode = {};
	ComPtr<ID3D12RootSignature> rs;

	pso_desc.BlendState = DefaultBlendDesc();
	pso_desc.DepthStencilState = DefaultDepthStencilDesc();
	pso_desc.RasterizerState = DefaultRasterizerStateDesc();

	auto blobs = CompileShaders(shader_file_paths);

	auto vs_blob = blobs[srimaShaderFilePaths::eVertex];
	if (vs_blob)
	{
		shader_bytecode.pShaderBytecode = vs_blob->GetBufferPointer();
		shader_bytecode.BytecodeLength = vs_blob->GetBufferSize();
		pso_desc.VS = shader_bytecode;
	}

	auto ps_blob = blobs[srimaShaderFilePaths::ePixel];
	if (ps_blob)
	{
		shader_bytecode.pShaderBytecode = ps_blob->GetBufferPointer();
		shader_bytecode.BytecodeLength = ps_blob->GetBufferSize();
		pso_desc.PS = shader_bytecode;
	}

	pso_desc.InputLayout = input_layout_desc;

	if (root_signature)
	{
		pso_desc.pRootSignature = root_signature.Get();
	}
	else
	{
		rs = CreateEmptyRootSignature();
		pso_desc.pRootSignature = rs.Get();
	}

	pso_desc.DepthStencilState.DepthEnable = FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.NodeMask = 1;

	if (FAILED(d3d12->device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso))))
	{
		throw std::runtime_error("pso create failure.");
	}

	return pso;
}


void srima::SetVertexBuffers(srimaVertexBuffer* vertex_buffer)
{
	auto vertex_buffer_view = vertex_buffer->GetVertexBufferView();
	d3d12->command_list->IASetVertexBuffers(0u, static_cast<uint32_t>(vertex_buffer_view.size()), vertex_buffer_view.data());
}

void srima::Draw(uint32_t vertex_count)
{
	d3d12->command_list->DrawInstanced(vertex_count, 1u, 0u, 0u);
}

void srima::DrawInstanced(uint32_t vertex_count, uint32_t instance_count)
{
	d3d12->command_list->DrawInstanced(vertex_count, instance_count, 0u, 0u);
}

//Minimum Implementation
bool WaitForPreviousFrame()
{
	const uint64_t current_fence_value = d3d12->master_fence_value;
	if (FAILED(d3d12->command_queue->Signal(d3d12->fence.Get(), current_fence_value))) return false;
	d3d12->master_fence_value++;

	if (d3d12->fence->GetCompletedValue() < current_fence_value)
	{
		if (FAILED(d3d12->fence->SetEventOnCompletion(current_fence_value, d3d12->fence_event))) return false;
		WaitForSingleObject(d3d12->fence_event, INFINITE);
	}

	d3d12->frame_index = d3d12->swapchain->GetCurrentBackBufferIndex();

	return true;
}

bool WaitForGpu()
{
	const uint64_t current_fence_value = d3d12->master_fence_value;

	if (FAILED(d3d12->command_queue->Signal(d3d12->fence.Get(), current_fence_value))) return false;
	++d3d12->master_fence_value;

	if (FAILED(d3d12->fence->SetEventOnCompletion(current_fence_value, d3d12->fence_event))) return false;

	WaitForSingleObject(d3d12->fence_event, INFINITE);

	return true;
}
