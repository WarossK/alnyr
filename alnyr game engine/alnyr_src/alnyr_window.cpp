#include <alnyr_window.h>
#include <srima_src/srima_window.h>

void alnyr::window::alnyrSetAppActiveCallBack(std::function<void()> app_active_callback)
{
	srima::window::srimaSetAppActiveCallBack(app_active_callback);
}

void alnyr::window::alnyrSetKeyInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> key_input_callback)
{
	srima::window::srimaSetKeyInputCallBack(key_input_callback);
}

void alnyr::window::alnyrSetMouseInputCallBack(std::function<void(UINT, WPARAM, LPARAM)> mouse_input_callback)
{
	srima::window::srimaSetMouseInputCallBack(mouse_input_callback);
}
