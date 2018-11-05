#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>

bool alnyr::alnyrEngine::Initialize()
{
	window_ = srima::window::srimaCreateWindow("alnyr", 1280u, 720u);

	if (!srima::d3d12::InitializePipeline(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	while (window_->ProcessMessage())
	{
		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();
		srima::d3d12::PopulateCommandList();
		srima::d3d12::WaitNextFrame();
	}
}

void alnyr::alnyrEngine::Uninitialize()
{
	srima::d3d12::Uninitialize();
	srima::window::srimaTerminateWindow(window_);
}

void alnyr::alnyrEngine::SetStartScene(alnyrSceneParameter* scene_parameter)
{
	if(!scene_parameter) throw std::runtime_error("scene parameter is nullptr.");
	scene_manager_->SetScene(new alnyrScene(scene_parameter));
}