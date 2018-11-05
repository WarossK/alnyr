#pragma once

#include <vector>
#include <alnyr_texture.h>

namespace alnyr
{
	class alnyrResourceGroup
	{
	public:
		const uint32_t group_id_;

	private:
		std::vector<alnyrTexture> textures_;
		//std::vector<alnyrMesh> meshes_;
		//�A�j���[�V����
		//�T�E���h
		//(�G�t�F�N�g)

	public:
		//�C�ӂ̒l������B�����Ȃ̂Ō�ŉ����l����B
		alnyrResourceGroup(uint32_t group_id) : group_id_(group_id) {}
		~alnyrResourceGroup() {}

		void SetTextureResourceArray(const std::vector<alnyrTexture>& resource_array);
		//void SetMeshResourceArray(const std::vector<alnyrMesh>& resource_array);
		//void SetAnimationResourceArray(const std::vector<alnyrAnimation>& resource_array);
		//void SetSoundResourceArray(const std::vector<alnyrSound>& resource_array);
		//void SetEffectResourceArray(const std::vector<alnyrEffect>& resource_array);

		void AddTextureResource(const alnyrTexture&& resource);//move
		//void AddMeshResource(const alnyrMesh&& resource);//move
		//void AddAnimationResource();
		//void AddSoundResource();
		//void AddEffectResource();
	};

}