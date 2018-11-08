#pragma once

#include <functional>
#include <alnyr_src/alnyr_scene.h>
#include <alnyr_src/alnyr_resource_group.h>

namespace alnyr
{
	class alnyrScene;
	class alnyrResourceGroup;
	struct alnyrSceneParameter
	{
		std::function<void(alnyrScene*)> initialize;
		std::function<void(alnyrScene*)> uninitialize;
		alnyrResourceGroup* resource_group;
	};
}