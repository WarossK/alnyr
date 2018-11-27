#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>
#include <srima_src/srima_render_resource.h>
#include <chrono>
#include <random>

#undef max

std::vector<Vector3> ins;

bool alnyr::alnyrEngine::Initialize()
{
	window_ = alnyrCreateWindow("alnyr", 1280u, 720u);
	scene_manager_ = new alnyrSceneManager;

	if (!srima::Initialize(window_)) return false;

	return true;
}

#define thresd_num 2
void alnyr::alnyrEngine::Run()
{
	alnyrThreadPool<thresd_num> tp;
	const auto& width = window_->GetWidth();
	const auto& height = window_->GetHeight();
	const uint32_t kInstanceNum = width * height;
	std::vector<srima::srimaRenderResource*> render_resources;
	std::mt19937 mt;
	std::random_device rnd;
	mt.seed(rnd());

	std::uniform_real_distribution rndom(-1.0f, 1.0f);

	render_resources.emplace_back(new srima::srimaTriangleSample);

	for (auto&& render_resource : render_resources)
	{
		render_resource->Initialize();
	}

	ins.resize(kInstanceNum);

	auto start_time = std::chrono::system_clock::now();
	uint32_t frame_count = 0u;
	const auto xxx = 2.0f / width;
	const auto yyy = 2.0f / height;
	Vector2 pos{};

	while (window_->ProcessMessage())
	{
#if true
		auto i = 0;
		for (auto&& itr : ins)
		{
			itr.x = std::fmodf(xxx * i, 2.0f) + rndom(mt);
			itr.y = (i / width * -yyy) + rndom(mt);
			itr.z = 0.0f;
			++i;
		}
#elif true
		auto i = 0;
		for (auto&& itr : ins)
		{
			itr.x = std::fmodf(xxx * i, 2.0f);
			itr.y = (i / width * -yyy);
			itr.z = 0.0f;
			++i;
	}
#else
		auto i = 0;
		for (auto&& itr : ins)
		{
			itr.x = std::fmodf(xxx * i, 2.0f);
			itr.y = (i / width * -yyy);
			//itr.z = 0.0f;
			++i;
		}
#endif

		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();

		for (auto&& rr : render_resources)
		{
			rr->UpdateInstance(1u, sizeof(Vector3), ins.data(), static_cast<uint32_t>(ins.size()));
		}

		srima::Execute(render_resources);

		auto end_time = std::chrono::system_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

		if (dur >= 1'000'000'000)
		{
			char s[256];
			sprintf_s(s, "[vertex num : %d] fps : %dfps", kInstanceNum * 4u, frame_count);
			SetWindowText(window_->GetHandle<HWND>(), s);
			frame_count = 0u;
			start_time = std::chrono::system_clock::now();
		}

		++frame_count;
}

	for (auto&& render_resource : render_resources)
	{
		delete render_resource;
		render_resource = nullptr;
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