#include "alnyr_engine.h"
#include "../../srima rendering framework/srima.hpp"

bool alnyr::alnyrEngine::Initialize()
{
	window_ = srima::window::srimaCreateWindow("alnyr", 1280u, 720u);

	if (!srima::d3d12::Initialize(window_)) return false;

	return true;
}

void alnyr::alnyrEngine::Run()
{
	while (window_->ProcessMessage())
	{

	}
}

void alnyr::alnyrEngine::Uninitialize()
{
	srima::d3d12::Uninitialize();
	srima::window::srimaTerminateWindow(window_);
}
