#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace srima
{
	class srimaRenderPipeline
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_state_;

	public:
		
	};
}