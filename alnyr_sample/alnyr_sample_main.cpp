#include <alnyr.hpp>
#include "alnyr_sample_scene.h"
#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

void dbg() {}
void dbg(uint32_t alloc_num) { _CrtSetBreakAlloc(alloc_num); }

int main(int, char**)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	dbg();
#endif

	auto engine = alnyr::CreateEngine();

	if (engine->Initialize())
	{
		engine->SetStartScene<SampleScene>();
		engine->Run();
	}

	alnyr::TerminateEngine(&engine);

	return 0;
}