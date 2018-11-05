#include "srima_thread_pool.h"
#include <stdexcept>

srima::thread::srimaGraphicsCommandList::srimaGraphicsCommandList(
	const Microsoft::WRL::ComPtr<ID3D12Device4>& device,
	const Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator,
	unsigned int command_num)
{
	command_lists = static_cast<ID3D12GraphicsCommandList**>(malloc(sizeof(ID3D12GraphicsCommandList**) * command_num));

	if (FAILED(device->CreateCommandList(
		1,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(command_lists))))
	{
		throw std::runtime_error("srimaGraphicsCommandList create failure.");
	}

	for (unsigned int i = 0; i < command_num; ++i)
	{
		command_lists[i]->Close();
	}
}

srima::thread::srimaGraphicsCommandList::~srimaGraphicsCommandList()
{

}

ID3D12GraphicsCommandList ** srima::thread::srimaGraphicsCommandList::GetCommandLists()
{
	return command_lists;
}
