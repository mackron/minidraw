/*
Typography library. Choice of public domain or MIT-0. See license statements at the end of this file.
minitype - v0.x.x - 2018-xx-xx

David Reid - davidreidsoftware@gmail.com
*/

#ifndef MINITYPE_H
#define MINITYPE_H

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
#endif

#include <stddef.h> /* For size_t. */

/* Sized types. Prefer built-in types. Fall back to stdint. */
#ifdef _MSC_VER
    #if defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
    typedef   signed __int8  mt_int8;
    typedef unsigned __int8  mt_uint8;
    typedef   signed __int16 mt_int16;
    typedef unsigned __int16 mt_uint16;
    typedef   signed __int32 mt_int32;
    typedef unsigned __int32 mt_uint32;
    typedef   signed __int64 mt_int64;
    typedef unsigned __int64 mt_uint64;
    #if defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
#else
    #define MT_HAS_STDINT
    #include <stdint.h>
    typedef int8_t   mt_int8;
    typedef uint8_t  mt_uint8;
    typedef int16_t  mt_int16;
    typedef uint16_t mt_uint16;
    typedef int32_t  mt_int32;
    typedef uint32_t mt_uint32;
    typedef int64_t  mt_int64;
    typedef uint64_t mt_uint64;
#endif

#ifdef MT_HAS_STDINT
    typedef uintptr_t mt_uintptr;
#else
    #if defined(_WIN32)
        #if defined(_WIN64)
            typedef mt_uint64 mt_uintptr;
        #else
            typedef mt_uint32 mt_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
            typedef mt_uint64 mt_uintptr;
        #else
            typedef mt_uint32 mt_uintptr;
        #endif
    #else
        typedef mt_uint64 mt_uintptr;   /* Fallback. */
    #endif
#endif
typedef mt_uint8    mt_bool8;
typedef mt_uint32   mt_bool32;
#define MT_TRUE     1
#define MT_FALSE    0

typedef void* mt_handle;
typedef void* mt_ptr;
typedef void (* mt_proc)(void);

#if defined(_MSC_VER) && !defined(_WCHAR_T_DEFINED)
typedef mt_uint16 wchar_t;
#endif

typedef char        mt_utf8;
typedef mt_uint16   mt_utf16;
typedef mt_uint32   mt_utf32;

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
#if defined(MT_UNIX) || defined(MT_LINUX)
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
#define MT_SUCCESS                   0

/* General errors. */
#define MT_ERROR                    -1  /* A generic error. */
#define MT_INVALID_ARGS             -2
#define MT_INVALID_OPERATION        -3
#define MT_OUT_OF_MEMORY            -4
#define MT_OUT_OF_RANGE             -5
#define MT_TOO_BIG                  -6
#define MT_ACCESS_DENIED            -7
#define MT_DOES_NOT_EXIST           -8
#define MT_ALREADY_EXISTS           -9
#define MT_TOO_MANY_OPEN_FILES      -10
#define MT_INVALID_FILE             -11
#define MT_FAILED_TO_OPEN_FILE      -12
#define MT_FAILED_TO_READ_FILE      -13
#define MT_FAILED_TO_WRITE_FILE     -14
#define MT_END_OF_FILE              -15
#define MT_NO_SPACE                 -16
#define MT_NEGATIVE_SEEK            -17
#define MT_PATH_TOO_LONG            -18
#define MT_NAME_TOO_LONG            -19
#define MT_MEMORY_ALREADY_MAPPED    -20
#define MT_TIMEOUT                  -21
#define MT_CANCELLED                -22
#define MT_INVALID_BOM              -200
#define MT_INVALID_CODE_POINT       -201


/* Forward Declarations */
typedef struct mt_api mt_api;
typedef struct mt_api_config mt_api_config;
typedef struct mt_font mt_font;
typedef struct mt_font_config mt_font_config;
typedef struct mt_gc mt_gc;
typedef struct mt_gc_config mt_gc_config;
typedef struct mt_item mt_item;
typedef struct mt_script_state mt_script_state;

/* Enumerators */
typedef enum
{
    mt_backend_gdi,            /* Typography via Uniscribe */
    mt_backend_direct2d,       /* Typography via DirectWrite */
    mt_backend_coregraphics,   /* Typography via Core Text */
    mt_backend_cairo,          /* Typography via Pango */
    mt_backend_xft             /* Typography via HarfBuzz and minitype */
} mt_backend;

/* Structures */
struct mt_api_config
{
    mt_backend backend;
    struct
    {
        /*HDC*/ mt_handle hDC;  /* Optional pre-created global device context. */
    } gdi;
};

struct mt_api
{
    mt_backend backend;
    void      (* uninit)     (mt_api* pAPI);
    mt_result (* itemizeUTF8)(mt_api* pAPI, const mt_utf8* pText, size_t textLength, mt_item** ppItems, mt_uint32* pItemCount);
    void      (* freeItems)  (mt_api* pAPI, mt_item* pItems);
    mt_result (* fontInit)   (mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont);
    void      (* fontUninit) (mt_font* pFont);
    mt_result (* gcInit)     (mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC);
    void      (* gcUninit)   (mt_gc* pGC);

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

#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HDC*/ mt_handle hDC;  /* Optional pre-created global device context. */
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
};

struct mt_font_config
{
    const char* family;
    mt_uint32 sizeInPixels;
};

