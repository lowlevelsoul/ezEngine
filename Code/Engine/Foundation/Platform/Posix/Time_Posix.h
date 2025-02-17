#include <Foundation/FoundationInternal.h>
EZ_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Time/Time.h>

#include <time.h>

void ezTime::Initialize()
{
}

ezTime ezTime::Now()
{
  struct timespec sp;
  clock_gettime(CLOCK_MONOTONIC_RAW, &sp);

  return ezTime::MakeFromSeconds((double)sp.tv_sec + (double)(sp.tv_nsec / 1000000000.0));
}
