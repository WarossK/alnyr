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

	HANDLE event_handle;

	unsigned int frame_index;
	unsigned int rtv_descriptor_size;
}*properties = nullptr;

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
	D3D12_COMMAND_QUEUE_DESC	queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(properties->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&properties->command_queue)))) return false;

	return true;
}

bool CreateSwapChain(const ComPtr<IDXGIFactory4>& factory, srima::window::srimaWindow* window)
{
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
	swapchain_desc.BufferCount = kFrameCount;
	swapchain_desc.Width = window->GetWidth();
	swapchain_desc.Height = window->GetHeight();
	swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchain_desc.SampleDesc.Count = 1;

	auto window_handle = reinterpret_cast<HWND>(window->GetHandle());

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

bool CreateDescriptorHeap()
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
		if (FAILED(properties->swapchain->GetBuffer(i, IID_PPV_ARGS(&rt)))) return false;
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

bool CreateRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.NumParameters = 0;
	root_signature_desc.pParameters = nullptr;
	root_signature_desc.NumStaticSamplers = 0;
	root_signature_desc.pStaticSamplers = nullptr;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	if (FAILED(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) return false;
	if (FAILED(properties->device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&properties->root_signature)))) return false;

	return true;
}

bool CreateCommandList()
{
	return SUCCEEDED(properties->device->CreateCommandList(
		1,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		properties->command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&properties->command_list)));

	//IID_ID3D12GraphicsCommandList,
	//reinterpret_cast<void**>(properties->command_list.GetAddressOf())));
}

bool CreateFence()
{
	properties->event_handle = CreateEvent(0, FALSE, FALSE, 0);

	return SUCCEEDED(properties->device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&properties->fence)));

	//IID_ID3D12Fence,
	//reinterpret_cast<void**>(properties->fence.GetAddressOf())));
}

bool srima::d3d12::Initialize(srima::window::srimaWindow* window)
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

	properties->viewport.Width = window->GetWidth();
	properties->viewport.Height = window->GetHeight();

	std::vector<std::function<bool()>> initilize_list
	{
		std::bind(CreateDevice, factory)
		,CreateCommandQueue
		,std::bind(CreateSwapChain, factory, window)
		,CreateDescriptorHeap
		,CreateFrameResource
		,CreateCommandAllocator
		,CreateRootSignature
		,CreateCommandList
		,CreateFence
	};

	return std::all_of(initilize_list.begin(), initilize_list.end(), [](std::function<bool()>& func)
	{
		return func();
	});
}

void srima::d3d12::TestRender()
{
	auto resource_barrier = [](ID3D12GraphicsCommandList* pCmdList,
							   ID3D12Resource* pResource,
							   D3D12_RESOURCE_STATES stateBefore,
							   D3D12_RESOURCE_STATES stateAfter)
	{
		D3D12_RESOURCE_BARRIER desc = {};
		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource = pResource;
		desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore = stateBefore;
		desc.Transition.StateAfter = stateAfter;

		pCmdList->ResourceBarrier(1, &desc);
	};

	D3D12_CPU_DESCRIPTOR_HANDLE	rtv_handle = {};
	rtv_handle.ptr =
		properties->rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr + properties->frame_index * properties->rtv_descriptor_size;
	properties->command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

	properties->command_list->RSSetViewports(1, &properties->viewport);
	//リソースバリア
	resource_barrier(
		properties->command_list.Get()
		, properties->render_targets[properties->frame_index].Get()
		, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

	float clearColor[] = { 0.39f, 0.58f, 0.92f, 1.0f };
	properties->command_list->ClearRenderTargetView(rtv_handle, clearColor, 1, &properties->scissor_rect);
	//リソースバリア
	resource_barrier(
		properties->command_list.Get()
		, properties->render_targets[properties->frame_index].Get()
		, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);

	Present();
}

void srima::d3d12::Present()
{
	properties->command_list->Close();
	try
	{
		properties->command_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(properties->command_list.GetAddressOf()));
	}
	catch (_com_error ce)
	{
		std::wcout << ce.ErrorMessage() << std::endl;
	}

	properties->fence->Signal(0);
	properties->fence->SetEventOnCompletion(1, properties->event_handle);
	properties->command_queue->Signal(properties->fence.Get(), 1);
	WaitForSingleObject(properties->event_handle, INFINITE);

	properties->swapchain->Present(0, 0);

	properties->command_allocator->Reset();
	properties->command_list->Reset(properties->command_allocator.Get(), nullptr);
}

void srima::d3d12::Uninitialize()
{
	if (!properties) throw std::runtime_error("srima::d3d12::Uninitialize() >> d3d12 uninitialized.");

	delete properties;
	properties = nullptr;
}
