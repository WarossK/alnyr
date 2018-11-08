#pragma once

#include <alnyr_src/alnyr_scene.h>

namespace alnyr
{
	template<class Scene>
	alnyrScene* CreateScene()
	{
		return new Scene(args...);
	}
}