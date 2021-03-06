#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>

namespace srima
{
	struct srimaVertexBufferViewState
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_;
		uint32_t stride_;
		uint32_t size_in_bytes_;
	};

	class srimaVertexBuffer
	{
		friend srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);
		friend srimaVertexBuffer CreateInstancingVertexBuffer(std::array<std::tuple<void*, uint32_t, uint32_t>, 2u> ptrs);
	private:
		std::vector<srimaVertexBufferViewState> vertex_buffer_view_state_;

	public:
		void Remap(uint32_t index, void* ptr, uint32_t size)
		{
			void* gpu_ptr;
			D3D12_RANGE range { 0, 0 };
			if (FAILED(vertex_buffer_view_state_[index].vertex_buffer_->Map(0u, &range, &gpu_ptr)))
			{
				throw std::runtime_error("gpu memory map error.");
			}

			memcpy(gpu_ptr, ptr, size);
			vertex_buffer_view_state_[index].vertex_buffer_->Unmap(0u, nullptr);
		}

		const std::vector<D3D12_VERTEX_BUFFER_VIEW> GetVertexBufferView()
		{
			std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views;
			vertex_buffer_views.resize(vertex_buffer_view_state_.size());

			const auto size = vertex_buffer_view_state_.size();
			for (uint32_t i = 0u; i < size; ++i)
			{
				vertex_buffer_views[i].BufferLocation = vertex_buffer_view_state_[i].vertex_buffer_->GetGPUVirtualAddress();
				vertex_buffer_views[i].StrideInBytes = vertex_buffer_view_state_[i].stride_;
				vertex_buffer_views[i].SizeInBytes = vertex_buffer_view_state_[i].size_in_bytes_;
			}

			return vertex_buffer_views;
		}
	};
}