#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>

namespace srima
{
	class srimaVertexBuffer
	{
		friend srimaVertexBuffer CreateVertexBuffer(void* vertices_start_ptr, uint32_t vertex_size, uint32_t vertex_array_size);
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_;//allocate
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_view_;//view

	public:
		const std::vector<D3D12_VERTEX_BUFFER_VIEW>& GetVertexBufferView() const
		{
			return vertex_buffer_view_;
		}
	};
}