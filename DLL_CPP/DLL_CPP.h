#ifdef DLLCPP_EXPORTS
#define DLLCPP_API __declspec(dllexport)
#else
#define DLLCPP_API __declspec(dllimport)
#endif

extern "C" bool DLLCPP_API _cpp_calculate_histogram(unsigned __int32* histogram,
                                        unsigned __int8* pixel_buffer,
                                        unsigned __int32 number_of_pixels);
