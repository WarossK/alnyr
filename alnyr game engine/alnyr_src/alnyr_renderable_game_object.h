#pragma once

#include <alnyr_material.h>
#include <alnyr_transform.h>
#include <alnyr_game_object.h>

namespace alnyr
{
	class alnyrRenderableGameObject : public alnyrGameObject
	{
	private:
		alnyrTransform* transform_;
		alnyrMaterial* material_;

	public:
		alnyrRenderableGameObject()
		{
			transform_ = AddBehavior<alnyrTransform>();
			material_ = AddBehavior<alnyrMaterial>();
		}

		alnyrTransform* GetTransform() { return transform_; }
		alnyrMaterial* GetMaterial() { return material_; }
	};
}