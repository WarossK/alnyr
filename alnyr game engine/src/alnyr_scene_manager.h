#pragma once

#include <memory>
#include "alnyr_scene.h"

namespace alnyr
{
	class alnyrSceneManager
	{
	private:
		alnyrScene* current_scene_;
		alnyrScene* next_scene_;

	public:
		bool SceneInitialize();
		void SceneUpdate();
		void SceneUninitialize();

		void SetScene(alnyrScene* scene);
	};
}