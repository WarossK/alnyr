#pragma once

#include <alnyr_src/alnyr_engine.h>
#include <alnyr_src/alnyr_window.h>

//���J����\��̃w�b�_
//alnyr_scene_parameter			//scene��initialize��uninitialize�ƃ��\�[�X�O���[�v�̐ݒ�p�\����
//alnyr_resource_group			//���[�h���ꂽ���\�[�X�̂܂Ƃ܂�
//alnyr_game_object				//�p���p
//alnyr_renderable_game_object	//�p���p
//alnyr_behavior				//�p���p

namespace alnyr
{
	alnyrEngine* CreateEngine();
	void TerminateEngine();
}