struct mt_font
{
    mt_api* pAPI;   /* The mt_api object that was used to initialize the font. */

#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HFONT*/ mt_handle hFont;
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
};

struct mt_gc_config
{
    int _unused;
};

struct mt_gc
{
    mt_api* pAPI;   /* The mt_api object that was used to initialize the graphics context. */

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
};

struct mt_item
{
    mt_uint8 bidiLevel;
    void* pBackendData; /* Internal use only. Backend-specific data. */
};


/**************************************************************************************************************************************************************

API

**************************************************************************************************************************************************************/
mt_result mt_init(const mt_api_config* pConfig, mt_api* pAPI);
void mt_uninit(mt_api* pAPI);
mt_result mt_itemize_utf8(mt_api* pAPI, const mt_utf8* pText, size_t textLength, mt_item** ppItems, mt_uint32* pItemCount);
void mt_free_items(mt_api* pAPI, mt_item* pItems);


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
mt_result mt_font_init(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont);
void mt_font_uninit(mt_font* pFont);


/**************************************************************************************************************************************************************

Graphics Context

**************************************************************************************************************************************************************/
mt_result mt_gc_init(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC);
void mt_gc_uninit(mt_gc* pGC);


/**************************************************************************************************************************************************************

Unicode

TODO: Implement optional invalid code point replacement.

**************************************************************************************************************************************************************/
#define MT_UNICODE_MIN_CODE_POINT                       0x000000
#define MT_UNICODE_MAX_CODE_POINT                       0x10FFFF
#define MT_UNICODE_REPLACEMENT_CHARACTER                0x00FFFD
#define MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF8    3
#define MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16   1
#define MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF32   1

#define MT_FORBID_BOM                                   (1 << 1)
#define MT_ERROR_ON_INVALID_CODE_POINT                  (1 << 2)

/*
Checks if the given UTF-16 byte order mark indicates little endian.
*/
MT_INLINE mt_bool32 mt_is_utf16_bom_le(const mt_uint8 bom[2])
{
    /* RFC 2781 - Section 3.2 */
    return bom[0] == 0xFF && bom[1] == 0xFE;
}

/*
Checks if the given UTF-16 byte order mark indicates big endian.
*/
MT_INLINE mt_bool32 mt_is_utf16_bom_be(const mt_uint8 bom[2])
{
    /* RFC 2781 - Section 3.2 */
    return bom[0] == 0xFE && bom[1] == 0xFF;
}

/*
Checks if the given UTF-32 byte order mark indicates little endian.
*/
MT_INLINE mt_bool32 mt_is_utf32_bom_le(const mt_uint8 bom[4])
{
    return bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00;
}

/*
Checks if the given UTF-32 byte order mark indicates big endian.
*/
MT_INLINE mt_bool32 mt_is_utf32_bom_be(const mt_uint8 bom[4])
{
    return bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF;
}

/*
Checks whether or not the given bytes start with a UTF-8 byte order mark.
*/
MT_INLINE mt_bool32 mt_has_utf8_bom(const mt_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MT_FALSE;
    }

    if (len < 3) {
        return MT_FALSE;
    }

    return (pBytes[0] == 0xEF && pBytes[1] == 0xBB && pBytes[2] == 0xBF);
}

/*
Checks whether or not the given bytes start with a UTF-16 byte order mark.
*/
MT_INLINE mt_bool32 mt_has_utf16_bom(const mt_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MT_FALSE;
    }

    if (len < 2) {
        return MT_FALSE;
    }

    return mt_is_utf16_bom_le(pBytes) || mt_is_utf16_bom_be(pBytes);
}

/*
Checks whether or not the given bytes start with a UTF-16 byte order mark.
*/
MT_INLINE mt_bool32 mt_has_utf32_bom(const mt_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MT_FALSE;
    }

    if (len < 4) {
        return MT_FALSE;
    }

    return mt_is_utf32_bom_le(pBytes) || mt_is_utf32_bom_be(pBytes);
}

/*
Swaps the endianness of the given UTF-16 string.
*/
void mt_swap_endian_utf16(mt_utf16* pUTF16, size_t count);

/*
Swaps the endianness of the given UTF-32 string.
*/
void mt_swap_endian_utf32(mt_utf32* pUTF32, size_t count);


/*
Calculates the number of UTF-16's required to fully contain the given UTF-8 string after conversion, not including the null temrinator.

Remarks
-------
This is the same as mt_utf8_to_utf16ne/le/be(), except it doesn't actually do any conversion.
*/
mt_result mt_utf8_to_utf16_length(size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);

/*
Converts a UTF-8 string to a UTF-16 string.

Parameters
----------
pUTF16:    The buffer that receives the converted string data. This can be NULL in which case only the length is calculated.

utf16Cap:  The capacity of the buffer pointed to by pTextUTF16

pUTF16Len: A pointer to the variable that will receive the length of the output string not including the null terminator. This will be set to 0 when
           an error occurs, however you should not use this for validation. Instead you should look at the return value.

pUTF8:     The UTF-8 string to convert. It is an error for this to be null.

utf8Len:   The length of the UTF-8 string in bytes, not including the null terminator.

flags:     This can be a combination of the following:
           MT_FORBID_BOM: Causes an error if the string starts with a BOM.


Return Value
------------
MT_SUCCESS if successful; an error result otherwise. Converting an empty string is valid.

MT_INVALID_BOM will be returned if the string starts with a byte order mark and MT_FORBID_BOM is set in [flags].

MT_INVALID_ARGS will be returned if [pUTF8] is null.

MT_INVALID_ARGS will be returned if [pUTF8] is not big enough to fully convert the last code point.

MT_OUT_OF_MEMORY will be returned if the [pUTF8] buffer is too small.

MT_INVALID_CODE_POINT will be returned if an invalid code point is encounted and MT_ERROR_ON_INVALID_CODE_POINT is set in [flags].


Remarks
-------
The output string will always be null terminated.

When the output string ([pUTF16]) is null, this is equivalent to `mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags)`.

The byte order mark will _not_ be included in the output string nor it's length.

Invalid characters are replaced with the Unicode replacement character (MT_UNICODE_REPLACEMENT_CHARACTER/U+FFFD).
*/
mt_result mt_utf8_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);
mt_result mt_utf8_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);
mt_result mt_utf8_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);


