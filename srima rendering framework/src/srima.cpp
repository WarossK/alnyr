#include "../srima.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <stdexcept>
#include <algorithm>
#include <iostream>

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
	ComPtr<ID3D12RootSignature> root_signature;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;

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
	return SUCCEEDED(properties->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&properties->command_allocator)));
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
	if (FAILED(properties->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&properties->root_signature)))) return false;

	return true;
}

bool srima::d3d12::Initialize(srima::window::srimaWindow* window)
{
	if (properties) throw std::runtime_error("srima::d3d12::Initialize() >> d3d12 already initialized.");
	properties = new Property();

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug>	debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
			debug_controller->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return false;

	std::vector<std::function<bool()>> initilize_list
	{
		std::bind(CreateDevice, factory)
		,CreateCommandQueue
		,std::bind(CreateSwapChain, factory, window)
		,CreateDescriptorHeap
		,CreateFrameResource
		,CreateCommandAllocator
		,CreateRootSignature
	};

	return std::all_of(initilize_list.begin(), initilize_list.end(),[](std::function<bool()>& func)
	{
		return func();
	});
}

void srima::d3d12::Uninitialize()
{
	if (!properties) throw std::runtime_error("srima::d3d12::Uninitialize() >> d3d12 uninitialized.");

	delete properties;
	properties = nullptr;
}
