#pragma once

#include <VertexTypes.h>

#define SRIMA_D3D12

namespace srima
{
#if defined(SRIMA_D3D12)
	using srimaVertexPosition = DirectX::VertexPosition;
	using srimaVertexPositionColor = DirectX::VertexPositionColor;
	using srimaVertexPositionColorTexture = DirectX::VertexPositionColorTexture;
	using srimaVertexPositionDualTexture = DirectX::VertexPositionDualTexture;
	using srimaVertexPositionNormal = DirectX::VertexPositionNormal;
	using srimaVertexPositionNormalColor = DirectX::VertexPositionNormalColor;
	using srimaVertexPositionNormalColorTexture = DirectX::VertexPositionNormalColorTexture;
	using srimaVertexPositionNormalTexture = DirectX::VertexPositionNormalTexture;
	using srimaVertexPositionTexture = DirectX::VertexPositionTexture;
#elif defined(SRIMA_VULKAN)

#endif
}