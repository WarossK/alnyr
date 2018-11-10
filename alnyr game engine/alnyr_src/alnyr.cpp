#include <../alnyr.hpp>
#include <stdexcept>

static alnyr::alnyrEngine* singleton = nullptr;

alnyr::alnyrEngine * alnyr::CreateEngine()
{
	if (singleton) throw std::runtime_error("alnyrEngine is singleton.");

	singleton = new alnyrEngine();

	return singleton;
}

void alnyr::TerminateEngine(alnyrEngine** engine)
{
	if(!singleton) throw std::runtime_error("alnyrEngine is not created.");

	singleton->Uninitialize();

	delete singleton;
	singleton = nullptr;
	(*engine) = nullptr;
}
