#pragma once

#include <filesystem>
#include <wrl/client.h>
#include <d3d12.h>

namespace srima
{
	class srimaShaderAsset
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature_;
		D3D12_SHADER_BYTECODE vertex_shader_byte_code_;
		D3D12_SHADER_BYTECODE pixel_shader_byte_code_;
		D3D12_SHADER_BYTECODE domain_shader_byte_code_;
		D3D12_SHADER_BYTECODE hull_shader_byte_code_;
		D3D12_SHADER_BYTECODE geometry_shader_byte_code_;

	public:
		void CreateRootSignature();
		void CreateVertexShader(std::filesystem::path hlsl_path);
		void CreatePixelShader(std::filesystem::path hlsl_path);
		void CreateDomainShader(std::filesystem::path hlsl_path);
		void CreateHullShader(std::filesystem::path hlsl_path);
		void CreateGeometryShader(std::filesystem::path hlsl_path);
	};

	static void ShaderLoadReady();
	static void ShaderUnLoad();
}