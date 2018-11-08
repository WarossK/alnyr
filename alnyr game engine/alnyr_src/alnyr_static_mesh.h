#pragma once

#include <vector>
#include <filesystem>
#include <alnyr_src/alnyr_math.h>

namespace alnyr
{
	class alnyrStaticMesh
	{
	private:
		std::vector<Vector3> vertices_;

	public:
		alnyrStaticMesh() { vertices_.clear(); }
		bool Initialize(const std::vector<Vector3>& vertices);
		//bool Initialize(const std::vector<srimaVertex3D>& vertices);
		bool Initialize(std::filesystem::path model_path);
	};
}