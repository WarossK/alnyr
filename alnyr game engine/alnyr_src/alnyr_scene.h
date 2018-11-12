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

		void SetResourceGroup(std::unique_ptr<alnyrResourceGroup>&& resource_group) { resource_group_ = std::move(resource_group); }
		const std::unique_ptr<alnyrResourceGroup>& GetResourceGroup() { return resource_group_; }

		virtual void ResourceLoad(const std::unique_ptr<alnyrResourceGroup>& resource_group) = 0;
		virtual void Initialize() = 0;

	public:
		alnyrScene() : resource_group_(std::make_unique<alnyrResourceGroup>()) {}
		alnyrScene(std::unique_ptr<alnyrResourceGroup>&& resource_group) : resource_group_(std::move(resource_group)) {}
		virtual ~alnyrScene() {}

		void CallResouceLoad() { ResourceLoad(resource_group_); }
		virtual void CallInitialize() { Initialize(); };
		void RenderExecute();

		template<class ObjectType, class...ObjectArgs> alnyrGameObject* AddGameObject(ObjectArgs...args)
		{
			objects_.emplace_back(new ObjectType(args...));
			return objects_.back();
		}
	};
}