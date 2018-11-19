#pragma once

#include <srima.hpp>
#include <srima_src/srima_vertex_types.h>

namespace srima
{
	class srimaRenderResource
	{
	protected:
		srimaRootSignature root_signature_;
		srimaGraphicsPipelineState pipeline_state_;

	public:
		virtual ~srimaRenderResource() {}
		const srimaRootSignature& GetRootSignature() const { return root_signature_; }
		const srimaGraphicsPipelineState& GetPipelineState() const { return pipeline_state_; }

		virtual void Initialize() {}

		void SetPSOAndRS(const srimaCommandList& cl)
		{
			cl->SetPipelineState(pipeline_state_.Get());
			cl->SetGraphicsRootSignature(root_signature_.Get());
		}

		virtual void StackCommand(const srimaCommandList&) = 0;
	};

	class srimaTriangleSample : public srimaRenderResource
	{
	private:
		srimaVertexBuffer vertex_buffer_;

	public:
		srimaTriangleSample()
		{
		}

		void Initialize() override
		{
			__declspec(align(16)) struct vertex1
			{
				Vector3 pos;
				Color col;
			};

			std::vector<D3D12_INPUT_ELEMENT_DESC> ied
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			std::vector<vertex1> vertices
			{
				vertex1{ DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
				vertex1{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
				vertex1{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			};

			vertex_buffer_ = CreateVertexBuffer(vertices.data(), sizeof(vertex1), static_cast<uint32_t>(vertices.size()));

			srimaShaderFilePaths shader_file_paths;
			shader_file_paths.VS = "vertex_test.hlsl";
			shader_file_paths.PS = "pixel_test.hlsl";

			D3D12_INPUT_LAYOUT_DESC ild;
			ild.NumElements = static_cast<uint32_t>(ied.size());
			ild.pInputElementDescs = ied.data();

			pipeline_state_ = helper::CreatePSO(ild, root_signature_, shader_file_paths);
			root_signature_ = helper::CreateEmptyRootSignature();
		}

		void StackCommand(const srimaCommandList& cl) override
		{
			cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto vbvs = vertex_buffer_.GetVertexBufferView();
			cl->IASetVertexBuffers(0u, static_cast<uint32_t>(vbvs.size()), vbvs.data());
			cl->DrawInstanced(3u, 1u, 0u, 0u);
		}
	};
}