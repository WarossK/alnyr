#pragma once

#include <filesystem>

namespace alnyr
{
	class alnyrTexture
	{
		bool LoadFromFile();
		bool MultiLoadFromDirectory(std::filesystem::path directory);
	};
}