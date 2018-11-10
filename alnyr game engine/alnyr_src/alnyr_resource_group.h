#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <alnyr_src/alnyr_texture.h>
#include <alnyr_src/alnyr_static_mesh.h>
#include <alnyr_src/alnyr_dynamic_mesh.h>

namespace alnyr
{
	class alnyrResourceGroup
	{
	private:
		std::unordered_map<std::string, alnyrTexture> textures_;
		std::unordered_map<std::string, alnyrStaticMesh> static_meshes_;
		std::unordered_map<std::string, alnyrDynamicMesh> dynamic_meshes_;
		//アニメーション
		//サウンド
		//(エフェクト)

	public:
		alnyrResourceGroup() {}
		~alnyrResourceGroup()
		{
			textures_.clear();
			static_meshes_.clear();
			dynamic_meshes_.clear();
		}

		void SetTextureResourceArray(const std::vector<alnyrTexture>& resource_array);
		void SetMeshResourceArray(const std::vector<alnyrStaticMesh>& resource_array);
		//void SetAnimationResourceArray(const std::vector<alnyrAnimation>& resource_array);
		//void SetSoundResourceArray(const std::vector<alnyrSound>& resource_array);
		//void SetEffectResourceArray(const std::vector<alnyrEffect>& resource_array);

		void AddTextureResource(const alnyrTexture&& resource);//move
		void AddMeshResource(const alnyrStaticMesh&& resource) {};//move
		//void AddAnimationResource();
		//void AddSoundResource();
		//void AddEffectResource();
	};

}