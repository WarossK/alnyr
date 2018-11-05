#include <alnyr_scene_manager.h>
#include <alnyr_scene.h>

bool alnyr::alnyrSceneManager::SceneInitialize()
{
	return false;
}

void alnyr::alnyrSceneManager::SceneUpdate()
{
}

void alnyr::alnyrSceneManager::SceneRender()
{
}

void alnyr::alnyrSceneManager::SceneUninitialize()
{
}

void alnyr::alnyrSceneManager::SetScene(alnyrScene * scene)
{
	current_scene_ = scene;
}