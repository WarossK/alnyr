#include <alnyr_scene_manager.h>
#include <alnyr_scene.h>

#undef max

static alnyr::alnyrSceneManager* manager = nullptr;

class alnyrDefaultLoadScene : public alnyr::alnyrScene
{
	void ResourceLoad(const std::unique_ptr<alnyr::alnyrResourceGroup>&) {}
	void Initialize() {}
};

alnyr::alnyrSceneManager::alnyrSceneManager() : fade_frame_(0u)
{
	if (manager) throw std::runtime_error("scene manager is already created.");
	manager = this;
	load_scene_ = new alnyrDefaultLoadScene;
}

alnyr::alnyrSceneManager::~alnyrSceneManager()
{
	manager = nullptr;
	delete load_scene_;
	load_scene_ = nullptr;
}

void alnyr::alnyrSceneManager::SceneInitialize()
{
	current_scene_->ObjectInitialize();
}

void alnyr::alnyrSceneManager::SceneUpdate()
{
	switch (scene_state_)
	{
	case eUpdate:
		current_scene_->ObjectUpdate();
		break;

	case eFadeIn:
		if (fade_in_frame_)
		{
			fade_percentage_ = fade_frame_++ / static_cast<float>(fade_in_frame_);
		}
		else
		{
			fade_percentage_ = 1.0f;
		}

		if (fade_percentage_ >= 1.0f)
		{
			scene_state_ = eUpdate;
			fade_percentage_ = 0.0f;
		}
		break;

	case eFadeOut:
		if (fade_out_frame_)
		{
			fade_percentage_ = fade_frame_++ / static_cast<float>(fade_out_frame_);
		}
		else
		{
			fade_percentage_ = 1.0f;
		}

		if (fade_percentage_ >= 1.0f)
		{
			scene_state_ = eFadeIn;
			fade_percentage_ = 0.0f;

			std::swap(current_scene_, next_scene_);
			delete next_scene_;
			next_scene_ = nullptr;

			if (!load_thread_.IsProcessComplete())
			{
				scene_state_ = eLoading;
			}
		}
		break;

	case eLoading:
		load_scene_->ObjectUpdate();
		break;

	default:
		throw std::runtime_error("scene state error.");
		break;
	}
}

void alnyr::alnyrSceneManager::SceneRender()
{
	switch (scene_state_)
	{
	case eLoading:
		load_scene_->RenderExecute();
		break;

	default:
		current_scene_->RenderExecute();
		break;
	}
}

void alnyr::alnyrSceneManager::SceneUninitialize()
{
	current_scene_->ObjectUninitialize();
}

float alnyr::alnyrSceneManager::GetFadePercentage()
{
	return fade_percentage_;
}

void alnyr::LoadNextSceneResource(alnyrScene * next_scene)
{
	manager->next_scene_ = next_scene;
	manager->load_thread_.Enqueue(&alnyrScene::CallResouceLoad, next_scene);
	manager->load_thread_.Enqueue(&alnyrScene::CallInitialize, next_scene);
}

void alnyr::SceneChange(uint32_t fade_out_frame, uint32_t fade_in_frame)
{
	manager->fade_out_frame_ = fade_out_frame == std::numeric_limits<uint32_t>::max() ? manager->fade_out_frame_ : fade_out_frame;
	manager->fade_in_frame_ = fade_in_frame == std::numeric_limits<uint32_t>::max() ? manager->fade_in_frame_ : fade_in_frame;
	manager->scene_state_ = eFadeOut;
}
