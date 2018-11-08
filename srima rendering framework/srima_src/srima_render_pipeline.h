#pragma once

#include <functional>
#include <d3d12.h>
#include <wrl/client.h>
#include <srima_shader_asset.h>

namespace srima
{
	struct srimaRenderPipeline
	{
		friend void DestroyRenderPipeline(srimaRenderPipeline*);
	protected:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_state_;

	public:
		srimaRenderPipeline() {}

		virtual const Microsoft::WRL::ComPtr<ID3D12PipelineState>& operator->() = 0;

	protected:
		~srimaRenderPipeline() {}
	};

	srimaRenderPipeline* CreateRenderPipeline2D(const srimaShaderAsset*);
	srimaRenderPipeline* CreateRenderPipeline3D(const srimaShaderAsset*);
	srimaRenderPipeline* CreateRenderPipeline(const srimaShaderAsset*, std::function<void(const srimaShaderAsset*, D3D12_GRAPHICS_PIPELINE_STATE_DESC&)> descrepter);

	void DestroyRenderPipeline(srimaRenderPipeline* pipeline_obj);

}