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
using srimaInputLayoutDesc = D3D12_INPUT_LAYOUT_DESC;
using srimaShaderBlob = Microsoft::WRL::ComPtr<ID3DBlob>;

using srimaCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>;

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
	inline void* ptr_cast(void* ptr)
	{
		return ptr;
	}

	struct srimaShaderFilePaths
	{
		enum ShaderType
		{
			eVertex,
			ePixel,
			eDomain,
			eHull,
			eGeometry,
			eNumShaderType
		};

		std::filesystem::path VS;
		std::filesystem::path PS;
		std::filesystem::path DS;
		std::filesystem::path HS;
		std::filesystem::path GS;
	};

	class srimaRenderResource;

	bool Initialize(alnyr::alnyrRenderingDest* rendering_dest);
	void Clear(Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f }); //default clear color is black.
	void Execute(const std::vector<srimaRenderResource*>& render_resources);
	void Uninitialize();

	//recommend : in #if defined(_DEBUG) block.
	void SetDebugCallback(std::function<void(std::string_view)> callback);

	//Create Functions
	//////////////////////
	srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);//インスタンシングしない前提で作っとる
	template<class VertexType> srimaVertexBuffer CreateVertexBuffer(const std::vector<VertexType>& vertices)
	{
		auto vertex_list = vertices;
		return CreateVertexBuffer(vertex_list.data(), sizeof(typename decltype(vertex_list)::value_type), static_cast<uint32_t>(vertex_list.size()));
	}

	srimaGraphicsPipelineState CreatePipeline(srimaGraphicsPipelineStateDesc* pipeline_state_desc);//デカすぎてキレイにラップできねぇ・・・
	srimaTexture CreateTexture(std::filesystem::path texture_path);

	srimaRasterizerStateDesc DefaultRasterizerStateDesc();
	srimaBlendDesc DefaultBlendDesc();
	srimaDepthStencilStateDesc DefaultDepthStencilDesc();

	namespace helper
	{
		srimaGraphicsPipelineState CreatePSO(srimaInputLayoutDesc input_layout_desc, const srimaRootSignature& root_signature, srimaShaderFilePaths shader_file_paths);
		template<class VertexType> srimaGraphicsPipelineState CreatePSO(const srimaRootSignature& root_signature, srimaShaderFilePaths shader_file_paths)
		{
			return CreatePSO(VertexType::InputLayout, root_signature, shader_file_paths);
		}

		srimaRootSignature CreateRootSignature();
		srimaRootSignature CreateEmptyRootSignature();
	}

	namespace user_helper::particle
	{
		srimaGraphicsPipelineState CreateParticlePSO();
	}


//#if defined(_DEBUG)
//	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(std::filesystem::path hlsl_path, std::string shader_version, std::string function_name = "main");
//#endif
	//End of Create Functions

	//Set Functions
	/////////////////////////
	void SetVertexBuffers(srimaVertexBuffer* vertex_buffer);
	//End of Set Functions

	//Draw Functions
	////////////////////////
	void Draw(uint32_t vertex_count);//deprecated.
	void DrawInstanced(uint32_t vertex_count, uint32_t instance_count);
	//End of Draw Functions
}