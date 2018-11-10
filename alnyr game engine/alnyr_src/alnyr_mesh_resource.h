#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace alnyr
{
	class alnyrMeshResource
	{
	protected:
		std::string pipeline_name_;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_;
		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

	public:
		void SetPipelineName(std::string pipeline_name)
		{
			pipeline_name_ = pipeline_name;
		}

		const std::string& GetPipelineName() const
		{
			return pipeline_name_;
		}

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const
		{
			return vertex_buffer_view_;
		}
	};
}