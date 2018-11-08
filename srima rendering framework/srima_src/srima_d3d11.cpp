#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

struct RenderTarget
{
	std::vector<ComPtr<ID3D11RenderTargetView>> render_target_view;
	std::vector<ComPtr<ID3D11ShaderResourceView>> render_target_shader_resource_view;
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;
};

struct Properties
{
	D3D11_VIEWPORT viewport;
	D3D11_RECT scissor_rect;
};

void CreateDeviceAndSwapchain(srima::window::srimaWindow* window)
{
	HRESULT result;
	uint32_t create_device_flags = 0;
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
	std::vector<D3D_DRIVER_TYPE> driver_types;
	DXGI_SWAP_CHAIN_DESC swapchain_desc;

#if defined(_DEBUG)
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	driver_types.push_back(D3D_DRIVER_TYPE_HARDWARE);
	driver_types.push_back(D3D_DRIVER_TYPE_WARP);
	driver_types.push_back(D3D_DRIVER_TYPE_REFERENCE);

	SecureZeroMemory(&swapchain_desc, sizeof(swapchain_desc));

	auto width = window->GetWidth();
	auto height = window->GetHeight();

	swapchain_desc.BufferCount = 1;
	swapchain_desc.BufferDesc.Width = width;
	swapchain_desc.BufferDesc.Height = height;
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.OutputWindow = window->GetWindowHandle();
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.Windowed = true;
	swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.Flags = 0;

	swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;

	result = S_OK;
	for (auto driver_type : driver_types)
	{
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driver_type,
			nullptr,
			create_device_flags,
			&feature_level,
			1,
			D3D11_SDK_VERSION,
			&swapchain_desc,
			swapchain.GetAddressOf(),
			device.GetAddressOf(),
			nullptr,
			device_context.GetAddressOf());

		if (SUCCEEDED(result)) break;
	}

	if (FAILED(result))
	{
		MessageBox(nullptr, "swap chain initialize failure.", "error!", MB_OK);
		return false;
	}

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D11Debug> d3d_debug;
	result = device.As(&d3d_debug);
	if (SUCCEEDED(result))
	{
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3d_info_queue;
		result = d3d_debug.As(&d3d_info_queue);
		if (SUCCEEDED(result))
		{
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
			};
			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3d_info_queue->AddStorageFilterEntries(&filter);
		}
	}
#endif
	return true;
}