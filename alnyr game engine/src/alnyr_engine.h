#pragma once

#include <memory>
#include "alnyr_scene_manager.h"
#include "../../srima rendering framework/src/srima_window.h"

namespace alnyr
{
	class alnyrEngine
	{
		friend alnyrEngine* CreateEngine();
		friend void TerminateEngine();
	private:
		srima::window::srimaWindow* window_;
		alnyrSceneManager* scene_manager_;

	private:
		alnyrEngine() {}
		~alnyrEngine() {}

	public:
		bool Initialize();
		void Run();
		void Uninitialize();

		template<class Scene>
		void SetStartScene()
		{
			static_assert(std::is_base_of_v<alnyrScene, Scene>, "required : derived alnyrScene class.");
			scene_manager_->SetScene(new Scene);
		}
	};
}