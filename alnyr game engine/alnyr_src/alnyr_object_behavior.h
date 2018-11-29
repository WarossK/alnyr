#pragma once

#include <alnyr_game_object.h>

namespace alnyr
{
	class alnyrObjectBehavior
	{
	private:
		alnyrGameObject* game_object;

	public:
		alnyrObjectBehavior() = delete;
		alnyrObjectBehavior(alnyrGameObject* game_object) :
			game_object(game_object) {}

		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Uninitialize() {}

		alnyrGameObject* GetGameObject()
		{
			return game_object;
		}
	};
}