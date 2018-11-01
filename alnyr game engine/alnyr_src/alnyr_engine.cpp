#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>

bool alnyr::alnyrEngine::Initialize()
{
	window_ = srima::window::srimaCreateWindow("alnyr", 1280u, 720u);

	if (!srima::d3d12::Initialize(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	while (window_->ProcessMessage())
	{
		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();
		srima::d3d12::TestRender();
	}
}

void alnyr::alnyrEngine::Uninitialize()
{
	srima::d3d12::Uninitialize();
	srima::window::srimaTerminateWindow(window_);
}

void alnyr::alnyrEngine::SetScene(alnyrScene* scene)
{
	scene_manager_->SetScene(scene);
}