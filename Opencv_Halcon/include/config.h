#if defined(_USRDLL)
#define OPENCV_HALCON_EXPORTS __declspec(dllexport)
#else
#define OPENCV_HALCON_EXPORTS __declspec(dllimport)
#endif