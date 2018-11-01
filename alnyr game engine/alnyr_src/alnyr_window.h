#pragma once

#include <functional>
#include <Windows.h>

namespace alnyr
{
	namespace window
	{
		void alnyrSetAppActiveCallBack(std::function<void()> app_active_callback);
		void alnyrSetKeyInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> key_input_callback);
		void alnyrSetMouseInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> mouse_input_callback);
	}
}