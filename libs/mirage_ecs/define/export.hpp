#ifndef MIRAGE_ECS_DEFINE_EXPORT
#define MIRAGE_ECS_DEFINE_EXPORT

#if defined(MIRAGE_BUILD_SHARED) && defined(MIRAGE_BUILD_MSVC)
#if defined(MIRAGE_BUILD_ECS) || defined(MIRAGE_BUILD_ENGINE)
#define MIRAGE_ECS __declspec(dllexport)
#else
#define MIRAGE_ECS __declspec(dllimport)
#endif
#else
#define MIRAGE_ECS
#endif

#endif
