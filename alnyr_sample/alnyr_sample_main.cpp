#include <alnyr.hpp>
#include "alnyr_sample_scene.h"

int main(int, char**)
{
	auto engine = alnyr::CreateEngine();

	if (engine->Initialize())
	{
		engine->SetStartScene<SampleScene>();
		engine->Run();
	}

	alnyr::TerminateEngine(&engine);

	return 0;
}