#pragma once

#include <functional>
#include <type_traits>

namespace srima { namespace window { class srimaWindow; } }
namespace alnyr
{
	class alnyrScene;
	class alnyrSceneManager;
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

		//内部でポインタをreleaseします。
		void SetStartScene(alnyrScene* scene);
	};
}