
#if defined(_USRDLL)
#define BLOB_EXPORTS __declspec(dllexport)
#else
#define BLOB_EXPORTS __declspec(dllimport)
#endif