/*
Calculates the number of UTF-32's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf8_to_utf32ne/le/be(), except it doesn't actually do any conversion.
*/
mt_result mt_utf8_to_utf32_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);

/*
Converts a UTF-8 string to a UTF-32 string.
*/
mt_result mt_utf8_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);
mt_result mt_utf8_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);
mt_result mt_utf8_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);


/*
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf16/be/le_to_utf8(), except it doesn't actually do any conversion.
*/
mt_result mt_utf16ne_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16le_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16be_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string.
*/
mt_result mt_utf16ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);





#ifdef __cplusplus
}
#endif
#endif  /* MINITYPE_H */


/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 IMPLEMENTATION

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#if defined(MINITYPE_IMPLEMENTATION)

/* CPU Architecture */
#if defined(__x86_64__) || defined(_M_X64)
    #define MT_X64
#elif defined(__i386) || defined(_M_IX86)
    #define MT_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define MT_ARM
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1300
    #define MT_HAS_BYTESWAP16_INTRINSIC
    #define MT_HAS_BYTESWAP32_INTRINSIC
    #define MT_HAS_BYTESWAP64_INTRINSIC
#elif defined(__clang__)
    #if __has_builtin(__builtin_bswap16)
        #define MT_HAS_BYTESWAP16_INTRINSIC
    #endif
    #if __has_builtin(__builtin_bswap32)
        #define MT_HAS_BYTESWAP32_INTRINSIC
    #endif
    #if __has_builtin(__builtin_bswap64)
        #define MT_HAS_BYTESWAP64_INTRINSIC
    #endif
#elif defined(__GNUC__)
    #if ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
        #define MT_HAS_BYTESWAP32_INTRINSIC
        #define MT_HAS_BYTESWAP64_INTRINSIC
    #endif
    #if ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        #define MT_HAS_BYTESWAP16_INTRINSIC
    #endif
#endif


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

#ifndef MT_MOVE_MEMORY
#ifdef MT_WIN32
#define MT_MOVE_MEMORY(dst, src, sz) MoveMemory((dst), (src), (sz))
#else
#define MT_MOVE_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef MT_ASSERT
#include <assert.h>
#ifdef MT_WIN32
#define MT_ASSERT(condition) assert(condition)
#else
#define MT_ASSERT(condition) assert(condition)
#endif
#endif

#define MT_ZERO_OBJECT(p)           MT_ZERO_MEMORY((p), sizeof(*(p)))
#define MT_COUNTOF(x)               (sizeof(x) / sizeof((x)[0]))
#define MT_MAX(x, y)                (((x) > (y)) ? (x) : (y))
#define MT_MIN(x, y)                (((x) < (y)) ? (x) : (y))
#define MT_CLAMP(x, lo, hi)         (MT_MAX((lo), MT_MIN((x), (hi))))
#define MT_OFFSET_PTR(p, offset)    (((mt_uint8*)(p)) + (offset))



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


static MT_INLINE mt_bool32 mt_is_little_endian()
{
#if defined(MT_X86) || defined(MT_X64)
    return MT_TRUE;
#else
    int n = 1;
    return (*(char*)&n) == 1;
#endif
}

static MT_INLINE mt_bool32 mt_is_big_endian()
{
    return !mt_is_little_endian();
}

static MT_INLINE mt_uint16 mt_swap_endian_uint16(mt_uint16 n)
{
#ifdef MT_HAS_BYTESWAP16_INTRINSIC
    #if defined(_MSC_VER)
        return _byteswap_ushort(n);
    #elif defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap16(n);
    #else
        #error "This compiler does not support the byte swap intrinsic."
    #endif
#else
    return ((n & 0xFF00) >> 8) |
           ((n & 0x00FF) << 8);
#endif
}

static MT_INLINE mt_uint32 mt_swap_endian_uint32(mt_uint32 n)
{
#ifdef MT_HAS_BYTESWAP32_INTRINSIC
    #if defined(_MSC_VER)
        return _byteswap_ulong(n);
    #elif defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap32(n);
    #else
        #error "This compiler does not support the byte swap intrinsic."
    #endif
#else
    return ((n & 0xFF000000) >> 24) |
           ((n & 0x00FF0000) >>  8) |
           ((n & 0x0000FF00) <<  8) |
           ((n & 0x000000FF) << 24);
#endif
}


static MT_INLINE mt_uint16 mt_be2host_16(mt_uint16 n)
{
#ifdef __linux__
    return be16toh(n);
#else
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
#endif
}

static MT_INLINE mt_uint32 mt_be2host_32(mt_uint32 n)
{
#ifdef __linux__
    return be32toh(n);
#else
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
#endif
}

