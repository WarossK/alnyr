#pragma once

namespace alnyr
{
	class alnyrScene;
	class alnyrSceneManager
	{
	private:
		alnyrScene* current_scene_;
		alnyrScene* next_scene_;

	public:
		bool SceneInitialize();
		void SceneUpdate();
		void SceneRender();
		void SceneUninitialize();

		void SetScene(alnyrScene* scene);
	};
}