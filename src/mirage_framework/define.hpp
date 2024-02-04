#ifndef MIRAGE_FRAMEWORK_DEFINE
#define MIRAGE_FRAMEWORK_DEFINE

#if defined(MIRAGE_BUILD_DEBUG)
#include <cassert>
#endif

// OS detector
#if defined(_WIN32)
#define OS_WIN
#elif defined(__APPLE__)
#define OS_APPLE
#endif

// dllexport on MSVC
#if defined(MIRAGE_BUILD_SHARED) && defined(OS_WIN)
#if defined(MIRAGE_BUILD)
#define MIRAGE_API __declspec(dllexport)
#else
#define MIRAGE_API __declspec(dllimport)
#endif
#else
#define MIRAGE_API
#endif

// Debug checker
#if defined(MIRAGE_BUILD_DEBUG)
#define MIRAGE_DCHECK(condition) assert(condition)
#else
#define MIRAGE_DCHECK(condition) ((void)0)
#endif

#endif  // MIRAGE_FRAMEWORK_DEFINE
