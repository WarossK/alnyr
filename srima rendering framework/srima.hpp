#pragma once

#include <alnyr_math.h>
#include <alnyr_rendering_dest.h>

#include <srima_src/srima_vertex_buffer.h>

#define SRIMA_D3D12

#if defined(SRIMA_D3D12)
#include <d3d12.h>
#include <wrl/client.h>
using srimaGraphicsPipelineStateDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC;
using srimaGraphicsPipelineState = Microsoft::WRL::ComPtr<ID3D12PipelineState>;

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
	srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);//インスタンシングしない前提で作っとる
	srimaVertexBuffer CreateInstancingVertexBuffer(uint32_t view_num, void** vertices_start_ptr, uint32_t* vertex_size, uint32_t* vertex_array_size);
	srimaGraphicsPipelineState CreatePipeline(srimaGraphicsPipelineStateDesc* pipeline_state_desc);//デカすぎてキレイにラップできねぇ・・・

	//Set Functions
	void SetVertexBuffers(const srimaVertexBuffer& vertex_buffer);

	//Draw Functions
	void Draw();
}