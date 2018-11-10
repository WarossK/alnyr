#include "../alnyr_rendering_dest.h"
#include <Windows.h>

class Window : public alnyr::alnyrRenderingDest
{
private:
	MSG window_message_;

public:
	Window(void* handle, uint32_t width, uint32_t height)
	{
		handle_ = handle;
		width_ = width;
		height_ = height;
		SecureZeroMemory(&window_message_, sizeof(MSG));
	}
	virtual bool ProcessMessage() override;
	virtual void Uninitialize() override;
};

std::function<void(uint32_t, uint64_t, int64_t)> callback = [](uint32_t, uint64_t, int64_t) {};

LRESULT __stdcall WindowProc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		callback(message, wparam, lparam);
	}

	return DefWindowProc(window_handle, message, wparam, lparam);
}

void alnyr::alnyrSetProcessMessageCallBack(std::function<void(uint32_t, uint64_t, int64_t)> message_process_callback)
{
	callback = message_process_callback;
}

alnyr::alnyrRenderingDest * alnyr::alnyrCreateWindow(const char * title, unsigned int width, unsigned int height)
{
	WNDCLASSEX window_class;

	auto instance = GetModuleHandle(nullptr);

	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = WindowProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance;
	window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	window_class.hIconSm = window_class.hIcon;
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	window_class.lpszMenuName = nullptr;
	window_class.lpszClassName = title;
	window_class.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&window_class))
	{
		MessageBox(nullptr, "RegisterClassEx() failed.", "Window Initialize failed.", MB_OK);
		return nullptr;
	}

	int window_style = WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME);

	RECT window_rectangle = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
	AdjustWindowRect(&window_rectangle, WS_OVERLAPPEDWINDOW, false);
	int screen_width = (SHORT)(window_rectangle.right - window_rectangle.left);
	int screen_height = (SHORT)(window_rectangle.bottom - window_rectangle.top);

	RECT desktop_rectangle;
	GetWindowRect(GetDesktopWindow(), &desktop_rectangle);
	int window_position_x = (desktop_rectangle.right - screen_width) / 2;
	if (window_position_x < 0) window_position_x = 0;
	int window_position_y = (desktop_rectangle.bottom - screen_height) / 2;
	if (window_position_y < 0) window_position_y = 0;

	auto window_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		title, title,
		window_style,
		window_position_x, window_position_y,
		screen_width, screen_height,
		nullptr, nullptr,
		instance, nullptr);

	if (!window_handle)
	{
		MessageBox(nullptr, "CreateWindowEx() failure.", "Window Initialize failure.", MB_OK);
		PostQuitMessage(0);
		return nullptr;
	}

	ShowWindow(window_handle, SW_SHOW);
	SetForegroundWindow(window_handle);
	SetFocus(window_handle);

	return new Window(window_handle, width, height);
}

alnyr::alnyrRenderingDest * alnyr::alnyrCreateOtherWindow(const char * title, unsigned int width, unsigned int height)
{
	return nullptr;
}

void alnyr::alnyrTerminateWindow(alnyrRenderingDest** window)
{
	auto& window_ptr = *window;

	window_ptr->Uninitialize();
	delete window_ptr;
	window_ptr = nullptr;
}

bool Window::ProcessMessage()
{
	while (PeekMessage(&window_message_, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&window_message_);
		DispatchMessage(&window_message_);

		if (window_message_.message == WM_QUIT) return false;
	}

	return true;
}

void Window::Uninitialize()
{
	PostQuitMessage(0);
	DestroyWindow(reinterpret_cast<HWND>(GetHandle()));
}
