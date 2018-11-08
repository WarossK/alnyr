#include <alnyr_scene_manager.h>
#include <alnyr_scene.h>

static alnyr::alnyrSceneManager* manager = nullptr;

alnyr::alnyrSceneManager::alnyrSceneManager()
{
	manager = this;
}

alnyr::alnyrSceneManager::~alnyrSceneManager()
{
	manager = nullptr;
}

void alnyr::alnyrSceneManager::SceneInitialize()
{
	;
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

void alnyr::alnyrSceneManager::SetScene(alnyrScene* scene)
{
	current_scene_ = scene;
}

void alnyr::alnyrSceneManager::SceneChange()
{
	//manager->;
}
