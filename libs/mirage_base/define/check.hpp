#ifndef MIRAGE_BASE_DEFINE_CHECK
#define MIRAGE_BASE_DEFINE_CHECK

#include <cassert>

#if defined(MIRAGE_BUILD_DEBUG)
#define MIRAGE_DCHECK(condition) assert(!!(condition))
#else
#define MIRAGE_DCHECK(condition) ((void)0)
#endif
#define MIRAGE_CHECK(condition) assert(!!(condition))

#define NOT_REACHABLE assert(false)

#endif  // MIRAGE_BASE_DEFINE_CHECK
