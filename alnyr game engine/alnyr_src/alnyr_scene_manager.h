#pragma once

#include <memory>
#include <alnyr_thread_pool.h>

namespace alnyr
{
	using alnyrLoadThread = alnyrThreadPool<1u>;

	class alnyrScene;
	class alnyrEngine;
	class alnyrResourceGroup;
	
	class alnyrSceneManager
	{
	private:
		alnyrScene* load_scene_;
		alnyrScene* current_scene_;
		alnyrLoadThread load_thread_;
		std::future<bool> is_load_complete_;

	public:
		alnyrSceneManager();
		~alnyrSceneManager();

		void SceneInitialize();
		void SceneUpdate();
		void SceneRender();
		void SceneUninitialize();

		static void LoadNextSceneResource(std::function<bool(const std::unique_ptr<alnyrResourceGroup>&)> next_resource_group);
		static void LoadNextSceneResourceAndSetObject(std::function<bool(alnyrScene*)> next_scene);
		static void LoadLoadingSceneResourceAndSetObject(std::function<bool(alnyrScene*)> next_scene);

		static void SetFadeOutCallBack(std::function<bool()> fade_out_callback);
		static void SetFadeInCallBack(std::function<bool()> fade_in_callback);
	};
}