#pragma once

#if defined(USE_CMAKE_EXPORT_HEADER)
    #define __stdcall
    #define DllExport
#else
    #if _WIN32 || _WIN64
        #pragma comment(lib, "ws2_32.lib") // Winsock Library
        #define DllExport __declspec(dllexport)
    #else
        #define __stdcall
        #define DllExport __attribute__((visibility("default")))
    #endif
#endif