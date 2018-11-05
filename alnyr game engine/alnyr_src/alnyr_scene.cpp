#include "alnyr_scene.h"
#include <alnyr_camera.h>
#include <alnyr_scene_parameter.h>

void alnyr::alnyrScene::SceneInitialize(std::function<void(alnyrScene*)> initialize_descriptor)
{
	AddGameObject<alnyrCamera>();
	initialize_descriptor(this);
}

void alnyr::alnyrScene::SceneUninitialize(std::function<void(alnyrScene*)> uninitialize_descriptor)
{
	uninitialize_descriptor(this);
}

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
	if (!new_resources) return scene_param->resource_group;

	delete scene_param->resource_group;
	scene_param->resource_group = nullptr;

	return new_resources;
}
