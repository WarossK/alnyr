#pragma once

#include <alnyr_math.h>
#include <alnyr_object_behavior.h>
#include <alnyr_texture.h>

namespace alnyr
{
	struct alnyrMaterial : public alnyrObjectBehavior
	{
		alnyrGameObject const* game_object;
		Color diffuse;
		Color albedo;
		Color specular;
		alnyrTexture texture;

		alnyrMaterial(alnyrGameObject* game_object)
			: alnyrObjectBehavior(game_object) {}
	};
}