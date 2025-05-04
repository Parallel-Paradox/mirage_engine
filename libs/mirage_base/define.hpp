#ifndef MIRAGE_BASE_DEFINE
#define MIRAGE_BASE_DEFINE

#if defined(MIRAGE_BUILD_DEBUG)
#include <cassert>
#endif

#if defined(MIRAGE_BUILD_SHARED) && defined(MIRAGE_BUILD_MSVC)
#if defined(MIRAGE_BUILD_BASE) || defined(MIRAGE_BUILD_ENGINE)
#define MIRAGE_BASE __declspec(dllexport)
#else
#define MIRAGE_BASE __declspec(dllimport)
#endif
#else
#define MIRAGE_BASE
#endif

#if defined(MIRAGE_BUILD_DEBUG)
#define MIRAGE_DCHECK(condition) assert(!!(condition))
#else
#define MIRAGE_DCHECK(condition) ((void)0)
#endif
#define MIRAGE_CHECK(condition) assert(!!(condition))

#define NOT_REACHABLE assert(false)

#endif  // MIRAGE_BASE_DEFINE
