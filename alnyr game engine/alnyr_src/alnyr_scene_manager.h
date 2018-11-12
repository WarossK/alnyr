#pragma once

#include <memory>
#include <alnyr_thread_pool.h>
#include <numeric>

#undef max

namespace alnyr
{
	using alnyrLoadThread = alnyrThreadPool<1u>;

	class alnyrScene;
	class alnyrEngine;
	class alnyrResourceGroup;

	enum SceneState
	{
		eUpdate
		, eFadeIn
		, eFadeOut
		, eLoading
	};

	class alnyrSceneManager
	{
		friend void LoadNextSceneResource(alnyrScene* next_scene);
		friend void SceneChange(uint32_t, uint32_t);
	private:
		alnyrScene* load_scene_;
		alnyrScene* current_scene_;
		alnyrScene* next_scene_;

		alnyrLoadThread load_thread_;

		SceneState scene_state_;

		uint32_t fade_frame_;
		float fade_percentage_;

		uint32_t fade_in_frame_;
		uint32_t fade_out_frame_;

		//std::function<float(uint32_t)> fade_in_callback;
		//std::function<float(uint32_t)> fade_out_callback;

	public:
		alnyrSceneManager();
		~alnyrSceneManager();

		void SceneInitialize();
		void SceneUpdate();
		void SceneRender();
		void SceneUninitialize();

		float GetFadePercentage();

		//static void SetFadeOutCallBack(std::function<float(uint32_t)> fade_out_callback);
		//static void SetFadeInCallBack(std::function<float(uint32_t)> fade_in_callback);
	};

	//‰B•Á‚µ‚Ä‚¥¥¥¥
	void LoadNextSceneResource(alnyrScene* next_scene);

	template<class NextScene, class...NextSceneArgs> static void LoadNextSceneResourceOnBackground(NextSceneArgs...args)
	{
		LoadNextSceneResource(new NextScene(args...));
	}

	void SceneChange(uint32_t fade_out_frame = std::numeric_limits<uint32_t>::max(), uint32_t fade_in_frame = std::numeric_limits<uint32_t>::max());
}