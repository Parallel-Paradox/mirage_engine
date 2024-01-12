#ifndef MIRAGE_FRAMEWORK_DEFINE
#define MIRAGE_FRAMEWORK_DEFINE

#if defined(MIRAGE_BUILD_SHARED) && defined(_WIN32)

#if defined(MIRAGE_BUILD)
#define MIRAGE_API __declspec(dllexport)
#else
#define MIRAGE_API __declspec(dllimport)
#endif

#else
#define MIRAGE_API
#endif

#endif  // MIRAGE_FRAMEWORK_DEFINE
