#pragma once

#include <alnyr_math.h>
#include <alnyr_rendering_dest.h>

#define SRIMA_D3D12

#if defined(SRIMA_D3D12)
#include <d3d12.h>
using srimaVertexBuffer = D3D12_VERTEX_BUFFER_VIEW;

#elif defined(SRIMA_VULKAN)
#include <vulkan.hpp>
//using srimaVertexBuffer =;
#endif

namespace alnyr { class alnyrRenderingDest; }

namespace srima
{
	bool Initialize(alnyr::alnyrRenderingDest* rendering_dest);
	void Clear(Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f }); //default clear color is black.
	void Execute();
	void Uninitialize();

	//recommend : in #if defined(_DEBUG) block.
	void SetDebugCallback(std::function<void(std::string_view)> callback);

	//Create Functions
	srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);

	//Set Functions
	void SetVertexBuffer();

	//Draw Functions
	void Draw();
}