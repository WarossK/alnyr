#include "srima_render_pipeline.h"

struct RenderPipelineImpl : public srima::srimaRenderPipeline
{
	RenderPipelineImpl(){}
	const Microsoft::WRL::ComPtr<ID3D12PipelineState>& operator->() override { return pipeline_state_; }
};

srima::srimaRenderPipeline * srima::CreateRenderPipeline2D(const srimaShaderAsset*)
{
	auto rpl = new RenderPipelineImpl;
	auto pl = *reinterpret_cast<Microsoft::WRL::ComPtr<ID3D12PipelineState>*>(rpl);

	//plに対して2Dパイプラインステート作成

	return rpl;
}

srima::srimaRenderPipeline * srima::CreateRenderPipeline3D(const srimaShaderAsset*)
{
	return nullptr;
}

srima::srimaRenderPipeline * srima::CreateRenderPipeline(const srimaShaderAsset*, std::function<void(const srimaShaderAsset*, D3D12_GRAPHICS_PIPELINE_STATE_DESC&)> descrepter)
{
	return nullptr;
}

void srima::DestroyRenderPipeline(srimaRenderPipeline * pipeline_obj)
{
	delete pipeline_obj;
}
