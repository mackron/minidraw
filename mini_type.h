/*
Typography library. Public domain. See "unlicense" statement at the end of this file.
mini_type - v0.x.x - 2018-xx-xx

David Reid - davidreidsoftware@gmail.com
*/

#ifndef MINI_TYPE_H
#define MINI_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4324)   /* structure was padded due to alignment specifier */
#endif

/* Platform/backend detection. */
#ifdef _WIN32
    #define MT_WIN32
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        #define MT_WIN32_UWP
    #else
        #define MT_WIN32_DESKTOP
    #endif
#else
    #define MT_POSIX
    #include <pthread.h>    /* Unfortunate #include, but needed for pthread_t, pthread_mutex_t and pthread_cond_t types. */

    #ifdef __unix__
        #define MT_UNIX
        #if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #define MT_BSD
        #endif
    #endif
    #ifdef __linux__
        #define MT_LINUX
    #endif
    #ifdef __APPLE__
        #define MT_APPLE
    #endif
    #ifdef __ANDROID__
        #define MT_ANDROID
    #endif
    #ifdef __EMSCRIPTEN__
        #define MT_EMSCRIPTEN
    #endif
#endif

#include <stddef.h> /* For size_t. */

#ifndef MT_HAS_STDINT
    #if defined(_MSC_VER)
        #if _MSC_VER >= 1600
            #define MT_HAS_STDINT
        #endif
    #else
        #if defined(__has_include)
            #if __has_include(<stdint.h>)
                #define MT_HAS_STDINT
            #endif
        #endif
    #endif
#endif

#if !defined(MT_HAS_STDINT) && (defined(__GNUC__) || defined(__clang__))   /* Assume support for stdint.h on GCC and Clang. */
    #define MT_HAS_STDINT
#endif

#ifndef MT_HAS_STDINT
typedef   signed char               mt_int8;
typedef unsigned char               mt_uint8;
typedef   signed short              mt_int16;
typedef unsigned short              mt_uint16;
typedef   signed int                mt_int32;
typedef unsigned int                mt_uint32;
    #if defined(_MSC_VER)
    typedef   signed __int64        mt_int64;
    typedef unsigned __int64        mt_uint64;
    #else
    typedef   signed long long int  mt_int64;
    typedef unsigned long long int  mt_uint64;
    #endif
    #if defined(_WIN32)
        #if defined(_WIN64)
        typedef mt_uint64           mt_uintptr;
        #else
        typedef mt_uint32           mt_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
        typedef mt_uint64           mt_uintptr;
        #else
        typedef mt_uint32           mt_uintptr;
        #endif
    #else
        typedef mt_uint64           mt_uintptr;    /* Fallback. */
    #endif
#else
#include <stdint.h>
typedef int8_t                      mt_int8;
typedef uint8_t                     mt_uint8;
typedef int16_t                     mt_int16;
typedef uint16_t                    mt_uint16;
typedef int32_t                     mt_int32;
typedef uint32_t                    mt_uint32;
typedef int64_t                     mt_int64;
typedef uint64_t                    mt_uint64;
typedef uintptr_t                   mt_uintptr;
#endif
typedef mt_uint8                    mt_bool8;
typedef mt_uint32                   mt_bool32;
#define MT_TRUE                     1
#define MT_FALSE                    0

typedef void* mt_handle;
typedef void* mt_ptr;
typedef void (* mt_proc)(void);

#if defined(_MSC_VER) && !defined(_WCHAR_T_DEFINED)
typedef mt_uint16 wchar_t;
#endif

typedef char                        mt_utf8;
typedef mt_uint16                   mt_utf16;
typedef mt_uint32                   mt_utf32;

/* Define NULL for some compilers. */
#ifndef NULL
#define NULL 0
#endif

#if defined(SIZE_MAX)
    #define MT_SIZE_MAX     SIZE_MAX
#else
    #define MT_SIZE_MAX     0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif


#ifdef _MSC_VER
#define MT_INLINE __forceinline
#else
#ifdef __GNUC__
#define MT_INLINE inline __attribute__((always_inline))
#else
#define MT_INLINE inline
#endif
#endif


/* Backend Support */
#if defined(MT_WIN32)
    #define MT_SUPPORT_GDI
    #define MT_SUPPORT_DIRECT2D
#endif
#if defined(MT_APPLE)
    #define MT_SUPPORT_COREGRAPHICS
#endif
#if defined(MT_UNIX)
    #define MT_SUPPORT_CAIRO
    #define MT_SUPPORT_XFT
#endif

/* Backends */
#if !defined(MT_NO_GDI) && defined(MT_SUPPORT_GDI)
    #define MT_ENABLE_GDI
#endif
#if !defined(MT_NO_DIRECT2D) && defined(MT_SUPPORT_DIRECT2D)
    #define MT_ENABLE_DIRECT2D
