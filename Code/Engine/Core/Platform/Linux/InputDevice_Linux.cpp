#include <Core/CorePCH.h>

#if EZ_ENABLED(EZ_PLATFORM_LINUX)

#  if EZ_DISABLED(EZ_SUPPORTS_GLFW)
#    include <Core/Platform/NoImpl/InputDevice_NoImpl.inl>
#  endif

#endif
