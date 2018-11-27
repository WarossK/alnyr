#pragma once

#include <srima.hpp>
#include <srima_src/srima_vertex_types.h>

#undef max

namespace srima
{
	struct srimaConstantBufferPerInstance
	{
		Matrix model;
		Color color;
	};

	class srimaRenderResource
	{
	protected:
		uint32_t vertex_num_;
		uint32_t instance_num_;
		srimaRootSignature root_signature_;
		srimaConstantBuffer constant_buffer_;
		srimaGraphicsPipelineState pipeline_state_;

	public:
		virtual ~srimaRenderResource() {}
		const srimaRootSignature& GetRootSignature() const { return root_signature_; }
		const srimaGraphicsPipelineState& GetPipelineState() const { return pipeline_state_; }

		virtual void Initialize() {}
		virtual void StackCommand(const srimaCommandList&) = 0;

		void UpdateInstance(uint32_t index, uint32_t per_size, void* ptr, uint32_t arr_size)
		{
			UpdateDataPerInstance(index, per_size, ptr, arr_size);
			(index ? instance_num_ : vertex_num_) = arr_size;
			WaitForGpu();
		}

	private:
		virtual void UpdateDataPerInstance(uint32_t index, uint32_t per_size, void* ptr, uint32_t arr_size) {}
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
			srimaShaderFilePaths shader_file_paths;
			shader_file_paths.VS = "vertex_instance_test.hlsl";
			shader_file_paths.PS = "pixel_test.hlsl";

			root_signature_ = helper::CreateEmptyRootSignature();

			__declspec(align(16)) struct vertex1
			{
				Vector3 pos;
				Color col;
			};

			D3D12_INPUT_ELEMENT_DESC ied[]
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"RELATIVE_POS", 0, DXGI_FORMAT_R32G32B32_FLOAT,	1,  0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
			};

			auto xxx = 2.0f / 1280.0f;
			auto yyy = 2.0f / 720.0f;

			std::vector<vertex1> vertices
			{
				{ DirectX::XMFLOAT3(-1.0f,			1.0f,		0.0f), DirectX::XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-1.0f + xxx,	1.0f,		0.0f), DirectX::XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-1.0f,			1.0f - yyy,	0.0f), DirectX::XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-1.0f + xxx,	1.0f - yyy,	0.0f), DirectX::XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f) },
			};

			std::vector<Vector3> ins;
			ins.resize(1'150'000u);

			D3D12_INPUT_LAYOUT_DESC ild;
			ild.pInputElementDescs = ied;
			ild.NumElements = static_cast<uint32_t>(std::extent_v<decltype(ied)>);

			vertex_buffer_ = CreateInstancingVertexBuffer(vertices, ins);
			pipeline_state_ = helper::CreatePSO(ild, root_signature_, shader_file_paths);

			vertex_num_ = static_cast<uint32_t>(vertices.size());
			instance_num_ = static_cast<uint32_t>(ins.size());
		}

		void UpdateDataPerInstance(uint32_t index, uint32_t per_size, void* ptr, uint32_t arr_size) override
		{
			vertex_buffer_.Remap(index, ptr, arr_size * per_size);
		}

		void StackCommand(const srimaCommandList& cl) override
		{
			cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			auto vbvs = vertex_buffer_.GetVertexBufferView();

			const auto size = static_cast<uint32_t>(vbvs.size());
			for (uint32_t i = 0u; i < size; ++i)
			{
				auto in = &vbvs[i];
				cl->IASetVertexBuffers(i, 1u, in);
			}

			cl->DrawInstanced(vertex_num_, instance_num_, 0u, 0u);
		}
	};
}