#endif
#if !defined(MT_NO_COREGRAPHICS) && defined(MT_SUPPORT_COREGRAPHICS)
    #define MT_ENABLE_COREGRAPHICS
#endif
#if !defined(MT_NO_CAIRO) && defined(MT_SUPPORT_CAIRO)
    #define MT_ENABLE_CAIRO
#endif
#if !defined(MT_NO_XFT) && defined(MT_SUPPORT_XFT)
    #define MT_ENABLE_XFT
#endif


/* Result Codes */
typedef int mt_result;
#define MT_SUCCESS                                       0
#define MT_ERROR                                        -1      /* A generic error. */
#define MT_INVALID_ARGS                                 -2
#define MT_INVALID_OPERATION                            -3
#define MT_OUT_OF_MEMORY                                -4
#define MT_FORMAT_NOT_SUPPORTED                         -5
#define MT_OUT_OF_RANGE                                 -6
#define MT_CANCELLED                                    -7
#define MT_INVALID_FILE                                 -8
#define MT_FAILED_TO_OPEN_FILE                          -9
#define MT_FAILED_TO_READ_FILE                          -10
#define MT_FAILED_TO_WRITE_FILE                         -11
#define MT_FILE_TOO_BIG                                 -12
#define MT_PATH_TOO_LONG                                -13
#define MT_NAME_TOO_LONG                                -14
#define MT_DOES_NOT_EXIST                               -15
#define MT_ALREADY_EXISTS                               -16
#define MT_ACCESS_DENIED                                -17
#define MT_TOO_MANY_OPEN_FILES                          -18
#define MT_END_OF_FILE                                  -19
#define MT_NO_SPACE                                     -20
#define MT_NEGATIVE_SEEK                                -21
#define MT_TIMEOUT                                      -22


/* Forward Declarations */
typedef struct mt_context mt_context;
typedef struct mt_gc mt_gc;

/* Enumerators */
typedef enum
{
    mt_backend_gdi,            /* Typography via Uniscribe */
    mt_backend_direct2d,       /* Typography via DirectWrite */
    mt_backend_coregraphics,   /* Typography via Core Text */
    mt_backend_cairo,          /* Typography via Pango */
    mt_backend_xft             /* Typography via HarfBuzz and mini_type */
} mt_backend;

/* Structures */
struct mt_context
{
    mt_backend graphicsBackend;

    union
    {
#if defined(MT_WIN32)
        struct
        {
            int _unused;
        } win32;
#endif
#if defined(MT_APPLE)
        struct
        {
            int _unused;
        } apple;
#endif
        int _unused;
    } platform;

    union
    {
#if defined(MT_SUPPORT_GDI)
        struct
        {
            int _unused;
        } gdi;
#endif
#if defined(MT_SUPPORT_DIRECT2D)
        struct
        {
            int _unused;
        } direct2d;
#endif
#if defined(MT_SUPPORT_COREGRAPHICS)
        struct
        {
            int _unused;
        } coregraphics;
#endif
#if defined(MT_SUPPORT_CAIRO)
        struct
        {
            int _unused;
        } cairo;
#endif
#if defined(MT_SUPPORT_XFT)
        struct
        {
            int _unused;
        } xft;
#endif
        int _unused;
    } backend;
};



#ifdef __cplusplus
}
#endif
#endif  /* MINI_TYPE_H */


/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 IMPLEMENTATION

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#if defined(MINI_TYPE_IMPLEMENTATION)

/* Standard library stuff. */
#ifndef MT_MALLOC
#ifdef MT_WIN32
#define MT_MALLOC(sz) HeapAlloc(GetProcessHeap(), 0, (sz))
#else
#define MT_MALLOC(sz) malloc((sz))
#endif
#endif

#ifndef MT_REALLOC
#ifdef MT_WIN32
#define MT_REALLOC(p, sz) (((sz) > 0) ? ((p) ? HeapReAlloc(GetProcessHeap(), 0, (p), (sz)) : HeapAlloc(GetProcessHeap(), 0, (sz))) : ((VOID*)(size_t)(HeapFree(GetProcessHeap(), 0, (p)) & 0)))
#else
#define MT_REALLOC(p, sz) realloc((p), (sz))
#endif
#endif

#ifndef MT_FREE
#ifdef MT_WIN32
#define MT_FREE(p) HeapFree(GetProcessHeap(), 0, (p))
#else
#define MT_FREE(p) free((p))
#endif
#endif

#ifndef MT_ZERO_MEMORY
#ifdef MT_WIN32
#define MT_ZERO_MEMORY(p, sz) ZeroMemory((p), (sz))
#else
#define MT_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif
#endif

#ifndef MT_COPY_MEMORY
#ifdef MT_WIN32
#define MT_COPY_MEMORY(dst, src, sz) CopyMemory((dst), (src), (sz))
#else
#define MT_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef MT_ASSERT
#ifdef MT_WIN32
#define MT_ASSERT(condition) assert(condition)
#else
#define MT_ASSERT(condition) assert(condition)
#endif
#endif