static MT_INLINE mt_uint16 mt_le2host_16(mt_uint16 n)
{
#ifdef __linux__
    return le16toh(n);
#else
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
#endif
}

static MT_INLINE mt_uint32 mt_le2host_32(mt_uint32 n)
{
#ifdef __linux__
    return le32toh(n);
#else
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
#endif
}



/**************************************************************************************************************************************************************

 GDI + Uniscribe

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_GDI)
#include <usp10.h>

/* Font */
mt_result mt_font_init__gdi(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont)
{
    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pFont != NULL);

    (void)pAPI;
    (void)pConfig;
    (void)pFont;

    return MT_SUCCESS;
}

void mt_font_uninit__gdi(mt_font* pFont)
{
    MT_ASSERT(pFont != NULL);

    (void)pFont;
}


/* Graphics */
mt_result mt_gc_init__gdi(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC)
{
    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pGC != NULL);

    (void)pAPI;
    (void)pConfig;
    (void)pGC;

    return MT_SUCCESS;
}

void mt_gc_uninit__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    (void)pGC;
}


/* API */
mt_result mt_itemize_utf16__gdi(mt_api* pAPI, const mt_utf16* pText, size_t textLength, mt_item** ppItems, mt_uint32* pItemCount)
{
    HRESULT hResult;
    mt_item* pItems = NULL;
    int itemCapacity;
    int itemCount = 0;

    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pText != NULL);
    MT_ASSERT(pText[0] != '\0');
    MT_ASSERT(textLength > 0);
    MT_ASSERT(ppItems != NULL);
    MT_ASSERT(pItemCount != NULL);

    /* ScriptItemize() uses an integer for the text length, so keep it simple and return an error if we're asking for more. */
    if (textLength > 0xEFFFFFFF) {
        return MT_INVALID_ARGS;
    }

    /*
    Unfortunately ScriptItemize() is kind of bad when it comes to counting the number of items in the string. From the documentation:
    
        If the function returns E_OUTOFMEMORY, the application can call it again with a larger pItems buffer.

    This means that the way to do this is to repeatedly call ScriptItemize() with ever increasing buffer sizes.
    */

    /* TODO: This needs optimizing. Try reusing caching buffers rather than calling malloc/realloc each time. */
    itemCapacity = 16;
    for (;;) {
        size_t bufferSizeInBytes;
        mt_item* pNewItems;

        /*
        The buffer is made up of two parts. The first part is the list of mt_item objects. The second part is the array of Uniscribe
        SCRIPT_ITEM objects. Everything is in the same allocation.
        */
        bufferSizeInBytes = itemCapacity * (sizeof(mt_item) + sizeof(SCRIPT_ITEM));
        MT_ASSERT(bufferSizeInBytes > 0);

        pNewItems = (mt_item*)MT_REALLOC(pItems, bufferSizeInBytes);
        if (pNewItems == NULL) {
            MT_FREE(pItems);
            return MT_OUT_OF_MEMORY;
        }

        pItems = pNewItems;

        /* Subtract 1 from item capacity because the last item is always used as a null terminator. */

        /* Temporarily commented out to avoid a link error with ScriptItemize() while testing stuff. */
#if 0
        hResult = ScriptItemize((const wchar_t*)pText, (int)textLength, itemCapacity-1, NULL, NULL, (SCRIPT_ITEM*)MT_OFFSET_PTR(pItems, itemCapacity*sizeof(mt_item)), &itemCount);
        if (hResult == E_OUTOFMEMORY) {
            /* Not enough room. Increase the capacity and try again. */
            itemCapacity *= 2;
            continue;
        }
#else
        hResult = S_OK;
#endif

        /*
        Getting here means we were able to itemize the string. Something we need to do here is move the SCRIPT_ITEMs so that they're
        sitting immediately after the mt_items. The reason for this is to make it easy to find where the first SCRIPT_ITEM is located
        in the buffer by looking at the item count instead of the capacity. This is beneficial because the item count is always readily
        available with the item array, whereas the capacity is not.
        */
        MT_MOVE_MEMORY(MT_OFFSET_PTR(pItems, itemCount*sizeof(mt_item)), MT_OFFSET_PTR(pItems, itemCapacity*sizeof(mt_item)), itemCount*sizeof(SCRIPT_ITEM));

        /* We need to duplicate some data from the platform-specific SCRIPT_ITEMs over to the cross-platform mt_items. */

    }
    
    *pItemCount = (mt_uint32)itemCount;
    return MT_SUCCESS;
}

mt_result mt_itemize_utf8__gdi(mt_api* pAPI, const mt_utf8* pTextUTF8, size_t textLength, mt_item** ppItems, mt_uint32* pItemCount)
{
    HRESULT hResult;
    mt_result result;
    size_t utf8Size;

    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pTextUTF8 != NULL);
    MT_ASSERT(pTextUTF8[0] != '\0');
    MT_ASSERT(textLength > 0);
    MT_ASSERT(ppItems != NULL);
    MT_ASSERT(pItemCount != NULL);

    /* Unfortunately Uniscribe only supports wchar_t. This is 2 bytes on Windows so I'm treating this as UTF-16. We need to convert. */
    result = mt_utf8_to_utf16ne(NULL, 0, &utf8Size, pTextUTF8, textLength, 0);
    if (result != MT_SUCCESS) {
        return result;  /* Some error has occurred with UTF-8 to UTF-16 conversion. */
    }

    

    
    (void)hResult;

    return MT_SUCCESS;
}

