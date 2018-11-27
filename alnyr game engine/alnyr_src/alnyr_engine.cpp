#include <alnyr_engine.h>
#include <srima.hpp>
#include <alnyr_scene.h>
#include <alnyr_scene_manager.h>
#include <srima_src/srima_render_resource.h>
#include <chrono>

#undef max

std::vector<Vector3> ins;

bool alnyr::alnyrEngine::Initialize()
{
	window_ = alnyrCreateWindow("alnyr", 1280u, 720u);
	scene_manager_ = new alnyrSceneManager;

	if (!srima::Initialize(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	constexpr uint32_t kInstanceNum = 1'150'000u;
	std::vector<srima::srimaRenderResource*> render_resources;

	render_resources.emplace_back(new srima::srimaTriangleSample);

	for (auto&& render_resource : render_resources)
	{
		render_resource->Initialize();
	}

	ins.resize(kInstanceNum);

	float x = 0.0f;
	int y = 0u;
	for (auto&& i : ins)
	{
		auto xxx = 2.0f / 1280.0f;
		auto yyy = 2.0f / 720.0f;
		i.x = std::fmodf(xxx * x, 2.0f);
		i.y = y * -yyy;
		i.z = 0.0f;

		++x;
		y = static_cast<int>(x / 1280.0f);
	}

	auto start_time = std::chrono::system_clock::now();
	uint32_t frame_count = 0u;

	while (window_->ProcessMessage())
	{
		for (auto&& rr : render_resources)
		{
			rr->UpdateInstance(1u, sizeof(Vector3), ins.data(), static_cast<uint32_t>(ins.size()));
		}

		scene_manager_->SceneUpdate();
		scene_manager_->SceneRender();

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