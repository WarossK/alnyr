#pragma once

#include <wrl/client.h>
#include <d3d12.h>

namespace srima
{
	namespace thread
	{
		class srimaGraphicsCommandList
		{
		private:
			ID3D12GraphicsCommandList** command_lists;

		public:
			srimaGraphicsCommandList(
				const Microsoft::WRL::ComPtr<ID3D12Device4>& device,
				const Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator,
				unsigned int command_num);
			~srimaGraphicsCommandList();
			ID3D12GraphicsCommandList** GetCommandLists();
		};
	}
}