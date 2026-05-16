#pragma once

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define DRAGO_PLATFORM_MACOS
        #define DRAGO_PLATFORM_NAME "macOS"
    #endif
#elif defined(_WIN32) || defined(_WIN64)
    #define DRAGO_PLATFORM_WINDOWS
    #define DRAGO_PLATFORM_NAME "Windows"
#elif defined(__linux__)
    #define DRAGO_PLATFORM_LINUX
    #define DRAGO_PLATFORM_NAME "Linux"
#else
    #error "Unsupported platform"
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define DRAGO_ARCH_ARM64
#elif defined(__x86_64__) || defined(_M_X64)
    #define DRAGO_ARCH_X64
#endif