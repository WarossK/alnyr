#pragma once

#include <filesystem>
#include <wrl/client.h>
#include <d3d12.h>

namespace srima
{
	struct srimaTexture
	{
		friend srimaTexture CreateTexture(std::filesystem::path texture_path);
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		void Create(Microsoft::WRL::ComPtr<ID3D12Device4>& device,std::filesystem::path texture_path);
	};
}