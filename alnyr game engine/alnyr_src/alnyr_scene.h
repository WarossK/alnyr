#pragma once

#include <vector>
#include <functional>
#include <alnyr_src/alnyr_game_object.h>
#include <alnyr_src/alnyr_resource_group.h>

namespace alnyr
{
	class alnyrScene
	{
		friend class alnyrSceneManager;
	private:
		std::vector<alnyrGameObject*> objects_;
		std::list<alnyrGameObject*> destroyed_objects_;
		alnyrResourceGroup* resource_group_;

		virtual void SceneInitialize() = 0;
		virtual void SceneUninitialize() = 0;

		void ObjectInitialize();
		void ObjectUpdate();
		void ObjectUninitialize();

		alnyrResourceGroup* GetResources(alnyrResourceGroup* new_resources = nullptr);

	public:
		alnyrScene(alnyrResourceGroup* resource_group) : resource_group_(resource_group) {}

		alnyrResourceGroup* GetResourceGroup() { return resource_group_; }

		template<class ObjectType, class...ObjectArgs> alnyrGameObject* AddGameObject(ObjectArgs...args)
		{
			objects_.emplace_back(new ObjectType(args...));
			return objects_.back();
		}
	};
}