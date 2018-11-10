#pragma once

#include <vector>
#include <filesystem>
#include <alnyr_math.h>
#include <alnyr_src/alnyr_mesh_resource.h>

namespace alnyr
{
	class alnyrStaticMesh : public alnyrMeshResource
	{
	private:
		std::vector<uint8_t> vertices_;

	public:
		alnyrStaticMesh() { vertices_.clear(); }

		bool Initialize(std::filesystem::path model_path);
		template<class VertexType> bool Initialize(const std::vector<VertexType>& vertices)
		{
			auto size = vertices.size() * sizeof(VertexType);
			vertices_.resize(size);

			auto ptr = reinterpret_cast<uint8_t*>(vertices.data());

			for (uint32_t i = 0u; i < size; ++i)
			{
				vertices_[i] = ptr[i];
			}

			return Initialize();
		}

	private:
		bool Initialize(uint32_t vertex_size, uint32_t vertex_array_size);
	};
}