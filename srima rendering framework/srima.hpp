#pragma once

#include <srima_src/srima_window.h>

namespace srima
{
	namespace vulkan {} //reserve

	namespace d3d12
	{
		bool Initialize(srima::window::srimaWindow* window);
		void TestRender();
		void Present();
		void Uninitialize();
	}
}