#include <alnyr_scene_manager.h>
#include <alnyr_scene.h>

static alnyr::alnyrSceneManager* manager = nullptr;

alnyr::alnyrSceneManager::alnyrSceneManager()
{
	if (manager) throw std::runtime_error("scene manager is already created.");
	manager = this;
}

alnyr::alnyrSceneManager::~alnyrSceneManager()
{
	manager = nullptr;
}

void alnyr::alnyrSceneManager::SceneInitialize()
{
	current_scene_->ObjectInitialize();
}

void alnyr::alnyrSceneManager::SceneUpdate()
{
	current_scene_->ObjectUpdate();
}

void alnyr::alnyrSceneManager::SceneRender()
{
	current_scene_->GetResourceGroup();//todo
}

void alnyr::alnyrSceneManager::SceneUninitialize()
{
	current_scene_->ObjectUninitialize();
}

void alnyr::alnyrSceneManager::LoadNextSceneResource(std::function<bool(const std::unique_ptr<alnyrResourceGroup>&)> next_resource_group)
{
	auto rg = std::make_unique<alnyrResourceGroup>();
	manager->is_load_complete_ = manager->load_thread_.Enqueue(next_resource_group, rg);


}

void alnyr::alnyrSceneManager::LoadNextSceneResourceAndSetObject(std::function<bool(alnyrScene*)> next_scene)
{
	auto next = new alnyrScene;
	next->SetResourceGroup(std::forward<std::unique_ptr<alnyrResourceGroup>>(std::make_unique<alnyrResourceGroup>()));
	manager->is_load_complete_ = manager->load_thread_.Enqueue(next_scene, next);

}

void alnyr::alnyrSceneManager::LoadLoadingSceneResourceAndSetObject(std::function<bool(alnyrScene*)> next_scene)
{
}
