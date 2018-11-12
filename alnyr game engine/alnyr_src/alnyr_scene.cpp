#include "alnyr_scene.h"
#include <alnyr_camera.h>

void alnyr::alnyrScene::ObjectInitialize()
{
	for (auto&& object : objects_)
	{
		if (object->IsBringNextScene()) continue;
		object->BehaviorInitialize();
	}
}

void alnyr::alnyrScene::ObjectUpdate()
{
	for (auto&& object : objects_)
	{
		if (object->IsDestroyed()) continue;
		object->BehaviorUpdate();

		if (object->IsDestroyed())
		{
			destroyed_objects_.emplace_back(object);
		}
	}

	//����element��erase����̂����Ȃ̂ň�Ԍ��̃I�u�W�F�N�g��destroyed�̎��̂ݍ폜����B
	for (bool check = true; check;)
	{
		check = false;
		for (auto&& destroyed_object : destroyed_objects_)
		{
			if (objects_.back() == destroyed_object)
			{
				delete objects_.back();
				objects_.pop_back();
				destroyed_objects_.remove(destroyed_object);
				check = true;
				break;
			}
		}
	}
}

void alnyr::alnyrScene::ObjectUninitialize()
{
	for (auto&& object : objects_)
	{
		if (object->IsBringNextScene()) continue;
		object->BehaviorUninitialize();
	}
}

void alnyr::alnyrScene::RenderExecute()
{
	resource_group_->Draw();
}
