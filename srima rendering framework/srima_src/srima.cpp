#include <../srima.hpp>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <comdef.h>
#include <filesystem>
#include <mutex>
#include <srima_src/srima_thread_pool.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

constexpr unsigned int kFrameCount = 2u;

struct Property
{
	ComPtr<ID3D12Device4> device;
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<IDXGISwapChain3> swapchain;
	ComPtr<ID3D12DescriptorHeap> rtv_heap;
	ComPtr<ID3D12Resource> render_targets[kFrameCount];
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12GraphicsCommandList> command_list;
	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12Fence> fence;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;

	std::mutex command_queue_mutex;

	HANDLE event_handle;

	unsigned int frame_index;
	unsigned int rtv_descriptor_size;
}*properties = nullptr;

//AppWakeUpInitialize
bool CreateDevice(const ComPtr<IDXGIFactory4>& factory)
{
	if (bool use_warp_device = false) //temporary.
	{
		ComPtr<IDXGIAdapter>	warp_adapter;
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)))) return false;
		if (FAILED(D3D12CreateDevice(warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&properties->device)))) return false;
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

		if (FAILED(D3D12CreateDevice(hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&properties->device)))) return false;
	}
	return true;
}

bool CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(properties->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&properties->command_queue)))) return false;

	return true;
}

bool CreateSwapChain(const ComPtr<IDXGIFactory4>& factory, srima::window::srimaWindow* window)
{
	{//set scissor-rect
		long width = window->GetWidth();
		long height = window->GetHeight();
		properties->scissor_rect = { 0u, 0u, width, height };
	}

	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
	swapchain_desc.BufferCount = kFrameCount;
	swapchain_desc.Width = window->GetWidth();
	swapchain_desc.Height = window->GetHeight();
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchain_desc.SampleDesc.Count = 1;

	auto window_handle = window->GetHandle<HWND>();

	ComPtr<IDXGISwapChain1>	swapchain;
	if (FAILED(factory->CreateSwapChainForHwnd(
		properties->command_queue.Get(),
		window_handle,
		&swapchain_desc,
		nullptr, nullptr, &swapchain))) return false;

	if (FAILED(factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER))) return false;

	if (FAILED(swapchain.As(&properties->swapchain))) return false;
	properties->frame_index = properties->swapchain->GetCurrentBackBufferIndex();

	return true;
}

bool CreateRenderTargetViewDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = kFrameCount;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(properties->device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&properties->rtv_heap)))) return true;

	properties->rtv_descriptor_size = properties->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	return true;
}

bool CreateFrameResource()
{
	D3D12_CPU_DESCRIPTOR_HANDLE	rtv_handle = {};
	rtv_handle.ptr = properties->rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr;

	unsigned int i = 0u;
	for (auto&& rt : properties->render_targets)
	{
		if (FAILED(properties->swapchain->GetBuffer(i++, IID_PPV_ARGS(&rt)))) return false;
		properties->device->CreateRenderTargetView(rt.Get(), nullptr, rtv_handle);
		rtv_handle.ptr += properties->rtv_descriptor_size;
	}

	return true;
}

bool CreateCommandAllocator()
{
	return SUCCEEDED(properties->device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&properties->command_allocator)));
}
//End of AppWakeUpInitialize

//AssetsInitialize
bool CreateEmptyRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC root_signature_desc;
	SecureZeroMemory(&root_signature_desc, sizeof(root_signature_desc));
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	if (FAILED(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
	{
		std::cout << reinterpret_cast<const char*>(error->GetBufferPointer()) << std::endl;
		return false;
	}

	if (FAILED(properties->device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&properties->root_signature)))) return false;

	return true;
}

