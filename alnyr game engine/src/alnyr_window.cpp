#include "alnyr_window.h"
#include <memory>

struct WindowsWindow : alnyr::window::alnyrWindow
{
	MSG window_message_;
	bool ProcessMessage();
	void Uninitialize();

public:
	WindowsWindow(HWND window_handle)
	{
		handle_ = window_handle;
		SecureZeroMemory(&window_message_, sizeof(MSG));
	}
};

LRESULT __stdcall WindowProc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

std::function<void()> app_active = [] {};
std::function<void(UINT, WPARAM, LPARAM)> key_input = [](UINT, WPARAM, LPARAM) {};
std::function<void(UINT, WPARAM, LPARAM)> mouse_input = [](UINT, WPARAM, LPARAM) {};

void alnyr::window::alnyrSetAppActiveCallBack(std::function<void()> app_active_callback)
{
	app_active = app_active_callback;
}

void alnyr::window::alnyrSetKeyInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> key_input_callback)
{
	key_input = key_input_callback;
}

void alnyr::window::alnyrSetMouseInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> mouse_input_callback)
{
	mouse_input = mouse_input_callback;
}

alnyr::window::alnyrWindow* alnyr::window::alnyrCreateWindow(const char * title, unsigned int width, unsigned int height)
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

	return new WindowsWindow(window_handle);
}

LRESULT __stdcall WindowProc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
		app_active();
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		mouse_input(message, wparam, lparam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		key_input(message, wparam, lparam);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(window_handle, message, wparam, lparam);
	}

	return 0;
}

bool WindowsWindow::ProcessMessage()
{
	while (PeekMessage(&window_message_, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&window_message_);
		DispatchMessage(&window_message_);

		if (window_message_.message == WM_QUIT) return false;
	}

	return true;
}

void alnyr::window::alnyrTerminateWindow(alnyrWindow* window)
{
	PostQuitMessage(0);
	DestroyWindow(reinterpret_cast<HWND>(window->GetHandle()));
}
