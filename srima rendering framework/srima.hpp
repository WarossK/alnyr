#pragma once

#include <srima_src/srima_window.h>

namespace srima
{
	namespace vulkan {} //reserve

	namespace d3d12
	{
		bool InitializePipeline(srima::window::srimaWindow* window);

		void PopulateCommandList();
		void WaitNextFrame();

		void Uninitialize();
	}
}