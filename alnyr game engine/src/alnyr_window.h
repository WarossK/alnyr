#pragma once

#include <Windows.h>
#include <functional>

namespace alnyr
{
	namespace window
	{
		class alnyrWindow
		{
		protected:
			void* handle_;

		public:
			virtual bool ProcessMessage() = 0;
			void* GetHandle() { return handle_; }
		};

		void alnyrSetAppActiveCallBack(std::function<void()> app_active_callback);
		void alnyrSetKeyInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> key_input_callback);
		void alnyrSetMouseInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> mouse_input_callback);
		alnyrWindow* alnyrCreateWindow(const char* title, unsigned int width, unsigned int height);//windows
		alnyrWindow* alnyrCreateOtherWindow(const char* title, unsigned int width, unsigned int height);//resserve
		void alnyrTerminateWindow(alnyrWindow* window);
	}
}