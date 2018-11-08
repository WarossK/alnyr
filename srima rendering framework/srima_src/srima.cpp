#include <../srima.hpp>

#if defined(SRIMA_D3D12)
#include "srima_src/srima_d3d12.cpp"
#elif defined(SRIMA_VULKAN) 
#include "srima_src/srima_vulkan.cpp"
#elif defined(SRIMA_D3D11)
#include "srima_src/srima_d3d11.cpp"
#elif defined(SRIMA_ULTIMATE_BETAGAKI)
#include "srima_src/srima_ultimate_betagaki.cpp"
#endif
