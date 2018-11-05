#pragma once

#include <alnyr_game_object.h>

namespace alnyr
{
	class alnyrObjectBehavior
	{
	public:
		alnyrGameObject const* game_object;

		alnyrObjectBehavior() = delete;
		alnyrObjectBehavior(alnyrGameObject* game_object) :
			game_object(game_object) {}

		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Uninitialize() {}
	};
}