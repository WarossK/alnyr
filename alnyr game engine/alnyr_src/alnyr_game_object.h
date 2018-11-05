#pragma once

#include <vector>
#include <unordered_map>
#include <type_id.hpp>

namespace alnyr
{
	class alnyrObjectBehavior;
	class alnyrGameObject
	{
	private:
		std::unordered_map<ctti::type_index, alnyrObjectBehavior*> object_behaviors_;

		alnyrGameObject* parent_;
		std::vector<alnyrGameObject*> children_;

		bool is_destroy_;

	public:
		void BehaviorInitialize();
		void BehaviorUpdate();
		void BehaviorUninitialize();

		void SetParent(alnyrGameObject* parent);
		alnyrGameObject* GetParent();
		void AddChild(alnyrGameObject* child);
		void RemoveChild(alnyrGameObject* child);

		void Destroy();
		const bool& IsDestroyed();

		template<class BehaviorType, class...BehaviorArgs> BehaviorType* AddBehavior(BehaviorArgs... args)
		{
			if(object_behaviors_.count(ctti::type_id<BehaviorType>())) throw std::runtime_error("behavior is found. already added.");
			return static_cast<BehaviorType*>(object_behaviors_[ctti::type_id<BehaviorType>()] = new BehaviorType(this, args...));
		}

		template<class BehaviorType> BehaviorType* GetBehavior()
		{
			if (!object_behaviors_.count(ctti::type_id<BehaviorType>())) throw std::runtime_error("behavior is not find.");
			return object_behaviors_[ctti::type_id<BehaviorType>()];
		}
	};
}