#pragma once

#include <Windows.h>
#include <functional>

namespace srima
{
	namespace window
	{
		class srimaWindow
		{
		protected:
			void* handle_;
			unsigned int width_;
			unsigned int height_;

		public:
			virtual bool ProcessMessage() = 0;
			void* GetHandle() { return handle_; }
			const unsigned int& GetWidth() { return width_; }
			const unsigned int& GetHeight() { return height_; }
		};

		void srimaSetAppActiveCallBack(std::function<void()> app_active_callback);
		void srimaSetKeyInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> key_input_callback);
		void srimaSetMouseInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> mouse_input_callback);
		srimaWindow* srimaCreateWindow(const char* title, unsigned int width, unsigned int height);//windows
		srimaWindow* srimaCreateOtherWindow(const char* title, unsigned int width, unsigned int height);//reserve
		void srimaTerminateWindow(srimaWindow* window);
	}
}