#include "alnyr_scene.h"
#include <alnyr_camera.h>

void alnyr::alnyrScene::ObjectInitialize()
{
	for (auto&& object : objects_)
	{
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

	//中間elementをeraseするのが嫌なので一番後ろのオブジェクトがdestroyedの時のみ削除する。
	while (bool check = true)
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
		object->BehaviorUninitialize();
	}
}

alnyr::alnyrResourceGroup * alnyr::alnyrScene::GetResources(alnyrResourceGroup * new_resources)
{
	if (!new_resources) return resource_group_;

	delete resource_group_;
	resource_group_ = nullptr;

	return new_resources;
}