void mt_free_items__gdi(mt_api* pAPI, mt_item* pItems)
{
    /* TODO: Use some caching or something to make item memory management more efficient. */

    MT_FREE(pItems);

    (void)pAPI;
}

void mt_uninit__gdi(mt_api* pAPI)
{
    MT_ASSERT(pAPI != NULL);

    (void)pAPI;
}

mt_result mt_init__gdi(const mt_api_config* pConfig, mt_api* pAPI)
{
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pAPI != NULL);

    pAPI->uninit      = mt_uninit__gdi;
    pAPI->itemizeUTF8 = mt_itemize_utf8__gdi;
    pAPI->freeItems   = mt_free_items__gdi;
    pAPI->fontInit    = mt_font_init__gdi;
    pAPI->fontUninit  = mt_font_uninit__gdi;
    pAPI->gcInit      = mt_gc_init__gdi;
    pAPI->gcUninit    = mt_gc_uninit__gdi;

    (void)pConfig;
    return MT_SUCCESS;
}
#endif


/**************************************************************************************************************************************************************

 Direct2D + DirectWrite

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_DIRECT2D)
/* These are C++ only it seems...
#include <d2d1.h>
#include <dwrite.h>
*/

/* Font */

/* Graphics */

/* API */

#endif


/**************************************************************************************************************************************************************

 Core Graphics + Core Text

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_COREGRAPHICS)

/* Font */

/* Graphics */

/* API */

#endif


/**************************************************************************************************************************************************************

 Cairo + Pango

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_CAIRO)
#include <pango/pangocairo.h>

/* Font */

/* Graphics */

/* API */

#endif

/**************************************************************************************************************************************************************

 Xft + HarfBuzz

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_XFT)
#include <X11/Xft/Xft.h>
#include <harfbuzz/hb-ft.h> /* HarfBuzz. Should usually be <hb.h> judging by the documentation. Will need to investigate. */

/* Font */

/* Graphics */

/* API */

#endif


