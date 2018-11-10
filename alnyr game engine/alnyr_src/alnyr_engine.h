#pragma once

#include <functional>
#include <type_traits>
#include <alnyr_rendering_dest.h>

namespace alnyr
{
	class alnyrScene;
	class alnyrSceneManager;
	class alnyrEngine
	{
		friend alnyrEngine* CreateEngine();
		friend void TerminateEngine(alnyrEngine**);
	private:
		alnyr::alnyrRenderingDest* window_;
		alnyrSceneManager* scene_manager_;

	private:
		alnyrEngine() : window_(nullptr), scene_manager_(nullptr) {}
		~alnyrEngine() {}

	public:
		bool Initialize();
		void Run();
		void Uninitialize();

		template<class Scene> void SetStartScene()
		{
			SetStartScene(new Scene);
		}

	private:
		void SetStartScene(alnyrScene* scene);
	};
}