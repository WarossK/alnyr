#pragma once

#include <alnyr_material.h>
#include <alnyr_transform.h>
#include <alnyr_positionable_game_object.h>

namespace alnyr
{
	class alnyrRenderableGameObject : public alnyrPositionableGameObject
	{
	private:
		alnyrMaterial* material_;

	public:
		alnyrRenderableGameObject()
		{
			material_ = AddBehavior<alnyrMaterial>();
		}

		virtual ~alnyrRenderableGameObject() {}

		alnyrMaterial* GetMaterial() { return material_; }
	};
}