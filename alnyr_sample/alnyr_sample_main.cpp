#include "../alnyr game engine/alnyr.hpp"
#include <iostream>

int main(int, char**)
{
	auto engine = alnyr::CreateEngine();

	srima::window::srimaSetAppActiveCallBack([] {std::cout << "app active." << std::endl; });
	srima::window::srimaSetKeyInputCallBack([](LONG, WPARAM, LPARAM) {std::cout << "key callback." << std::endl; });
	srima::window::srimaSetMouseInputCallBack([](LONG, WPARAM, LPARAM l) 
	{ 
		auto x = LOWORD(l);
		auto y = HIWORD(l);
		//std::cout << "mouse x : " << x << ", y : " << y << std::endl; 
	});

	if (engine->Initialize())
	{
		//engine->SetStartScene<int>();
		engine->Run();
	}

	alnyr::TerminateEngine();

	return 0;
}