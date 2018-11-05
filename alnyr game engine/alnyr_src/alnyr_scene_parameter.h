#pragma once

#include <functional>
#include <alnyr_scene.h>
#include <alnyr_resource_group.h>

namespace alnyr
{
	struct alnyrSceneParameter
	{
		std::function<void(alnyrScene*)> initialize;
		std::function<void(alnyrScene*)> uninitialize;
		alnyrResourceGroup* resource_group;
	};
}