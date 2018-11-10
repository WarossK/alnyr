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

namespace //static linkage
{
	struct srima_d3d12
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

		ComPtr<ID3D12PipelineState> pipeline_state;
		ComPtr<ID3D12Resource> vertex_buffer;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissor_rect;

		HANDLE event_handle;

		unsigned int frame_index;
		unsigned int rtv_descriptor_size;
		uint64_t fence_value = 0u;
	};


	std::function<void(std::string_view)> debug_callback_message = [](std::string_view) {};
	std::unique_ptr<srima_d3d12> d3d12;
}

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



	debug_callback_message("initialize scceeded.");
	return true;
}

void srima::Clear(Color clear_color)
{
}

void srima::Execute()
{
}

void srima::Uninitialize()
{
	d3d12.reset();
}

void srima::SetDebugCallback(std::function<void(std::string_view)> callback)
{
	debug_callback_message = callback;
}

srimaVertexBuffer srima::CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size)
{
	return srimaVertexBuffer();
}

void srima::SetVertexBuffer()
{
}

void srima::Draw()
{
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

	return TRUE;
}