/**************************************************************************************************************************************************************

API

**************************************************************************************************************************************************************/
mt_result mt_init(const mt_api_config* pConfig, mt_api* pAPI)
{
    mt_result result = MT_ERROR;

    if (pAPI == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pAPI);

    if (pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    switch (pConfig->backend)
    {
#if defined(MT_HAS_GDI)
        case mt_backend_gdi:
        {
            result = mt_init__gdi(pConfig, pAPI);
        } break;
#endif

        default:
        {
            result = MT_INVALID_ARGS;
        };
    }

    if (result != MT_SUCCESS) {
        return result;
    }

    return MT_SUCCESS;
}

void mt_uninit(mt_api* pAPI)
{
    if (pAPI == NULL) {
        return;
    }

    pAPI->uninit(pAPI);
}

mt_result mt_itemize_utf8(mt_api* pAPI, const mt_utf8* pText, size_t textLength, mt_item** ppItems, mt_uint32* pItemCount)
{
    mt_result result;
    mt_uint32 itemCount;

    if (pItemCount != NULL) {
        *pItemCount = 0;    /* Safety. */
    }

    if (pAPI == NULL || pText == NULL || ppItems == NULL) {
        return MT_INVALID_ARGS;
    }

    /*
    We could do some generic itemization here such as line breaks and tabs, however it makes things awkward with memory management. I'm therefore
    leaving this to each of the backends.
    */
    result = pAPI->itemizeUTF8(pAPI, pText, textLength, ppItems, &itemCount);

    if (pItemCount != NULL) {
        *pItemCount = itemCount;
    }

    return result;
}

void mt_free_items(mt_api* pAPI, mt_item* pItems)
{
    if (pAPI == NULL || pItems == NULL) {
        return;
    }

    pAPI->freeItems(pAPI, pItems);
}


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
mt_result mt_font_init(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont)
{
    mt_result result;

    if (pFont != NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pFont);

    if (pAPI == NULL || pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    pFont->pAPI = pAPI;

    result = pAPI->fontInit(pAPI, pConfig, pFont);
    if (result != MT_SUCCESS) {
        return result;
    }

    return MT_SUCCESS;
}

void mt_font_uninit(mt_font* pFont)
{
    if (pFont == NULL) {
        return;
    }

    pFont->pAPI->fontUninit(pFont);
}


/**************************************************************************************************************************************************************

Graphics Context

**************************************************************************************************************************************************************/
mt_result mt_gc_init(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC)
{
    mt_result result;

    if (pGC != NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pGC);

    if (pAPI == NULL || pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    pGC->pAPI = pAPI;

    result = pAPI->gcInit(pAPI, pConfig, pGC);
    if (result != MT_SUCCESS) {
        return result;
    }

    return MT_SUCCESS;
}

void mt_gc_uninit(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    pGC->pAPI->gcUninit(pGC);
}



/**************************************************************************************************************************************************************

Utility APIs

**************************************************************************************************************************************************************/
MT_INLINE mt_bool32 mt_is_invalid_utf8_octet(mt_utf8 utf8)
{
    /* RFC 3629 - Section 1: The octet values C0, C1, F5 to FF never appear. */
    return utf8 == 0xC0 || utf8 == 0xC1 || utf8 == 0xF5 || utf8 == 0xFF;
}

MT_INLINE void mt_utf32_cp_to_utf16_pair(mt_utf32 utf32cp, mt_utf16* pUTF16)
{
    /* RFC 2781 - Section 2.1 */
    mt_utf32 u;

    MT_ASSERT(utf32cp >= 0x10000);

    u = utf32cp - 0x10000;
    pUTF16[0] = (mt_utf16)(0xD800 | ((u & 0xFFC00) >> 10));
    pUTF16[1] = (mt_utf16)(0xDC00 | ((u & 0x003FF) >>  0));
}

MT_INLINE mt_utf32 mt_utf16_pair_to_utf32_cp(const mt_utf16* pUTF16)
{
    /* RFC 2781 - Section 2.1 */
    MT_ASSERT(pUTF16 != NULL);

    return (((mt_utf32)(pUTF16[0] & 0x003FF) << 10) | ((mt_utf32)(pUTF16[1] & 0x003FF) << 0)) + 0x10000;
}

MT_INLINE mt_bool32 mt_is_cp_in_surrogate_pair_range(mt_utf32 utf32)
{
    return utf32 >= 0xD800 && utf32 <= 0xDFFF;
}

MT_INLINE mt_bool32 mt_is_valid_code_point(mt_utf32 utf32)
{
    return utf32 <= MT_UNICODE_MAX_CODE_POINT && !mt_is_cp_in_surrogate_pair_range(utf32);
}

MT_INLINE size_t mt_utf32_cp_to_utf8_length(mt_utf32 utf32)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MT_ASSERT(utf32 <= MT_UNICODE_MAX_CODE_POINT);
    MT_ASSERT(mt_is_cp_in_surrogate_pair_range(utf32) == MT_FALSE);

    if (utf32 <= 0x7F) {
        return 1;
    }
    if (utf32 <= 0x7FF) {
        return 2;
    }
    if (utf32 <= 0xFFFF) {
        return 3;
    }
    if (utf32 <= 0x10FFFF) {
        return 4;
    }

    /* Invalid. This function assume's the UTF-32 code point is valid so do an assert. */
    MT_ASSERT(MT_FALSE);
    return 0; /* Invalid. */
}

MT_INLINE size_t mt_utf32_cp_to_utf8(mt_utf32 utf32, mt_utf8* pUTF8, size_t utf8Cap)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MT_ASSERT(utf32 <= MT_UNICODE_MAX_CODE_POINT);
    MT_ASSERT(mt_is_cp_in_surrogate_pair_range(utf32) == MT_FALSE);
    MT_ASSERT(utf8Cap > 1);

    if (utf32 <= 0x7F) {
        if (utf8Cap >= 1) {
            pUTF8[0] = (utf32 & 0x7F);
            return 1;
        }
    }
    if (utf32 <= 0x7FF) {
        if (utf8Cap >= 2) {
            pUTF8[0] = ((utf32 & 0x07C0) >> 6);
            pUTF8[1] =  (utf32 & 0x003F);
            return 2;
        }
    }
    if (utf32 <= 0xFFFF) {
        if (utf8Cap >= 3) {
            pUTF8[0] = ((utf32 & 0xF000) >> 12);
            pUTF8[1] = ((utf32 & 0x0FC0) >>  6);
            pUTF8[2] =  (utf32 & 0x003F);
            return 3;
        }
    }
    if (utf32 <= 0x10FFFF) {
        if (utf8Cap >= 4) {
            pUTF8[0] = ((utf32 & 0x1C0000) >> 18);
            pUTF8[1] = ((utf32 & 0x03F000) >> 12);
            pUTF8[2] = ((utf32 & 0x000FC0) >>  6);
            pUTF8[3] =  (utf32 & 0x00003F);
            return 4;
        }
    }

    /* Getting here means there was not enough room in the output buffer. */
    return 0;
}


void mt_swap_endian_utf16(mt_utf16* pUTF16, size_t count)
{
    if (count == (size_t)-1) {
        size_t i;
        for (i = 0; i < count; ++i) {
            pUTF16[i] = mt_swap_endian_uint16(pUTF16[i]);
        }
    } else {
        while (pUTF16[0] != 0) {
            pUTF16[0] = mt_swap_endian_uint16(pUTF16[0]);
            pUTF16 += 1;
        }
    }
}

void mt_swap_endian_utf32(mt_utf32* pUTF32, size_t count)
{
    if (count == (size_t)-1) {
        size_t i;
        for (i = 0; i < count; ++i) {
            pUTF32[i] = mt_swap_endian_uint32(pUTF32[i]);
        }
    } else {
        while (pUTF32[0] != 0) {
            pUTF32[0] = mt_swap_endian_uint32(pUTF32[0]);
            pUTF32 += 1;
        }
    }
}


mt_result mt_utf8_to_utf16_length(size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    size_t utf16Len = 0;

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;   /* Invalid input. */
    }

    if (utf8Len == 0 || pUTF8[0] == 0) {
        return MT_SUCCESS;   /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (mt_has_utf8_bom((const mt_uint8*)pUTF8, utf8Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        for (;;) {
            if (pUTF8[0] == 0) {
                break;  /* Reached the end of the null terminated string. */
            }

            if (pUTF8[0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                pUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16. */
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        pUTF8    += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;
                            } else {
                                /* Replacement. */
                                utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                pUTF8    += 4;
                            }
                        } else {
                            utf16Len += 2;  /* Must be at least 2 UTF-16s */
                            pUTF8    += 4;
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            pUTF8    += 1;
                        }
                    }
                }
            }
        }
    } else {
        /* Fixed length string. */
        for (size_t iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                iUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;
                            } else {
                                /* Replacement. */
                                utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                iUTF8    += 4;
                            }
                        } else {
                            utf16Len += 2;  /* Must be at least 2 UTF-16s */
                            iUTF8    += 4;
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            iUTF8    += 1;
                        }
                    }
                }
            }
        }
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    size_t utf16CapOriginal = utf16Cap;

    if (pUTF16 == NULL) {
        return mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags);
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf8_bom((const mt_uint8*)pUTF8, utf8Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF8[0] != 0) {
            if (pUTF8[0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[0];
                pUTF16   += 1;
                utf16Cap -= 1;
                pUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[1] & 0x3F) << 6) | (pUTF8[2] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        pUTF8    += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (utf16Cap < 2) {
                            break;  /* No enough room. */
                        } else {
                            if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                                return MT_INVALID_ARGS; /* Input string is too short. */
                            }

                            mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                            if (!mt_is_valid_code_point(cp)) {
                                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    return MT_INVALID_CODE_POINT;
                                } else {
                                    /* Replacement. */
                                    pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                                    pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    utf16Cap += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    pUTF8    += 4;
                                }
                            } else {
                                mt_utf32_cp_to_utf16_pair(cp, pUTF16);
                                pUTF16   += 2;
                                utf16Cap -= 2;
                                pUTF8    += 4;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            utf16Cap += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            pUTF8    += 1;
                        }
                    }
                }
            }
        }
    } else {
        /* Fixed length string. */
        for (size_t iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[iUTF8+0];
                pUTF16   += 1;
                utf16Cap -= 1;
                iUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (utf16Cap < 2) {
                            break;  /* No enough room. */
                        } else {
                            if (iUTF8+3 >= utf8Len) {
                                return MT_INVALID_ARGS;
                            }

                            mt_uint32 cp = ((mt_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((mt_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                            if (!mt_is_valid_code_point(cp)) {
                                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    return MT_INVALID_CODE_POINT;
                                } else {
                                    /* Replacement. */
                                    pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                                    pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    iUTF8    += 4;
                                }
                            } else {
                                mt_utf32_cp_to_utf16_pair(cp, pUTF16);
                                pUTF16   += 2;
                                utf16Cap -= 2;
                                iUTF8    += 4;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            iUTF8    += 1;
                        }
                    }
                }
            }
        }
    }

    /* Null terminate. */
    if (utf16Cap == 0) {
        return MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    }

    pUTF16[0] = 0;

    if (pUTF16Len != NULL) {
        *pUTF16Len = (utf16CapOriginal - utf16Cap);
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    mt_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, flags);

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    if (result != MT_SUCCESS) {
        return result;
    }

    if (pUTF16 != NULL && !mt_is_little_endian()) {
        mt_swap_endian_utf16(pUTF16, utf16Len);
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    mt_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, flags);

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    if (result != MT_SUCCESS) {
        return result;
    }

    if (pUTF16 != NULL && !mt_is_big_endian()) {
        mt_swap_endian_utf16(pUTF16, utf16Len);
    }

    return MT_SUCCESS;
}



