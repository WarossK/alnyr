#pragma once

#include <vector>
#include <unordered_map>
#include <type_id.hpp>
#include <boost/container/flat_map.hpp>
#include <alnyr_util.h>
#include <alnyr_math.h>

namespace alnyr
{
	class alnyrObjectBehavior;
	class alnyrTransform;

	class alnyrGameObject
	{
	private:
		boost::container::flat_map<ctti::type_index, alnyrObjectBehavior*, util::alnyr_less<ctti::type_index>> object_behaviors_;

		alnyrGameObject* parent_;
		std::vector<alnyrGameObject*> children_;

		bool is_destroy_;
		bool is_bring_scene_change_;

	public:
		alnyrGameObject(uint32_t behavior_capacity = 32u);
		virtual ~alnyrGameObject() {}
		void BehaviorInitialize();
		void BehaviorUpdate();
		void BehaviorUninitialize();

		void SetParent(alnyrGameObject* parent);
		alnyrGameObject* GetParent();
		void AddChild(alnyrGameObject* child);
		void RemoveChild(alnyrGameObject* child); //removeしたオブジェクトにchildの所有権を渡す。
		const std::vector<alnyrGameObject*>& GetChildren();

		void Destroy();
		const bool& IsDestroyed();

		void BringNextScene(bool bringing = true);
		const bool& IsBringNextScene();

		template<class BehaviorType, class...BehaviorArgs> BehaviorType* AddBehavior(BehaviorArgs... args)
		{
			if (object_behaviors_.count(ctti::type_id<BehaviorType>())) throw std::runtime_error("behavior is found. already added.");
			return static_cast<BehaviorType*>(object_behaviors_[ctti::type_id<BehaviorType>()] = new BehaviorType(this, args...));
		}

		//クッソ遅いのでオブジェクトのイニシャライズでキャッシュしてほしい
		template<class BehaviorType> BehaviorType* GetBehavior()
		{
			if (!object_behaviors_.count(ctti::type_id<BehaviorType>())) throw std::runtime_error("behavior is not found.");
			return static_cast<BehaviorType>(object_behaviors_[ctti::type_id<BehaviorType>()]);
		}
	};
}