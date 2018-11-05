#pragma once

#include <vector>
#include <functional>
#include <alnyr_game_object.h>

namespace alnyr
{
	class alnyrResourceGroup;
	class alnyrSceneParameter;
	class alnyrScene
	{
		friend class alnyrSceneManager;
	private:
		std::vector<alnyrGameObject*> objects_;
		std::list<alnyrGameObject*> destroyed_objects_;
		alnyrSceneParameter* scene_param;

		void SceneInitialize(std::function<void(alnyrScene*)> initialize_descriptor = [](alnyrScene*) {});
		void SceneUninitialize(std::function<void(alnyrScene*)> uninitialize_descriptor = [](alnyrScene*) {});

		void ObjectInitialize();
		void ObjectUpdate();
		void ObjectUninitialize();

		alnyrResourceGroup* GetResources(alnyrResourceGroup* new_resources);

	public:
		alnyrScene(alnyrSceneParameter* scene_param) : scene_param(scene_param) {}

		template<class ObjectType, class...ObjectArgs> alnyrGameObject* AddGameObject(ObjectArgs...args)
		{
			objects_.emplace_back(new ObjectType(args...));
			return objects_.back();
		}
	};
}