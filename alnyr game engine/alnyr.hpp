#pragma once

#include <alnyr_src/alnyr_engine.h>
#include <alnyr_src/alnyr_window.h>

//公開する予定のヘッダ
//alnyr_scene_parameter			//sceneのinitializeとuninitializeとリソースグループの設定用構造体
//alnyr_resource_group			//ロードされたリソースのまとまり
//alnyr_game_object				//継承用
//alnyr_renderable_game_object	//継承用
//alnyr_behavior				//継承用

namespace alnyr
{
	alnyrEngine* CreateEngine();
	void TerminateEngine();
}