mt_result mt_utf8_to_utf32_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    size_t utf32Len = 0;

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;   /* Invalid input. */
    }

    if (utf8Len == 0 || pUTF8[0] == 0) {
        return MT_SUCCESS;   /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (mt_has_utf8_bom((const mt_uint8*)pUTF8, utf8Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        for (;;) {
            if (pUTF8[0] == 0) {
                break;  /* Reached the end of the null terminated string. */
            }

            utf32Len += 1;
            if (pUTF8[0] < 128) {   /* ASCII character. */
                pUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }
                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;
                            } else {
                                /* Replacement. */
                            }
                        }
                        pUTF8 += 4;
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            pUTF8 += 1;
                        }
                    }
                }
            }
        }
    } else {
        /* Fixed length string. */
        for (size_t iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            utf32Len += 1;
            if (pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                iUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }
                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;
                            } else {
                                /* Replacement. */
                            }
                        }

                        iUTF8 += 4;
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            iUTF8 += 1;
                        }
                    }
                }
            }
        }
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    size_t utf32CapOriginal = utf32Cap;

    if (pUTF32 == NULL) {
        return mt_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf8_bom((const mt_uint8*)pUTF8, utf8Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF8[0] != 0) {
            if (pUTF8[0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[0];
                pUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[1] & 0x3F) << 6) | (pUTF8[2] & 0x3F);
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            return MT_INVALID_ARGS; /* Input string is too short. */
                        }

                        pUTF32[0] = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        pUTF8 += 4;

                        if (!mt_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;   /* No characters should be in the UTF-16 surrogate pair range. */
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF8 += 1;
                        }
                    }
                }

                pUTF32   += 1;
                utf32Cap -= 1;
            }
        }
    } else {
        /* Fixed length string. */
        for (size_t iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[iUTF8+0];
                iUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        pUTF32[0] = ((mt_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((mt_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                        iUTF8 += 4;

                        if (!mt_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                return MT_INVALID_CODE_POINT;
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            iUTF8 += 1;
                        }
                    }
                }

                pUTF32   += 1;
                utf32Cap -= 1;
            }
        }
    }

    /* Null terminate. */
    if (utf32Cap == 0) {
        return MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    }

    pUTF32[0] = 0;

    if (pUTF32Len != NULL) {
        *pUTF32Len = (utf32CapOriginal - utf32Cap);
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    mt_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, flags);

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    if (result != MT_SUCCESS) {
        return result;
    }

    if (pUTF32 != NULL && !mt_is_little_endian()) {
        mt_swap_endian_utf32(pUTF32, utf32Len);
    }

    return MT_SUCCESS;
}

