#pragma once

#include <srima.hpp>

namespace srima
{
	class srimaRenderResource
	{
	private:
		srimaRootSignature root_signature_;
		srimaGraphicsPipelineState pipeline_state_;

	public:
		const srimaRootSignature& GetRootSignature() const { return root_signature_; }
		const srimaGraphicsPipelineState& GetPipelineState() const { return pipeline_state_; }
		virtual void StackCommand() = 0;
	};
}