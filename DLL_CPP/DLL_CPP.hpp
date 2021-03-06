#ifdef DLLCPP_EXPORTS
#define DLLCPP_API __declspec(dllexport)
#else
#define DLLCPP_API __declspec(dllimport)
#endif

/**
 * Creates histogram (256 buckets) from 8-bit pixel values.
 * 
 * @param histogram Pointer to histogram memory block (256 * sizeof(unsigned __int32)
 * @param pixels Pointer to pixels buffer memory block
 * @param number_of_pixels Number of processed pixels
 * @return Number of pixels is valid
 */
extern "C" DLLCPP_API bool _cpp_calculate_histogram(unsigned __int32* histogram,
                                                    const unsigned __int8* pixels,
                                                    unsigned __int32 number_of_pixels);
