#pragma once

#define USE_DIRECTX_MATH

#if defined(USE_DIRECTX_MATH)//DirectXMath
#include <d3d12.h>
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

#elif defined(USE_OPENGL_MATHEMATICS)//GLM
#include <glm/glm.hpp>
using namespace glm;
using Vector2 = vec2;
using Vector3 = vec3;
using Vector4 = vec4;
using Matrix = mat4x4;

#endif