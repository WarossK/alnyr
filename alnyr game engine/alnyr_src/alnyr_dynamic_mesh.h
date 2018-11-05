#pragma once

#include <vector>
#include <filesystem>
#include <alnyr_math.h>

class alnyrDynamicMesh
{
private:
	std::vector<Vector3> vertices_;

public:
	bool Initialize(const std::vector<Vector3>& vertices);
	bool Initialize(std::filesystem::path model_path);
};