#include <Foundation/Platform/PlatformDesc.h>

ezPlatformDesc g_PlatformDescOSX("OSX", "Desktop");

#if EZ_ENABLED(EZ_PLATFORM_OSX)

const ezPlatformDesc* ezPlatformDesc::s_pThisPlatform = &g_PlatformDescOSX;

#endif
