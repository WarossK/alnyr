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
		std::unique_ptr<alnyrResourceGroup> resource_group_;

		void ObjectInitialize();
		void ObjectUpdate();
		void ObjectUninitialize();

	public:
		alnyrScene() {}
		~alnyrScene() {}

		void SetResourceGroup(std::unique_ptr<alnyrResourceGroup>&& resource_group) { resource_group_ = std::move(resource_group); }
		const std::unique_ptr<alnyrResourceGroup>& GetResourceGroup() { return resource_group_; }

		template<class ObjectType, class...ObjectArgs> alnyrGameObject* AddGameObject(ObjectArgs...args)
		{
			objects_.emplace_back(new ObjectType(args...));
			return objects_.back();
		}
	};
}