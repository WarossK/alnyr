#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <alnyr.hpp>
#include <alnyr_src/alnyr_scene_parameter.h>

int main(int, char**)
{
	auto engine = alnyr::CreateEngine();

	alnyr::window::alnyrSetAppActiveCallBack([] { std::cout << "app active." << std::endl; });
	alnyr::window::alnyrSetKeyInputCallBack([](LONG, WPARAM, LPARAM) { std::cout << "key callback." << std::endl; });
	alnyr::window::alnyrSetMouseInputCallBack([](LONG, WPARAM, LPARAM l)
	{
		//std::cout << "mouse x : " << LOWORD(l) << ", y : " << HIWORD(l) << std::endl;
	});

	alnyr::alnyrSceneParameter param;
	param.initialize = [](alnyr::alnyrScene*) {};
	param.uninitialize = [](alnyr::alnyrScene*) {};
	param.resource_group = nullptr;

	if (engine->Initialize())
	{
		engine->SetStartScene(&param);
		engine->Run();
	}

	alnyr::TerminateEngine();

	return 0;
}