mt_result mt_utf8_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags)
{
    mt_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, flags);

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    if (result != MT_SUCCESS) {
        return result;
    }

    if (pUTF32 != NULL && !mt_is_big_endian()) {
        mt_swap_endian_utf32(pUTF32, utf32Len);
    }

    return MT_SUCCESS;
}



mt_result mt_utf16_to_utf8_length_internal(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags, mt_bool32 isLE)
{
    size_t utf8Len = 0;
    mt_utf16 w1;
    mt_utf16 w2;
    mt_utf32 utf32;

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF16 == NULL) {
        return MT_INVALID_ARGS;   /* Invalid input. */
    }

    if (utf16Len == 0 || pUTF16[0] == 0) {
        return MT_SUCCESS;   /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (mt_has_utf16_bom((const mt_uint8*)pUTF16, utf16Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF16 += 1; /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }
    }

    if (utf16Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF16[0] != 0) {
            if (isLE) {
                w1 = mt_le2host_16(pUTF16[0]);
            } else {
                w1 = mt_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        return MT_INVALID_ARGS; /* Ran out of input data. */
                    }

                    if (isLE) {
                        w2 = mt_le2host_16(pUTF16[1]);
                    } else {
                        w2 = mt_be2host_16(pUTF16[1]);
                    }
                    
                    if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                        utf32 = mt_utf16_pair_to_utf32_cp(pUTF16);
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                        }
                    }

                    pUTF16 += 2;
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            utf8Len += mt_utf32_cp_to_utf8_length(utf32);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF16;
        for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
            if (isLE) {
                w1 = mt_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = mt_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 >= utf16Len) {
                        return MT_INVALID_ARGS; /* Ran out of input data. */
                    }

                    if (isLE) {
                        w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                    } else {
                        w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                    }
                    
                    if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                        utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                        }
                    }

                    iUTF16 += 2;
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            utf8Len += mt_utf32_cp_to_utf8_length(utf32);
        }
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = utf8Len;
    }

    return MT_SUCCESS;
}

mt_result mt_utf16ne_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf16le_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    } else {
        return mt_utf16be_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    }
}

mt_result mt_utf16le_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, MT_TRUE);
}

mt_result mt_utf16be_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, MT_FALSE);
}

mt_result mt_utf16_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (mt_has_utf16_bom((const mt_uint8*)pUTF16, utf16Len)) {
        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf16_bom_le((const mt_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            return mt_utf16le_to_utf8_length(pUTF8Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return mt_utf16be_to_utf8_length(pUTF8Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf16ne_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
}


mt_result mt_utf16_to_utf8_internal(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags, mt_bool32 isLE)
{
    size_t utf8CapOriginal = utf8Cap;
    mt_utf16 w1;
    mt_utf16 w2;
    mt_utf32 utf32;
    size_t utf8cpLen;   /* Code point length in UTF-8 code units. */

    if (pUTF8 == NULL) {
        return mt_utf16_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf16_bom((const mt_uint8*)pUTF16, utf16Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }
    }

    if (utf16Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF16[0] != 0) {
            if (isLE) {
                w1 = mt_le2host_16(pUTF16[0]);
            } else {
                w1 = mt_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        return MT_INVALID_ARGS; /* Ran out of input data. */
                    }

                    if (isLE) {
                        w2 = mt_le2host_16(pUTF16[1]);
                    } else {
                        w2 = mt_be2host_16(pUTF16[1]);
                    }
                    
                    if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                        utf32 = mt_utf16_pair_to_utf32_cp(pUTF16);
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                        }
                    }

                    pUTF16 += 2;
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                return MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
        }
    } else {
        /* Fixed length string. */
        size_t iUTF16;
        for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
            if (isLE) {
                w1 = mt_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = mt_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 >= utf16Len) {
                        return MT_INVALID_ARGS; /* Ran out of input data. */
                    }

                    if (isLE) {
                        w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                    } else {
                        w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                    }
                    
                    if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                        utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            return MT_INVALID_CODE_POINT;
                        } else {
                            /* Replacement. */
                            utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                        }
                    }

                    iUTF16 += 2;
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        return MT_INVALID_CODE_POINT;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                return MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
        }
    }
    
    /* Null terminate. */
    if (utf8Cap == 0) {
        return MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    }

    pUTF8[0] = 0;

    if (pUTF8Len != NULL) {
        *pUTF8Len = (utf8CapOriginal - utf8Cap);
    }

    return MT_SUCCESS;
}

mt_result mt_utf16ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, flags);
    } else {
        return mt_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, flags);
    }
}

mt_result mt_utf16le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, flags, MT_TRUE);
}

mt_result mt_utf16be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, flags, MT_FALSE);
}

mt_result mt_utf16_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (pUTF8 == NULL) {
        return mt_utf16_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf16_bom((const mt_uint8*)pUTF16, utf16Len)) {
        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf16_bom_le((const mt_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            return mt_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return mt_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf16ne_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, flags);
}
#endif  /* MINITYPE_IMPLEMENTATION */

/*
This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================
Copyright 2019 David Reid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
