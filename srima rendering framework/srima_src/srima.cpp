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
		ComPtr<ID3D12CommandAllocator> command_allocator;

		ComPtr<ID3D12GraphicsCommandList> command_list;
		ComPtr<ID3D12Fence> fence;
		HANDLE event_handle;

		//ComPtr<ID3D12RootSignature> root_signature;
		//ComPtr<ID3D12PipelineState> pipeline_state;
		//ComPtr<ID3D12Resource> vertex_buffer;

		unsigned int frame_index;
		unsigned int rtv_descriptor_size;
		uint64_t fence_value = 0u;
	};


	//std::function<void(std::string_view)> debug_callback_message = [](std::string_view) {};
	std::function<void(std::string_view)> debug_callback_message = [](std::string_view msg)
	{
		std::cout << "[srima] " << msg << std::endl;
	};

	std::unique_ptr<srima_d3d12> d3d12;
}

bool WaitForPreviousFrame();

void CreateDevice(const ComPtr<IDXGIFactory4>& factory)
{
	for (uint32_t i = 0u; i < 2u; ++i)
	{
		if (i)
		{
			ComPtr<IDXGIAdapter> warp_adapter;
			if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)))) throw std::runtime_error("warp adapter enumerate failure.");
			if (FAILED(D3D12CreateDevice(warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12->device))))throw std::runtime_error("device create failure.");
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardware_adapter(nullptr);
			ComPtr<IDXGIAdapter1> adapter;

			for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
			{
				DXGI_ADAPTER_DESC1 adapter_desc;
				adapter->GetDesc1(&adapter_desc);
				if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) break;
			}

			hardware_adapter = adapter.Detach();

			if (FAILED(D3D12CreateDevice(hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12->device)))) continue;
			break;
		}
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
	if (FAILED(d3d12->device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&d3d12->command_allocator))))
		throw std::runtime_error("command allocator create failure.");
}


///////////////////////////////////////////////////////////////////////////////////////////
void CreateCommandList()
{
	if (FAILED(d3d12->device->CreateCommandList(
		1,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		d3d12->command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&d3d12->command_list))))
	{
		throw std::runtime_error("comandlist create failure.");
	}

	d3d12->command_list->Close();
}

void CreateFence()
{
	d3d12->event_handle = CreateEvent(0, FALSE, FALSE, 0);

	if (FAILED(d3d12->device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&d3d12->fence))))
		throw std::runtime_error("fence create failure.");
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

void srima::Execute()
{
	HRESULT result;
	result = d3d12->command_allocator->Reset();

	result = d3d12->command_list->Reset(d3d12->command_allocator.Get(), nullptr);

	//d3d12->command_list->SetGraphicsRootSignature(d3d12->root_signature.Get());
	d3d12->command_list->RSSetViewports(1, &d3d12->viewport);
	d3d12->command_list->RSSetScissorRects(1, &d3d12->scissor_rect);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = d3d12->render_targets[d3d12->frame_index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3d12->command_list->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = d3d12->rtv_heap->GetCPUDescriptorHandleForHeapStart();
	rtv_handle.ptr += (d3d12->rtv_descriptor_size * d3d12->frame_index);

	d3d12->command_list->OMSetRenderTargets(1, &rtv_handle, false, nullptr);

	const float clearColor[] = { 0.7f, 1.0f, 0.1f, 1.0f };
	d3d12->command_list->ClearRenderTargetView(rtv_handle, clearColor, 0u, nullptr);

	{//ここにPSOが同一のオブジェクトごとにコマンドをPopulateする。

	}

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	d3d12->command_list->ResourceBarrier(1, &barrier);

	result = d3d12->command_list->Close();

	{//Execute
		ID3D12CommandList* ppCommandLists[] = { d3d12->command_list.Get() };
		d3d12->command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		d3d12->swapchain->Present(1, 0);
	}

	WaitForPreviousFrame();
}

void srima::Uninitialize()
{
	d3d12.reset();
}

///////////////////////////////////////////////////////

ComPtr<ID3DBlob> CompileShader(std::filesystem::path hlsl_path, std::string shader_version)
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> shader;

	auto cur = std::filesystem::current_path();
	if (FAILED(D3DCompileFromFile(hlsl_path.c_str(), nullptr, nullptr, "main", shader_version.c_str(), compileFlags, 0, &shader, nullptr)))
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
		return ComPtr<ID3DBlob>(nullptr);
	}

	return shader;
}

void srima::SetDebugCallback(std::function<void(std::string_view)> callback)
{
	debug_callback_message = callback;
}

srima::srimaVertexBuffer srima::CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size)
{
	srimaVertexBuffer vertex_buffer;

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
			IID_PPV_ARGS(&vertex_buffer.vertex_buffer_)))) throw std::runtime_error("vertex buffer create failure.");
	}

	uint8_t* vertex_data_begin;
	D3D12_RANGE	read_range = { 0, 0 };
	if (FAILED(vertex_buffer.vertex_buffer_->Map(0, &read_range, reinterpret_cast<void**>(&vertex_data_begin)))) throw std::runtime_error("vertex buffer mapping failure.");
	memcpy(vertex_data_begin, vertices_start_ptr, vertex_array_size);
	vertex_buffer.vertex_buffer_->Unmap(0, nullptr);

	vertex_buffer.vertex_buffer_view_.emplace_back(D3D12_VERTEX_BUFFER_VIEW{ vertex_buffer.vertex_buffer_->GetGPUVirtualAddress(), vertex_size, vertex_buffer_size });

	WaitForPreviousFrame();

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

void srima::SetVertexBuffers(const srimaVertexBuffer& vertex_buffer)
{
	auto vertex_buffer_view = vertex_buffer.GetVertexBufferView();
	d3d12->command_list->IASetVertexBuffers(0u, static_cast<uint32_t>(vertex_buffer_view.size()), vertex_buffer_view.data());
}

void srima::Draw(uint32_t vertex_count)
{
	d3d12->command_list->DrawInstanced(vertex_count, 1u, 0u, 0u);
}

//Minimum Implementation
bool WaitForPreviousFrame()
{
	const uint64_t fence = d3d12->fence_value;
	if (FAILED(d3d12->command_queue->Signal(d3d12->fence.Get(), fence))) return false;
	d3d12->fence_value++;

	if (d3d12->fence->GetCompletedValue() < fence)
	{
		if (FAILED(d3d12->fence->SetEventOnCompletion(fence, d3d12->event_handle))) return false;
		WaitForSingleObject(d3d12->event_handle, INFINITE);
	}

	d3d12->frame_index = d3d12->swapchain->GetCurrentBackBufferIndex();

	return true;
}