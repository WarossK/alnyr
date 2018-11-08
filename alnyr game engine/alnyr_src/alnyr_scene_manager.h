#pragma once

#include <memory>

namespace alnyr
{
	class alnyrScene;
	class alnyrEngine;
	
	class alnyrSceneManager
	{
	private:
		alnyrScene* load_scene_;

		alnyrScene* current_scene_;
		alnyrScene* next_scene_;

	public:
		alnyrSceneManager();
		~alnyrSceneManager();
		void SceneInitialize();
		void SceneUpdate();
		void SceneRender();
		void SceneUninitialize();

		void SetScene(alnyrScene* scene);

		static void SceneChange();
	};
}