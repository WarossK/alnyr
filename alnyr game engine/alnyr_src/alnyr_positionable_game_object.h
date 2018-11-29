#pragma once

#include <alnyr_src/alnyr_material.h>
#include <alnyr_src/alnyr_transform.h>
#include <alnyr_src/alnyr_game_object.h>

namespace alnyr
{
	class alnyrPositionableGameObject : public alnyrGameObject
	{
	private:
		alnyrTransform* transform_;

	public:
		alnyrPositionableGameObject()
		{
			transform_ = AddBehavior<alnyrTransform>();
		}

		virtual ~alnyrPositionableGameObject() {}

		alnyrTransform* GetTransform() { return transform_; }
	};
}