ComPtr<ID3DBlob> CompileShader(std::filesystem::path hlsl_path)
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> shader;

	if (FAILED(D3DCompileFromFile(hlsl_path.c_str(), nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &shader, nullptr)))
	{
		return ComPtr<ID3DBlob>(nullptr);
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

bool CreateVertexInputLayout()
{
	//かんがえる
	return true;
}

bool CreatePipelineStateObject(std::filesystem::path vertex_shader_path, std::filesystem::path pixel_shader_path)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

	return true;
}

bool CreateCommandList()
{
	if (FAILED(properties->device->CreateCommandList(
		1,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		properties->command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&properties->command_list))))
	{
		return false;
	}

	properties->command_list->Close();

	return true;
}

bool CreateFence()
{
	properties->event_handle = CreateEvent(0, FALSE, FALSE, 0);

	return SUCCEEDED(properties->device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&properties->fence)));
}

bool srima::d3d12::InitializePipeline(srima::window::srimaWindow* window)
{
	if (properties) throw std::runtime_error("srima::d3d12::Initialize() >> d3d12 already initialized.");
	properties = new Property();

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug>	debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
		{
			debug_controller->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return false;

	properties->viewport.TopLeftX = 0.0f;
	properties->viewport.TopLeftY = 0.0f;
	properties->viewport.Width = static_cast<float>(window->GetWidth());
	properties->viewport.Height = static_cast<float>(window->GetHeight());
	properties->viewport.MinDepth = 0.0f;
	properties->viewport.MaxDepth = 1.0f;

	std::vector<std::function<bool()>> initilize_list
	{
		std::bind(CreateDevice, factory)
		,CreateCommandQueue
		,std::bind(CreateSwapChain, factory, window)
		,CreateRenderTargetViewDescriptorHeap
		,CreateFrameResource
		,CreateCommandAllocator
		//
		,CreateEmptyRootSignature
		,CreateCommandList
		,CreateFence
	};

	return std::all_of(initilize_list.begin(), initilize_list.end(), [](std::function<bool()>& func)
	{
		return func();
	});
}

void srima::d3d12::PopulateCommandList()
{
	HRESULT result;
	result = properties->command_allocator->Reset();

	result = properties->command_list->Reset(properties->command_allocator.Get(), nullptr);

	//properties->command_list->SetGraphicsRootSignature(properties->root_signature.Get());
	properties->command_list->RSSetViewports(1, &properties->viewport);
	properties->command_list->RSSetScissorRects(1, &properties->scissor_rect);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = properties->render_targets[properties->frame_index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	properties->command_list->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = properties->rtv_heap->GetCPUDescriptorHandleForHeapStart();
	rtv_handle.ptr += (properties->rtv_descriptor_size * properties->frame_index);

	properties->command_list->OMSetRenderTargets(1, &rtv_handle, false, nullptr);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	D3D12_RECT rect(properties->scissor_rect);//{ 100, 100, 400, 400 };
	properties->command_list->ClearRenderTargetView(rtv_handle, clearColor, 1, &rect);

	//ここにPSOが同一のオブジェクトごとにコマンドをPopulateする。

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	properties->command_list->ResourceBarrier(1, &barrier);

	result = properties->command_list->Close();

	{//Excute
		ID3D12CommandList* ppCommandLists[] = { properties->command_list.Get() };
		properties->command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		properties->swapchain->Present(1, 0);
	}
}

void srima::d3d12::WaitNextFrame()
{
	static uint64_t f = 0;
	const uint64_t fence = f;
	properties->command_queue->Signal(properties->fence.Get(), fence);
	f++;

	if (properties->fence->GetCompletedValue() < fence)
	{
		properties->fence->SetEventOnCompletion(fence, properties->event_handle);
		WaitForSingleObject(properties->event_handle, INFINITE);
	}

	properties->frame_index = properties->swapchain->GetCurrentBackBufferIndex();
}

void srima::d3d12::Uninitialize()
{
	if (!properties) throw std::runtime_error("srima::d3d12::Uninitialize() >> d3d12 uninitialized.");

	delete properties;
	properties = nullptr;
}
