#ifndef MIRAGE_ECS_DEFINE
#define MIRAGE_ECS_DEFINE

#include "mirage_base/define.hpp"

#if defined(MIRAGE_BUILD_SHARED) && defined(MIRAGE_BUILD_MSVC)
#if defined(MIRAGE_BUILD_ECS)
#define MIRAGE_ECS __declspec(dllexport)
#else
#define MIRAGE_ECS __declspec(dllimport)
#endif
#else
#define MIRAGE_ECS
#endif

#endif  // MIRAGE_ECS_DEFINE
