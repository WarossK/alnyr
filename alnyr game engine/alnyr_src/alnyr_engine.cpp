#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>
#include <srima_src/srima_render_resource.h>
#include <chrono>
#include <random>

#undef max

bool alnyr::alnyrEngine::Initialize()
{
	window_ = alnyrCreateWindow("alnyr", 1280u, 720u);
	scene_manager_ = new alnyrSceneManager;

	if (!srima::Initialize(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	std::vector<srima::srimaRenderResource*> render_resources;
	int frame_count = 0u;
	auto start_time = std::chrono::system_clock::now();
	while (window_->ProcessMessage())
	{
		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();
		srima::Execute(render_resources);

		auto end_time = std::chrono::system_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

		if (dur >= 1'000'000'000)
		{
			char s[256];
			sprintf_s(s, "fps : %dfps", frame_count);
			SetWindowText(window_->GetHandle<HWND>(), s);
			frame_count = 0u;
			start_time = std::chrono::system_clock::now();
		}

		++frame_count;
	}
}

void alnyr::alnyrEngine::Uninitialize()
{
	delete scene_manager_;
	scene_manager_ = nullptr;
	srima::Uninitialize();
	alnyrTerminateWindow(&window_);
}

void alnyr::alnyrEngine::SetStartScene(alnyrScene* scene)
{
	if (!scene) throw std::runtime_error("scene is nullptr.");
	LoadNextSceneResource(scene);
	SceneChange(0u, 0u);
	scene_manager_->SceneUpdate();
}