#pragma once

#include <alnyr_src/alnyr_engine.h>

//公開する予定のヘッダ
//alnyr_resource_group			//ロードされたリソースのまとまり
//alnyr_game_object				//継承用
//alnyr_renderable_game_object	//継承用
//alnyr_behavior				//継承用

namespace alnyr
{
	alnyrEngine* CreateEngine();
	void TerminateEngine(alnyrEngine** engine);
}