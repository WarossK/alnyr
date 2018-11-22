#pragma once

#include <srima.hpp>
#include <srima_src/srima_vertex_types.h>

namespace srima
{
	class srimaRenderResource
	{
	protected:
		srimaRootSignature root_signature_;
		srimaConstantBuffer constant_buffer_;
		srimaGraphicsPipelineState pipeline_state_;

	public:
		virtual ~srimaRenderResource() {}
		const srimaRootSignature& GetRootSignature() const { return root_signature_; }
		const srimaGraphicsPipelineState& GetPipelineState() const { return pipeline_state_; }

		virtual void Initialize() {}

		template<class InstanceDataType, size_t N>
		void UpdateDataPerInstance(InstanceDataType(&data)[N])
		{
			
		}

		template<class InstanceDataType>
		void UpdateDataPerInstance(std::vector<InstanceDataType> data)
		{

		}

		template<class InstanceDataType, size_t N>
		void UpdateDataPerInstance(std::array<InstanceDataType, N> data)
		{

		}

		virtual void StackCommand(const srimaCommandList&) = 0;
	};

	class srimaTriangleSample : public srimaRenderResource
	{
	private:
		srimaVertexBuffer vertex_buffer_;
		uint32_t vertex_num_;
		uint32_t instance_num_;

	public:
		srimaTriangleSample()
		{
		}

		void Initialize() override
		{
			srimaShaderFilePaths shader_file_paths;
			shader_file_paths.VS = "vertex_test.hlsl";
			shader_file_paths.PS = "pixel_test.hlsl";

#if false
			shader_file_paths.VS = "vertex_instance_test.hlsl";

			__declspec(align(16)) struct vertex1
			{
				Vector3 pos;
				Color col;
			};

			D3D12_INPUT_ELEMENT_DESC ied[]
			{
				{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"RELATIVE_POS", 0, DXGI_FORMAT_R32G32B32_FLOAT,	1,  0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
			};

			vertex1 vertices[]
			{
				{ DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			};

			Vector3 positions[]
			{
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
			};

			D3D12_INPUT_LAYOUT_DESC ild;
			ild.pInputElementDescs = ied;
			ild.NumElements = static_cast<uint32_t>(std::extent_v<decltype(ied)>);

			vertex_buffer_ = CreateInstancingVertexBuffer(vertices, positions);
			pipeline_state_ = helper::CreatePSO(ild, root_signature_, shader_file_paths);

			vertex_num_ = std::extent_v<decltype(vertices)>;
			instance_num_ = std::extent_v<decltype(positions)>;

			//↑ オリジナル頂点,	↓ 定義済み頂点(注意 : 入力PositionのセマンティクスがSV_POSITION)
#else
			//std::vector<srima::srimaVertexPositionColor> vertices
			//{
			//	{ DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			//	{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			//	{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			//};

			srima::srimaVertexPositionColor vertices2[]
			{
				{ DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			};

			vertex_buffer_ = CreateVertexBuffer(vertices2);

			pipeline_state_ = helper::CreatePSO<srima::srimaVertexPositionColor>(root_signature_, shader_file_paths);

			vertex_num_ = std::extent_v<decltype(vertices2)>;
			instance_num_ = 10u;
#endif
			root_signature_ = helper::CreateEmptyRootSignature();
		}

		void StackCommand(const srimaCommandList& cl) override
		{
			cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto vbvs = vertex_buffer_.GetVertexBufferView();
			const auto instance_array_size = static_cast<uint32_t>(vbvs.size());
			cl->IASetVertexBuffers(0u, instance_array_size, vbvs.data());
			cl->DrawInstanced(vertex_num_, instance_num_, 0u, 0u);
		}
	};
}