#pragma once

#include <alnyr_src/alnyr_engine.h>
#include <alnyr_src/alnyr_window.h>

namespace alnyr
{
	alnyrEngine* CreateEngine();
	void TerminateEngine();
}