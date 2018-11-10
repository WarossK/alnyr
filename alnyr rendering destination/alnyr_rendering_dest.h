#pragma once

#include <functional>
#include <cstdint>

namespace alnyr
{
	class alnyrRenderingDest
	{
	protected:
		void* handle_;
		unsigned int width_;
		unsigned int height_;

	public:
		virtual bool ProcessMessage() = 0;
		virtual void Uninitialize() = 0;
		void* GetHandle() { return handle_; }
		template<class HandleType> HandleType GetHandle() { return reinterpret_cast<HandleType>(handle_); }
		const unsigned int& GetWidth() { return width_; }
		const unsigned int& GetHeight() { return height_; }
	};

	//>window callback format for windows.
	//>message, wparam, lparam
	void alnyrSetProcessMessageCallBack(std::function<void(uint32_t, uint64_t, int64_t)> message_prosess_callback);

	alnyrRenderingDest* alnyrCreateWindow(const char* title, unsigned int width, unsigned int height);//windows
	alnyrRenderingDest* alnyrCreateOtherWindow(const char* title, unsigned int width, unsigned int height);//reserve
	void alnyrTerminateWindow(alnyrRenderingDest** window);
}