#pragma once

#include <alnyr_math.h>
#include <alnyr_object_behavior.h>

namespace alnyr
{
	struct alnyrTransform : alnyrObjectBehavior
	{
		Vector3 position;
		Vector3 scale;
		Quaternion rotate;

		alnyrTransform(alnyrGameObject* game_object)
			: alnyrObjectBehavior(game_object) {}
	};
}