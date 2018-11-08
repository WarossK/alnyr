#include "srima_shader_asset.h"
#include <wrl/client.h>
#include <unordered_map>
#include <type_id.hpp>

using namespace Microsoft::WRL;
using Shaders = std::unordered_map<ctti::type_index, ComPtr<ID3DBlob>>;

Shaders* shaders = nullptr;

void srima::srimaShaderAsset::CreateVertexShader(std::filesystem::path hlsl_path)
{
	
}

void srima::srimaShaderAsset::CreatePixelShader(std::filesystem::path hlsl_path)
{
}

void srima::srimaShaderAsset::CreateDomainShader(std::filesystem::path hlsl_path)
{
}

void srima::srimaShaderAsset::CreateHullShader(std::filesystem::path hlsl_path)
{
}

void srima::srimaShaderAsset::CreateGeometryShader(std::filesystem::path hlsl_path)
{
}

void srima::ShaderLoadReady()
{
	if (shaders) return;
	shaders = new Shaders;
}

void srima::ShaderUnLoad()
{
	if (!shaders) return;
	delete shaders;
	shaders = nullptr;
}
