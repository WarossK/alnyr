#pragma once

#include <string>
#include <boost/container/flat_map.hpp>
#include <srima_src/srima_render_resource.h>

namespace srima
{
	class srimaRenderResourceManager
	{
		boost::container::flat_map<std::string, srimaRenderResource*> resources_;
	};
}