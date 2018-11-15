#include "alnyr_game_object.h"
#include <alnyr_object_behavior.h>

alnyr::alnyrGameObject::alnyrGameObject(uint32_t behavior_capacity) :
	is_destroy_(false)
	, is_bring_scene_change_(false)
{
	auto cap = behavior_capacity ? behavior_capacity : std::numeric_limits<uint32_t>::max();
	object_behaviors_.reserve(cap);
}

void alnyr::alnyrGameObject::BehaviorInitialize()
{
	for (auto&&[type_index, behavior] : object_behaviors_)
	{
		behavior->Initialize();
	}

	for (auto&& child : children_)
	{
		child->BehaviorInitialize();
	}
}

void alnyr::alnyrGameObject::BehaviorUpdate()
{
	for (auto&&[type_index, behavior] : object_behaviors_)
	{
		behavior->Update();
	}

	for (auto&& child : children_)
	{
		child->BehaviorUpdate();
	}
}

void alnyr::alnyrGameObject::BehaviorUninitialize()
{
	for (auto&&[type_index, behavior] : object_behaviors_)
	{
		behavior->Uninitialize();
		delete behavior;
		behavior = nullptr;
	}

	for (auto&& child : children_)
	{
		child->BehaviorUninitialize();
	}
}

void alnyr::alnyrGameObject::SetParent(alnyrGameObject * parent)
{
	parent_ = parent;
}

alnyr::alnyrGameObject * alnyr::alnyrGameObject::GetParent()
{
	return parent_;
}

void alnyr::alnyrGameObject::AddChild(alnyrGameObject * child)
{
	child->GetParent()->RemoveChild(child);//親のリストからremoveする。無ければそのまま。

	child->SetParent(this);
	children_.emplace_back(child);
}

void alnyr::alnyrGameObject::RemoveChild(alnyrGameObject * child)
{
	auto del_itr = std::find(children_.begin(), children_.end(), child);
	if (del_itr != children_.end())
	{
		children_.erase(del_itr);
	}
}

void alnyr::alnyrGameObject::Destroy()
{
	is_destroy_ = true;
}

const bool & alnyr::alnyrGameObject::IsDestroyed()
{
	return is_destroy_;
}

void alnyr::alnyrGameObject::BringNextScene(bool bringing)
{
	is_bring_scene_change_ = bringing;
}

const bool & alnyr::alnyrGameObject::IsBringNextScene()
{
	return is_bring_scene_change_;
}
