#pragma once

#include <srima_src/srima_window.h>

#define SRIMA_ULTIMATE_BETAGAKI

//current : single threaded. 

namespace srima
{
	bool Initialize(srima::window::srimaWindow* window);

	void Present();

	void Uninitialize();
}