#include <iostream>
#include <Windows.h>
#include <alnyr.hpp>

int main(int, char**)
{
	auto engine = alnyr::CreateEngine();

	alnyr::window::alnyrSetAppActiveCallBack([] { std::cout << "app active." << std::endl; });
	alnyr::window::alnyrSetKeyInputCallBack([](LONG, WPARAM, LPARAM) { std::cout << "key callback." << std::endl; });
	alnyr::window::alnyrSetMouseInputCallBack([](LONG, WPARAM, LPARAM l)
	{
		//std::cout << "mouse x : " << LOWORD(l) << ", y : " << HIWORD(l) << std::endl;
	});

	if (engine->Initialize())
	{
		//engine->SetStartScene<int>();
		engine->Run();
	}

	alnyr::TerminateEngine();

	return 0;
}