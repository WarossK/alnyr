#include "alnyr_static_mesh.h"
#include "srima.hpp"

bool alnyr::alnyrStaticMesh::Initialize(std::filesystem::path model_path)
{
	return false;
}

bool alnyr::alnyrStaticMesh::Initialize(uint32_t vertex_size, uint32_t vertex_array_size)
{
	vertex_buffer_view_ = srima::CreateVertexBuffer(vertices_.data(), vertex_size, vertex_array_size);

	return true;
}
