#include "..\alnyr.hpp"
#include <stdexcept>

static alnyr::alnyrEngine* singleton = nullptr;

alnyr::alnyrEngine * alnyr::CreateEngine()
{
	if (singleton) throw std::runtime_error("AlnyrEngine is singleton.");

	singleton = new alnyrEngine();

	return singleton;
}

void alnyr::TerminateEngine()
{
	if(!singleton) throw std::runtime_error("AlnyrEngine is not created.");

	singleton->Uninitialize();

	delete singleton;
	singleton = nullptr;
}
