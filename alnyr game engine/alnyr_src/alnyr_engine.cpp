#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>

bool alnyr::alnyrEngine::Initialize()
{
	window_ = alnyrCreateWindow("alnyr", 1280u, 720u);
	scene_manager_ = new alnyrSceneManager;

	if (!srima::Initialize(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	while (window_->ProcessMessage())
	{
		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();

		srima::Execute();
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