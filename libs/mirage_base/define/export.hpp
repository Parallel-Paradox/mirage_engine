#ifndef MIRAGE_BASE_DEFINE_EXPORT
#define MIRAGE_BASE_DEFINE_EXPORT

#if defined(MIRAGE_BUILD_SHARED) && defined(MIRAGE_BUILD_MSVC)
#if defined(MIRAGE_BUILD_BASE)
#define MIRAGE_BASE __declspec(dllexport)
#else
#define MIRAGE_BASE __declspec(dllimport)
#endif
#else
#define MIRAGE_BASE
#endif

#endif  // MIRAGE_BASE_DEFINE_EXPORT
