#pragma once

#include <filesystem>
#include <alnyr_math.h>
#include <alnyr_rendering_dest.h>

#include <srima_src/srima_vertex_buffer.h>
#include <srima_src/srima_texture.h>

#define SRIMA_D3D12

#if defined(SRIMA_D3D12)
#include <d3d12.h>
#include <wrl/client.h>
using srimaGraphicsPipelineStateDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC;
using srimaGraphicsPipelineState = Microsoft::WRL::ComPtr<ID3D12PipelineState>;
using srimaRootSignature = Microsoft::WRL::ComPtr<ID3D12RootSignature>;

using srimaRasterizerStateDesc = D3D12_RASTERIZER_DESC;
using srimaBlendDesc = D3D12_BLEND_DESC;
using srimaDepthStencilStateDesc = D3D12_DEPTH_STENCIL_DESC;

#elif defined(SRIMA_VULKAN)
#include <vulkan.hpp>
//using srimaVertexBuffer =;
#endif

//short-term objective : mass rendering non-texture triangles.

namespace alnyr
{
	class alnyrRenderingDest;
	//class alnyrRenderResource;
}

namespace srima
{
	class srimaRenderResource;

	bool Initialize(alnyr::alnyrRenderingDest* rendering_dest);
	void Clear(Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f }); //default clear color is black.
	void Execute(const std::vector<srimaRenderResource*>& render_resources);
	void Uninitialize();

	//recommend : in #if defined(_DEBUG) block.
	void SetDebugCallback(std::function<void(std::string_view)> callback);

	//Create Functions
	template<class VertexType>
	srimaVertexBuffer CreateVertexBuffer(const std::vector<VertexType>& vertices)
	{
		return CreateVertexBuffer(vertices.data(), sizeof(decltype(vertices)::value_type), vertices.data());
	}

	srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);//インスタンシングしない前提で作っとる
	srimaGraphicsPipelineState CreatePipeline(srimaGraphicsPipelineStateDesc* pipeline_state_desc);//デカすぎてキレイにラップできねぇ・・・
	srimaTexture CreateTexture(std::filesystem::path texture_path);

	srimaRasterizerStateDesc DefaultRasterizerStateDesc();
	srimaBlendDesc DefaultBlendDesc();
	srimaDepthStencilStateDesc DefaultDepthStencilDesc();

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(std::filesystem::path hlsl_path, std::string shader_version, std::string function_name = "main");
#endif
	//End of Create Functions

	//Set Functions
	void SetVertexBuffers(srimaVertexBuffer* vertex_buffer);
	//End of Set Functions

	//Draw Functions
	void Draw(uint32_t vertex_count);//deprecated.
	void DrawInstanced(uint32_t vertex_count, uint32_t instance_count);
	//End of Draw Functions
}