#ifdef DLLCPP_EXPORTS
#define DLLCPP_API __declspec(dllexport)
#else
#define DLLCPP_API __declspec(dllimport)
#endif