#pragma once

namespace alnyr
{
	class alnyrScene;
	class alnyrSceneManager
	{
	private:
		alnyrScene* load_scene_;

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