#define mt_zero_memory              MT_ZERO_MEMORY
#define mt_copy_memory              MT_COPY_MEMORY
#define mt_assert                   MT_ASSERT

#define mt_zero_object(p)           mt_zero_memory((p), sizeof(*(p)))
#define mt_countof(x)               (sizeof(x) / sizeof(x[0]))
#define mt_max(x, y)                (((x) > (y)) ? (x) : (y))
#define mt_min(x, y)                (((x) < (y)) ? (x) : (y))
#define mt_clamp(x, lo, hi)         (mt_max(lo, mt_min(x, hi)))
#define mt_offset_ptr(p, offset)    (((mt_uint8*)(p)) + (offset))



/* Although some backends may be enabled, they may not necessarily be usable (due to missing headers, compiler support, etc.). */
#if defined(MT_ENABLE_GDI) 
    #define MT_HAS_GDI          /* All relevant Windows platforms should support GDI + Uniscribe. */
#endif
#if defined(MT_ENABLE_DIRECT2D)
    #define MT_HAS_DIRECT2D     /* Headers are required for Direct2D + DirectWrite for now. TODO: Remove this dependency. */
    #if defined(__has_include)
        #if !__has_include(<d2d1.h>) || !__has_include(<dwrite.h>)
            #undef MT_HAS_DIRECT2D
        #endif
    #endif
#endif
#if defined(MT_ENABLE_COREGRAPHICS)
    #define MT_HAS_COREGRAPHICS /* All relevant Apple platforms should support Core Graphics and Core Text. */
#endif
#if defined(MT_ENABLE_CAIRO)
    #define MT_HAS_CAIRO        /* Headers are required for Cairo + Pango. */
    #if defined(__has_include)
        #if !__has_include(<pango/pangocairo.h>)
            #undef MT_HAS_CAIRO
        #endif
    #endif
#endif
#if defined(MT_ENABLE_XFT)
    #define MT_HAS_XFT          /* Some headers are required for XFT. */
    #if defined(__has_include)
        #if !__has_include(<X11/Xft/Xft.h>) || !__has_include(<harfbuzz/hb-ft.h>)
            #undef MT_HAS_XFT
        #endif
    #endif
#endif


#if defined(MT_WIN32)
#include <windows.h>

mt_result mt_result_from_win32_error(DWORD error)
{
    switch (error)
    {
        case ERROR_SUCCESS:             return MT_SUCCESS;
        case ERROR_PATH_NOT_FOUND:      return MT_DOES_NOT_EXIST;
        case ERROR_TOO_MANY_OPEN_FILES: return MT_TOO_MANY_OPEN_FILES;
        case ERROR_NOT_ENOUGH_MEMORY:   return MT_OUT_OF_MEMORY;
        case ERROR_DISK_FULL:           return MT_NO_SPACE;
        case ERROR_HANDLE_EOF:          return MT_END_OF_FILE;
        case ERROR_NEGATIVE_SEEK:       return MT_NEGATIVE_SEEK;
        case ERROR_INVALID_PARAMETER:   return MT_INVALID_ARGS;
        case ERROR_ACCESS_DENIED:       return MT_ACCESS_DENIED;
        case ERROR_SEM_TIMEOUT:         return MT_TIMEOUT;
        case ERROR_FILE_NOT_FOUND:      return MT_DOES_NOT_EXIST;
        default: break;
    }

    return MT_ERROR;
}

mt_result mt_result_from_HRESULT(HRESULT hr)
{
    switch (hr)
    {
        case S_OK:          return MT_SUCCESS;
        case E_INVALIDARG:  return MT_INVALID_ARGS;
        case E_OUTOFMEMORY: return MT_OUT_OF_MEMORY;
        default: break;
    }

    if (SUCCEEDED(hr)) {
        return MT_SUCCESS;
    } else {
        return MT_ERROR;
    }
}
#endif


/**************************************************************************************************************************************************************

 GDI + Uniscribe

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_GDI)
#include <usp10.h>

#endif


/**************************************************************************************************************************************************************

 Direct2D + DirectWrite

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_DIRECT2D)
#include <d2d1.h>
#include <dwrite.h>

#endif


/**************************************************************************************************************************************************************

 Core Graphics + Core Text

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_COREGRAPHICS)

#endif


/**************************************************************************************************************************************************************

 Cairo + Pango

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_CAIRO)
#include <pango/pangocairo.h>

#endif

/**************************************************************************************************************************************************************

 Xft + HarfBuzz

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_XFT)
#include <X11/Xft/Xft.h>
#include <harfbuzz/hb-ft.h> /* HarfBuzz. Should usually be <hb.h> judging by the documentation. Will need to investigate. */

#endif


#endif  /* MINI_TYPE_IMPLEMENTATION */