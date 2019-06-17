/*
Typography library. Choice of public domain or MIT-0. See license statements at the end of this file.
minitype - v0.x.x - 2019-xx-xx

David Reid - davidreidsoftware@gmail.com
*/

/* WORK IN PROGRESS */

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

typedef char      mt_utf8;
typedef mt_uint16 mt_utf16;
typedef mt_uint32 mt_utf32;

/* Define NULL for some compilers. */
#ifndef NULL
#define NULL 0
#endif

#if defined(SIZE_MAX)
    #define MT_SIZE_MAX     SIZE_MAX
#else
    #define MT_SIZE_MAX     0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif

#define MT_PI       3.14159265358979323846
#define MT_PIF      3.14159265358979323846f

#define MT_DEGREES(radians)  ((double)((radians) * 57.29577951308232087685))
#define MT_DEGREESF(radians) ( (float)((radians) * 57.29577951308232087685f))
#define MT_RADIANS(degrees)  ((double)((degrees) *  0.01745329251994329577))
#define MT_RADIANSF(degrees) ( (float)((degrees) *  0.01745329251994329577f))


#ifdef _MSC_VER
#define MT_INLINE __forceinline
#else
#ifdef __GNUC__
#define MT_INLINE inline __attribute__((always_inline))
#else
#define MT_INLINE inline
#endif
#endif

#define MT_PRIVATE


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
typedef struct mt_brush mt_brush;
typedef struct mt_brush_config mt_brush_config;
typedef struct mt_gc mt_gc;
typedef struct mt_gc_config mt_gc_config;
typedef struct mt_item mt_item;
typedef struct mt_script_state mt_script_state;

/* Enumerators */
typedef enum
{
    mt_backend_gdi,            /* Typography via Uniscribe */
    /*mt_backend_direct2d,*/       /* Typography via DirectWrite */
    /*mt_backend_coregraphics,*/   /* Typography via Core Text */
    mt_backend_cairo,          /* Typography via Pango */
    /*mt_backend_xft*/             /* Typography via minitype */
} mt_backend;

typedef enum
{
    mt_font_weight_medium = 0,
    mt_font_weight_thin,
    mt_font_weight_extra_light,
    mt_font_weight_light,
    mt_font_weight_semi_light,
    mt_font_weight_book,
    mt_font_weight_semi_bold,
    mt_font_weight_bold,
    mt_font_weight_extra_bold,
    mt_font_weight_heavy,
    mt_font_weight_extra_heavy,
    mt_font_weight_normal  = mt_font_weight_medium,
    mt_font_weight_default = mt_font_weight_medium
} mt_font_weight;

typedef enum
{
    mt_font_slant_none = 0,
    mt_font_slant_italic,
    mt_font_slant_oblique
} mt_font_slant;

typedef enum
{
    mt_line_cap_flat   = 0,
    mt_line_cap_round  = 1,
    mt_line_cap_square = 2
} mt_line_cap;

typedef enum
{
    mt_line_join_miter = 0,
    mt_line_join_round = 1,
    mt_line_join_bevel = 2
} mt_line_join;

typedef enum
{
    mt_brush_type_solid  = 0,       /* RGB or RGBA */
    /*mt_brush_type_linear = 1,*/   /* Source is a linear gradient. */
    mt_brush_type_gc     = 2        /* Source is a graphics context. */
} mt_brush_type;

typedef enum
{
    mt_blend_op_src      = 0,       /* Default. Draws the source over the top of the destination with no blending. */
    mt_blend_op_src_over = 1        /* Standard alpha blending. */
} mt_blend_op;

typedef enum
{
    mt_antialias_mode_default = 0,  /* Let the backend decide, but prefer anti-aliasing if available. Will be the same as mt_antialias_none on GDI since GDI does not support anti-aliasing. */
    mt_antialias_mode_none    = 1   /* Anti-aliasing will be disabled. Useful for straight-edge primitives like un-rotated rectangles or where performance is a concern. */
} mt_antialias_mode;

typedef enum
{
    mt_stretch_filter_nearest = 0,  /* Default. */
    mt_stretch_filter_linear  = 1
} mt_stretch_filter;

typedef enum
{
    mt_format_unknown = 0,
    mt_format_rgba,
    mt_format_rgb,
    mt_format_bgra, /* Optimal format for GDI. */
    mt_format_bgr,
    mt_format_argb  /* Optimal format for Cairo. */
} mt_format;

/* Structures */
#if defined(MT_SUPPORT_GDI)
typedef struct 
{
    mt_uint16 uJustification : 4;
    mt_uint16 fClusterStart  : 1;
    mt_uint16 fDiacritic     : 1;
    mt_uint16 fZeroWidth     : 1;
    mt_uint16 fReserved      : 1;
    mt_uint16 fShapeReserved : 8;
} MT_SCRIPT_VISATTR;

typedef struct
{
    mt_uint16 uBidiLevel         : 5;
    mt_uint16 fOverrideDirection : 1;
    mt_uint16 fInhibitSymSwap    : 1;
    mt_uint16 fCharShape         : 1;
    mt_uint16 fDigitSubstitute   : 1;
    mt_uint16 fInhibitLigate     : 1;
    mt_uint16 fDisplayZWG        : 1;
    mt_uint16 fArabicNumContext  : 1;
    mt_uint16 fGcpClusters       : 1;
    mt_uint16 fReserved          : 1;
    mt_uint16 fEngineReserved    : 2;
} MT_SCRIPT_STATE;

typedef struct
{
    mt_uint16 eScript       : 10;
    mt_uint16 fRTL          : 1;
    mt_uint16 fLayoutRTL    : 1;
    mt_uint16 fLinkBefore   : 1;
    mt_uint16 fLinkAfter    : 1;
    mt_uint16 fLogicalOrder : 1;
    mt_uint16 fNoGlyphIndex : 1;
    MT_SCRIPT_STATE s;
} MT_SCRIPT_ANALYSIS;

typedef struct
{
    int iCharPos;
    MT_SCRIPT_ANALYSIS a;
} MT_SCRIPT_ITEM;
#endif

typedef struct
{
    mt_uint8 r;
    mt_uint8 g;
    mt_uint8 b;
    mt_uint8 a;
} mt_color;

typedef struct
{
    mt_uint32 index;
    mt_int32 advance;
    union
    {
    #if defined(MT_SUPPORT_GDI)
        struct
        {
            MT_SCRIPT_VISATTR sv;   /* Passed around to ScriptShape() and ScriptPlace(). */
            long offsetX;           /* For filling in the GOFFSET parameter of ScriptTextOut(). */
            long offsetY;           /* ^^^ */
        } gdi;
    #endif
        mt_uint32 _unused;
    } backend;
} mt_glyph;

typedef struct
{
    mt_int32 lPadding;
    mt_int32 rPadding;
    mt_int32 sizeX;
    mt_int32 sizeY;
} mt_text_metrics;

typedef struct
{
    mt_int32 ascent;
    mt_int32 descent;
    mt_int32 lineHeight;
} mt_font_metrics;

typedef struct
{
    mt_int32 width;
    mt_int32 height;
    mt_int32 originX;
    mt_int32 originY;
    mt_int32 advanceX;
    mt_int32 advanceY;
} mt_glyph_metrics;

struct mt_item
{
    size_t offset;  /* Offset in code units (bytes for UTF-8, shorts for UTF-16, integers for UTF-32. */
    size_t length;  /* Length in code units. */

    /* Backend-specific data. */
    union
    {
    #if defined(MT_SUPPORT_GDI)
        struct
        {
            MT_SCRIPT_ANALYSIS sa;   /* Passed around to Script*() APIs. */
        } gdi;
    #endif
        mt_uint32 _unused;
    } backend;
};


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
    void      (* uninit)             (mt_api* pAPI);
    mt_result (* itemizeUTF8)        (mt_api* pAPI, const mt_utf8* pTextUTF8, size_t textLength, mt_item* pItems, mt_uint32* pItemCount);
    mt_result (* itemizeUTF16)       (mt_api* pAPI, const mt_utf16* pTextUTF16, size_t textLength, mt_item* pItems, mt_uint32* pItemCount);
    mt_result (* itemizeUTF32)       (mt_api* pAPI, const mt_utf32* pTextUTF32, size_t textLength, mt_item* pItems, mt_uint32* pItemCount);
    mt_result (* shapeUTF8)          (mt_font* pFont, mt_item* pItem, const mt_utf8* pTextUTF8, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters);
    mt_result (* shapeUTF16)         (mt_font* pFont, mt_item* pItem, const mt_utf16* pTextUTF16, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters);
    mt_result (* shapeUTF32)         (mt_font* pFont, mt_item* pItem, const mt_utf32* pTextUTF32, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters);
    mt_result (* place)              (mt_font* pFont, mt_item* pItem, mt_glyph* pGlyphs, size_t glyphCount, mt_text_metrics* pRunMetrics);
    mt_result (* fontInit)           (mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont);
    void      (* fontUninit)         (mt_font* pFont);
    mt_result (* brushInit)          (mt_api* pAPI, const mt_brush_config* pConfig, mt_brush* pBrush);
    void      (* brushUninit)        (mt_brush* pBrush);
    void      (* brushSetOrigin)     (mt_brush* pBrush, mt_int32 x, mt_int32 y);
    mt_result (* gcInit)             (mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC);
    void      (* gcUninit)           (mt_gc* pGC);
    mt_result (* gcSave)             (mt_gc* pGC);
    mt_result (* gcRestore)          (mt_gc* pGC);
    void      (* gcTranslate)        (mt_gc* pGC, mt_int32 offsetX, mt_int32 offsetY);
    void      (* gcRotate)           (mt_gc* pGC, float rotationInRadians);
    void      (* gcScale)            (mt_gc* pGC, float scaleX, float scaleY);
    void      (* gcSetLineWidth)     (mt_gc* pGC, mt_int32 width);
    void      (* gcSetLineCap)       (mt_gc* pGC, mt_line_cap cap);
    void      (* gcSetLineJoin)      (mt_gc* pGC, mt_line_join join);
    void      (* gcSetMiterLimit)    (mt_gc* pGC, float limit);
    void      (* gcSetLineDash)      (mt_gc* pGC, mt_uint32 count, float* dashes);
    void      (* gcSetLineBrush)     (mt_gc* pGC, mt_brush* pBrush);
    void      (* gcSetLineBrushSolid)(mt_gc* pGC, mt_color color);
    void      (* gcSetLineBrushGC)   (mt_gc* pGC, mt_gc* pSrcGC);
    void      (* gcSetFillBrush)     (mt_gc* pGC, mt_brush* pBrush);
    void      (* gcSetFillBrushSolid)(mt_gc* pGC, mt_color color);
    void      (* gcSetFillBrushGC)   (mt_gc* pGC, mt_gc* pSrcGC);
    void      (* gcSetFont)          (mt_gc* pGC, mt_font* pFont);
    void      (* gcSetTextFGColor)   (mt_gc* pGC, mt_color fgColor);
    void      (* gcSetTextBGColor)   (mt_gc* pGC, mt_color bgColor);
    void      (* gcSetBlendOp)       (mt_gc* pGC, mt_blend_op op);
    void      (* gcSetAntialiasMode) (mt_gc* pGC, mt_antialias_mode mode);
    void      (* gcSetStretchFilter) (mt_gc* pGC, mt_stretch_filter filter);
    void      (* gcMoveTo)           (mt_gc* pGC, mt_int32 x, mt_int32 y);
    void      (* gcLineTo)           (mt_gc* pGC, mt_int32 x, mt_int32 y);
    void      (* gcRectangle)        (mt_gc* pGC, mt_int32 left, mt_int32 top, mt_int32 right, mt_int32 bottom);
    void      (* gcArc)              (mt_gc* pGC, mt_int32 x, mt_int32 y, mt_int32 radius, float angle1InRadians, float angle2InRadians);
    void      (* gcCurveTo)          (mt_gc* pGC, mt_int32 x1, mt_int32 y1, mt_int32 x2, mt_int32 y2, mt_int32 x3, mt_int32 y3);
    void      (* gcClosePath)        (mt_gc* pGC);
    void      (* gcClip)             (mt_gc* pGC);
    void      (* gcResetClip)        (mt_gc* pGC);
    mt_bool32 (* gcIsPointInsideClip)(mt_gc* pGC, mt_int32 x, mt_int32 y);
    void      (* gcFill)             (mt_gc* pGC);
    void      (* gcStroke)           (mt_gc* pGC);
    void      (* gcFillAndStroke)    (mt_gc* pGC);
    void      (* gcDrawGC)           (mt_gc* pGC, mt_gc* pSrcGC, mt_int32 srcX, mt_int32 srcY);
    void      (* gcDrawGlyphs)       (mt_gc* pGC, const mt_item* pItem, const mt_glyph* pGlyphs, size_t glyphCount, mt_int32 x, mt_int32 y);

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
        /*HDC*/ mt_handle hGlobalDC;    /* Global DC for font creation and glyph shaping and placement. */
        /*HBRUSH*/ mt_handle hStockSolidFillBrush;  /* The GDI stock brush to use for solid brushes. */

        /* usp10.dll */
        mt_handle hUsp10DLL;

        /* gdi32.dll */
        mt_handle hGdi32DLL;

        /* msimg32.dll */
        mt_handle hMsimg32DLL;
        mt_proc AlphaBlend;

        mt_bool32 ownsGlobalDC : 1;
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
    mt_font_weight weight;
    mt_font_slant slant;
    mt_bool32 noClearType;
};

struct mt_font
{
    mt_api* pAPI;   /* The mt_api object that was used to initialize the font. */
    mt_font_metrics metrics;

#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HFONT*/ mt_handle hFont;
        /*SCRIPT_CACHE*/ mt_ptr sc; /* The SCRIPT_CACHE object passed around to ScriptShape(), ScriptPlace() and ScriptTextOut(). */
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

struct mt_brush_config
{
    mt_brush_type type;
    struct
    {
        mt_color color;
    } solid;
    /*struct
    {
        mt_int32 x0;
        mt_int32 y0;
        mt_int32 x1;
        mt_int32 x2;
    } linear;*/
    struct
    {
        mt_gc* pGC;
    } gc;
};

struct mt_brush
{
    mt_api* pAPI;
    mt_brush_config config;
#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HBRUSH*/  mt_handle hBrush;
        /*HBITMAP*/ mt_handle hBitmap;  /* Only used for complex brushes. Owned by the mt_brush object and deleted in mt_brush_uninit(). */
        mt_int32 originX;
        mt_int32 originY;
        mt_bool32 ownsBitmap : 1;       /* When set to true, hBitmap will be destroyed in mt_brush_uninit(). */
    } gdi;
#endif
};

struct mt_gc_config
{
    mt_uint32 sizeX;                    /* Set this to 0 if you are passing in a pre-existing backend context or surface (such as a HDC or cairo_surface). */
    mt_uint32 sizeY;                    /* Set this to 0 if you are passing in a pre-existing backend context or surface (such as a HDC or cairo_surface). */
    mt_uint32 stride;                   /* Stride in pixels. Only used when pInitialImageData is not null. */
    mt_format format;                   /* The format of the data contained in pInitialImageData (if any) and the preferred internal format. Cannot be mt_format_unkonwn if pInitialImageData is not null. */
    const void* pInitialImageData;      /* Can be null in which case the initial contents are undefined. */

#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HDC*/ mt_handle hDC;          /* Existing HDC to use as the rendering target. Set this for transient context's such as those used in BeginPaint()/EndPaint() pairs. */
    } gdi;
#endif
};

#if defined(MT_SUPPORT_GDI)
typedef struct
{
    /*HPEN*/ mt_handle hPen;            /* Lazily initialized. Set to NULL to indicate the case when a new pen handle needs to be created. */
    mt_int32 lineWidth;
    mt_line_cap lineCap;
    mt_line_join lineJoin;
    mt_uint32 dashCount;
    float dashes[16];
    mt_blend_op blendOp;
    mt_font* pFont;
    mt_brush_config lineStockBrush;     /* For use with solid and GC brushes. Only used when pUserLineBrush is NULL. */
    mt_brush* pUserLineBrush;           /* For use with user-defined brushes. */
    mt_brush transientFillBrush;        /* For use with solid and GC brushes. Only used when pUserFillBrush is NULL. */
    mt_brush* pUserFillBrush;           /* For use with user-defined brushes. */
    mt_bool32 hasTransientFillBrush : 1;
    mt_bool32 hasPathBegun : 1;         /* Determines whether or not BeginPath has been called. */
} mt_gc_state_gdi;
#endif

struct mt_gc
{
    mt_api* pAPI;                       /* The mt_api object that was used to initialize the graphics context. */
    mt_format format;
    mt_bool32 isTransient : 1;

#if defined(MT_SUPPORT_GDI)
    struct
    {
        /*HDC*/ mt_handle hDC;
        /*HBITMAP*/ mt_handle hBitmap;  /* The HBITMAP object that will be used as the target for graphics output. */
        void* pBitmapData;              /* A pointer to the raw bitmap data. Owned by hBitmap. Freed by GDI when hBitmap is deleted. */
        mt_uint32 bitmapSizeX;
        mt_uint32 bitmapSizeY;
        mt_gc_state_gdi* pState;        /* Heap allocated via realloc() for now. May change to a per-API allocation scheme. */
        mt_uint32 stateCap;             /* The capacity of pState. */
        mt_uint32 stateCount;           /* The number of valid items in pState. */
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



/**************************************************************************************************************************************************************

Unicode

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
mt_result mt_utf8_to_utf16ne_length(size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }
mt_result mt_utf8_to_utf16le_length(size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }
mt_result mt_utf8_to_utf16be_length(size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }

/*
Converts a UTF-8 string to a UTF-16 string.

Parameters
----------
[pUTF16](in)
    The buffer that receives the converted string data. This can be NULL in which case only the length is calculated.

[utf16Cap](in)
    The capacity of the buffer pointed to by pUTF16.

[pUTF16Len](out, optional)
    A pointer to the variable that will receive the length of the output string not including the null terminator. When
    an error occurs this will be set to the number of UTF-16 code units that were output up to the error.

[pUTF8](in)
    The UTF-8 string to convert. It is an error for this to be null.

[utf8Len](in)
    The length of the UTF-8 string in bytes, not including the null terminator. 

[pUTF8LenProcessed](out, optional)
    A pointer to a variable that will received the number of bytes that were processed in [pUTF8], not including the null
    terminator. This is can be NULL.

[flags](in, optional)
    This can be a combination of the following:
        MT_FORBID_BOM
            Causes an error if the string starts with a BOM.
        MT_ERROR_ON_INVALID_CODE_POINT
            When an invalid character is encounted, return an error instead of replacing it with the replacement character.
    This can be 0 in which case it will use defaults.


Return Value
------------
MT_SUCCESS if successful; an error result otherwise. Converting an empty string is valid.

MT_INVALID_BOM will be returned if the string starts with a byte order mark and MT_FORBID_BOM is set in [flags].

MT_INVALID_ARGS will be returned if [pUTF8] is null.

MT_INVALID_ARGS will be returned if [pUTF8] is not big enough to fully convert the last code point.

MT_OUT_OF_MEMORY will be returned if the [pUTF16] buffer is too small to contain the entire converted string including the null terminated.

MT_INVALID_CODE_POINT will be returned if an invalid code point is encounted and MT_ERROR_ON_INVALID_CODE_POINT is set in [flags].


Remarks
-------
The output string will always be null terminated. If the output buffer is not large enough to store the null terminator MT_OUT_OF_MEMORY will be returned.

When the output string ([pUTF16]) is null, this is equivalent to `mt_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags)`.

The byte order mark will _not_ be included in the output string nor it's length.

Invalid characters are replaced with the Unicode replacement character (MT_UNICODE_REPLACEMENT_CHARACTER/U+FFFD) unless the MT_ERROR_ON_INVALID_CODE_POINT
flag is set in which case an error will be returned.

You can look at [pUTF16Len] to know how many output characters were processed and [pUTF8LenProcessed] to know how many input characters were processed even
when an error occurs. This is useful if you want to implement a streaming type of system where you have a fixed sized buffer and want to continously process
a large string in a loop. Note that when an error is returned, the output buffer will _not_ be null terminated. You should only do this if MT_OUT_OF_MEMORY
or MT_SUCCESS is returned. MT_OUT_OF_MEMORY will be returned when the output buffer is not large enough to store the entire output string with the null
terminator.
*/
mt_result mt_utf8_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);
mt_result mt_utf8_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);
mt_result mt_utf8_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);


/*
Calculates the number of UTF-32's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf8_to_utf32ne/le/be(), except it doesn't actually do any conversion.
*/
mt_result mt_utf8_to_utf32_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags);
mt_result mt_utf8_to_utf32ne_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }
mt_result mt_utf8_to_utf32le_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }
mt_result mt_utf8_to_utf32be_length(size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, mt_uint32 flags) { return mt_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }

/*
Converts a UTF-8 string to a UTF-32 string.
*/
mt_result mt_utf8_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);
mt_result mt_utf8_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);
mt_result mt_utf8_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags);


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
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf8_length(size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string.
*/
mt_result mt_utf16ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);
mt_result mt_utf16le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);
mt_result mt_utf16be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);


/*
Calculates the number of UTF-16's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf16/be/le_to_utf32/be/le(), except it doesn't actually do any conversion.
*/
mt_result mt_utf16ne_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16le_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16be_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);
mt_result mt_utf16ne_to_utf32ne_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags) { return mt_utf16ne_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }
mt_result mt_utf16le_to_utf32le_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags) { return mt_utf16le_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }
mt_result mt_utf16be_to_utf32be_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags) { return mt_utf16be_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }

/*
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-32 string.
*/
mt_result mt_utf16ne_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);
mt_result mt_utf16le_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);
mt_result mt_utf16be_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);

/*
Converts a UTF-16 string to a UTF-32 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf16_to_utf32(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags);



/*
Calculates the number of UTF-8's required to fully contain the given UTF-32 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf32/be/le_to_utf8(), except it doesn't actually do any conversion.
*/
mt_result mt_utf32ne_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);
mt_result mt_utf32le_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);
mt_result mt_utf32be_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);

/*
Calculates the number of UTF-8's required to fully contain the given UTF-32 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf32_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);

/*
Converts a UTF-32 string to a UTF-8 string.
*/
mt_result mt_utf32ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);
mt_result mt_utf32le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);
mt_result mt_utf32be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);

/*
Converts a UTF-32 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf32_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);



/*
Calculates the number of UTF-16's required to fully contain the given UTF-32 string after conversion, not including the null terminator.

Remarks
-------
This is the same as mt_utf32/be/le_to_utf16/be/le(), except it doesn't actually do any conversion.
*/
mt_result mt_utf32ne_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);
mt_result mt_utf32le_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);
mt_result mt_utf32be_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);
mt_result mt_utf32ne_to_utf16ne_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags) { return mt_utf32ne_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }
mt_result mt_utf32le_to_utf16le_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags) { return mt_utf32le_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }
mt_result mt_utf32be_to_utf16be_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags) { return mt_utf32be_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }

/*
Calculates the number of UTF-16's required to fully contain the given UTF-32 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf32_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags);

/*
Converts a UTF-32 string to a UTF-16 string.
*/
mt_result mt_utf32ne_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);
mt_result mt_utf32le_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);
mt_result mt_utf32be_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);

/*
Converts a UTF-32 string to a UTF-16 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
mt_result mt_utf32_to_utf16(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags);



/**************************************************************************************************************************************************************

API

**************************************************************************************************************************************************************/
mt_result mt_init(const mt_api_config* pConfig, mt_api* pAPI);
void mt_uninit(mt_api* pAPI);


/**************************************************************************************************************************************************************

Itemization, Shaping and Placement

**************************************************************************************************************************************************************/
/*
Itemizes a UTF-8 string in preparation for shaping, placement and drawing.

[pItems](out, optional)
    A pointer to a buffer that will receive the items. Can be NULL, in which case only the item count is returned.

[pItemCount](in, out)
    A pointer to an unsigned integer that, on input, contains the capacity of [pItems] and on output will receive the actual item count. Cannot be NULL.


Return Value
------------
MT_SUCCESS is returned on success.

MT_INVALID_ARGS will be returned if [pText] is NULL, [textLength] is zero or [pItemCount] is NULL.

MT_NO_SPACE will be returned when [pItems] is not large enough to contain the items. In this case, [pItemCount] will be set to the required count.

Remarks
-------
This is the first function you should call when preparing text for drawing.
*/
mt_result mt_itemize_utf8(mt_api* pAPI, const mt_utf8* pTextUTF8, size_t textLength, mt_item* pItems, mt_uint32* pItemCount);

/*
Performs text shaping on the given text run. This function is used to convert text to glyphs.

[pFont](in)
    The font to use for the shaping.

[pItem](in, out)
    A pointer to the mt_item object from a previous call to mt_itemize*().

[pText](in)
    The text to itemize.

[textLength](in)
    The length of the text in code units.

[pGlyphs](out)
    A pointer to a buffer that will receive the glyphs for the given text.

[pGlyphCount](in, out)
    A pointer to the variable that, on input, contains the size of [pGlyphs] and on output will receive the actual glyph count.

[pClusters](out)
    A pointer to a buffer that will receive the clusters for the given text. The length of this buffer must be at least [textLength] size_t's. This will
    receive the index of the glyph that each code unit in [pTextUTF8] maps to.

Remarks
-------
You should call mt_itemize_utf8() before calling this function.

You can determine the number glyphs required for the input string by calling this with [pGlyphs] set the NULL.
*/
mt_result mt_shape_utf8(mt_font* pFont, mt_item* pItem, const mt_utf8* pTextUTF8, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters);

/*
Performs glyphs placement.

[pItem](in, out)
    A pointer to the mt_item object from a previous call to mt_itemize*().

[pGlyphs](in, out)
    A pointer to a buffer containing the glyphs to place.

[glyphCount](in)
    The number of glyphs in [pGlyphs].

[pRunMetrics](out, optional)
    A pointer to a mt_text_metrics object that will receive the width and height of the entire run as a whole.

Remarks
-------
You should call mt_shape_*() before calling this function.

This will modify the [x] and [y] members of each glyph in [pGlyphs].
*/
mt_result mt_place(mt_font* pFont, mt_item* pItem, mt_glyph* pGlyphs, size_t glyphCount, mt_text_metrics* pRunMetrics);


/*
Measure the length of a glyph string in pixels.
*/
mt_result mt_measure_x(mt_api* pAPI, mt_item* pItem, size_t glyphCount, const mt_glyph* pGlyphs, mt_int32* pX);

/*
Retrieves the index of the Unicode code point under the given pixel position.
*/
mt_result mt_x_to_cp(mt_api* pAPI, mt_item* pItem, mt_int32 x, size_t textLength, const size_t* pClusters, size_t glyphCount, const mt_glyph* pGlyphs, size_t* piCP, mt_int32* pOffsetToEdge);

/*
Retrieves the position within a string that contains the given pixel position.
*/
mt_result mt_cp_to_x(mt_api* pAPI, mt_item* pItem, size_t iCP, size_t textLength, const size_t* pClusters, size_t glyphCount, const mt_glyph* pGlyphs, mt_int32* pX);


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
/*
Initializes a font.
*/
mt_result mt_font_init(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont);

/*
Uninitializes a font.

Remarks
-------
Do not uninitialize a font while it is being used in a GC.
*/
void mt_font_uninit(mt_font* pFont);


/**************************************************************************************************************************************************************

Graphics

The graphics API uses a path + fill/stroke system for drawing primitives. It is roughly based on the APIs of GDI and Cairo.

The background of a path is filled using the current brush which is set with one of the following APIs:
    - mt_gc_set_fill_brush / mt_gc_set_line_brush
        Sets the current brush to a mt_brush object.
    - mt_gc_set_fill_brush_solid / mt_gc_set_line_brush_solid
        Sets the current brush to a solid color.
    - mt_gc_set_fill_brush_gc / mt_gc_set_line_brush_gc
        Sets the current brush to the current state of another graphics context. This is how you draw the contents of one GC to another.

Filling and stroking use their own separate brushes. The reason for this is to support the mt_gc_fill_and_stroke() API. This API performs a fill, followed by
a stroke with the important property that it does not cause flickering for single buffered environments. Since this is a single API, it's useful to have
separate brushes for the fill and stroke.

The graphics API uses a state system to determine how to draw something. For example, to set the width of a line you would call mt_gc_set_line_width() before
stroking the current path. The current state will remain until it's changed, unless otherwise noted. In order to simplify graphics backends, there are currently
no APIs for retrieving the current state - you will need to track this manually if required.

**************************************************************************************************************************************************************/

/******************************************************************************

Brushes

******************************************************************************/
mt_result mt_brush_init(mt_api* pAPI, const mt_brush_config* pConfig, mt_brush* pBrush);

/*
Uninitializes a brush.

Remarks
-------
Do not uninitialize a brush while it is being used in a GC.
*/
void mt_brush_uninit(mt_brush* pBrush);
void mt_brush_set_origin(mt_brush* pBrush, mt_int32 x, mt_int32 y);

/******************************************************************************

Graphics Context

******************************************************************************/
mt_result mt_gc_init(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC);
void mt_gc_uninit(mt_gc* pGC);

/******************************************************************************

State Management

******************************************************************************/
mt_result mt_gc_save(mt_gc* pGC);
mt_result mt_gc_restore(mt_gc* pGC);
void mt_gc_translate(mt_gc* pGC, mt_int32 offsetX, mt_int32 offsetY);
void mt_gc_rotate(mt_gc* pGC, float rotationInRadians);
void mt_gc_scale(mt_gc* pGC, float scaleX, float scaleY);
void mt_gc_set_miter_limit(mt_gc* pGC, float limit);
void mt_gc_set_line_width(mt_gc* pGC, mt_int32 width);
void mt_gc_set_line_cap(mt_gc* pGC, mt_line_cap cap);
void mt_gc_set_line_join(mt_gc* pGC, mt_line_join join);
void mt_gc_set_line_dash(mt_gc* pGC, mt_uint32 count, float* dashes);  /* Max value for <count> is 16. */
void mt_gc_set_line_brush(mt_gc* pGC, mt_brush* pBrush);
void mt_gc_set_line_brush_solid(mt_gc* pGC, mt_color color);
void mt_gc_set_line_brush_gc(mt_gc* pGC, mt_gc* pSrcGC);
void mt_gc_set_fill_brush(mt_gc* pGC, mt_brush* pBrush);
void mt_gc_set_fill_brush_solid(mt_gc* pGC, mt_color color);
void mt_gc_set_fill_brush_gc(mt_gc* pGC, mt_gc* pSrcGC);
void mt_gc_set_font(mt_gc* pGC, mt_font* pFont);
void mt_gc_set_text_fg_color(mt_gc* pGC, mt_color fgColor);
void mt_gc_set_text_bg_color(mt_gc* pGC, mt_color bgColor);
void mt_gc_set_blend_op(mt_gc* pGC, mt_blend_op op);
void mt_gc_set_antialias_mode(mt_gc* pGC, mt_antialias_mode mode);
void mt_gc_set_stretch_filter(mt_gc* pGC, mt_stretch_filter filter);

/******************************************************************************

Paths

******************************************************************************/
void mt_gc_move_to(mt_gc* pGC, mt_int32 x, mt_int32 y);
void mt_gc_line_to(mt_gc* pGC, mt_int32 x, mt_int32 y);
void mt_gc_rectangle(mt_gc* pGC, mt_int32 left, mt_int32 top, mt_int32 right, mt_int32 bottom);
void mt_gc_arc(mt_gc* pGC, mt_int32 x, mt_int32 y, mt_int32 radius, float angle1InRadians, float angle2InRadians);
void mt_gc_curve_to(mt_gc* pGC, mt_int32 x1, mt_int32 y1, mt_int32 x2, mt_int32 y2, mt_int32 x3, mt_int32 y3);  /* Cubic Bézier. Starting poing is the current position. x3y3 is the end point. x1y1 and x2y2 are the control points. */
void mt_gc_close_path(mt_gc* pGC);

/******************************************************************************

Filling, Stroking and Clipping

******************************************************************************/
/*
Sets the clipping region to the current path as if it were filled. Anything outside of this region will be clipped, including
subsequent calls to mt_gc_clip(). You can use mt_gc_reset_clip() to clear the clipping region. You can also use mt_gc_push()
and mt_gc_pop() to save and restore clipping regions.

Use mt_gc_is_point_inside_clip() to determine if a point is within the clipping region.

This will clear the path.
*/
void mt_gc_clip(mt_gc* pGC);

/*
Clears the clipping region.
*/
void mt_gc_reset_clip(mt_gc* pGC);

/*
Determines if a point is within the current clipping region.
*/
mt_bool32 mt_gc_is_point_inside_clip(mt_gc* pGC, mt_int32 x, mt_int32 y);

/*
Fills the current path using the current fill settings.

This will clear the path.
*/
void mt_gc_fill(mt_gc* pGC);

/*
Strokes the current path using the current line settings and the current brush.

This will clear the path.
*/
void mt_gc_stroke(mt_gc* pGC);

/*
Fills the current path, then strokes it. The fill will never overlap the stroke.

This will clear the path.
*/
void mt_gc_fill_and_stroke(mt_gc* pGC);

/*
Draws the contents of the given graphics context.

This will be positioned, rotated and scaled based on the current transform. srcX and srcY are the offset inside pSrcGC to use as the origin.

The image will be clipped against the current clipping region. Use the clip to restrict the iamge to a particular subregion of the source
image or if you want to draw a non-rectangular shape.
*/
void mt_gc_draw_gc(mt_gc* pGC, mt_gc* pSrcGC, mt_int32 srcX, mt_int32 srcY);

/*
Draws a glyph string using the currently selected font.
*/
void mt_gc_draw_glyphs(mt_gc* pGC, const mt_item* pItem, const mt_glyph* pGlyphs, size_t glyphCount, mt_int32 x, mt_int32 y);


/******************************************************************************

Graphics Helpers

******************************************************************************/
/* Helper API for creating an RGBA color. */
MT_INLINE mt_color mt_rgba(mt_uint8 r, mt_uint32 g, mt_uint8 b, mt_uint8 a)
{
    mt_color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

/* Helper API for creating an opaque RGB color. */
MT_INLINE mt_color mt_rgb(mt_uint8 r, mt_uint32 g, mt_uint8 b)
{
    return mt_rgba(r, g, b, 255);
}

/*
Copies and converts image data.

You are allowed to specify the same format for the source and target in which case this performs a direct copy with no conversion.
*/
void mt_copy_image_data(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_format dstFormat, mt_uint32 srcStride, mt_format srcFormat);

/*
Copies the given image data and flips it.
*/
void mt_copy_and_flip_image_data_y(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_format dstFormat, mt_uint32 srcStride, mt_format srcFormat);

/*
Flips the given image data horizontally, in place.
*/
void mt_flip_image_data_y(void* pDst, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 stride, mt_format format);

/*
Retrieves the number of bytes per pixel for the given format.
*/
mt_uint32 mt_get_bytes_per_pixel(mt_format format);

/*
Determines whether or not there's an alpha channel in the given format.
*/
mt_bool32 mt_format_has_alpha(mt_format format);


/**************************************************************************************************************************************************************

Utilities

**************************************************************************************************************************************************************/
/*
Checks if the given string is NULL or empty.
*/
mt_bool32 mt_is_null_or_empty_utf8(const mt_utf8* pUTF8);
mt_bool32 mt_is_null_or_empty(const mt_utf8* pUTF8) { return mt_is_null_or_empty_utf8(pUTF8); }

/*
Checks if the given string is NULL or contains only whitespace characters.
*/
mt_bool32 mt_is_null_or_whitespace_utf8(const mt_utf8* pUTF8);
mt_bool32 mt_is_null_or_whitespace(const mt_utf8* pUTF8) { return mt_is_null_or_whitespace_utf8(pUTF8); }

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

#ifndef MT_CALLOC
#ifdef MT_WIN32
#define MT_CALLOC(c, sz) HeapAlloc(GetProcessHeap(), 0x00000008, (c)*(sz))
#else
#define MT_CALLOC(c, sz) calloc((c), (sz))
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
#elif defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN
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
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
}

static MT_INLINE mt_uint32 mt_be2host_32(mt_uint32 n)
{
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
}

static MT_INLINE mt_uint16 mt_le2host_16(mt_uint16 n)
{
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
}

static MT_INLINE mt_uint32 mt_le2host_32(mt_uint32 n)
{
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
}


static MT_INLINE mt_uint16 mt_host2be_16(mt_uint16 n)
{
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
}

static MT_INLINE mt_uint32 mt_host2be_32(mt_uint32 n)
{
    if (mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
}

static MT_INLINE mt_uint16 mt_host2le_16(mt_uint16 n)
{
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint16(n);
    }

    return n;
}

static MT_INLINE mt_uint32 mt_host2le_32(mt_uint32 n)
{
    if (!mt_is_little_endian()) {
        return mt_swap_endian_uint32(n);
    }

    return n;
}


mt_result mt_result_from_errno(errno_t e)
{
    switch (e) {
        case EACCES: return MT_ACCESS_DENIED;
        case EEXIST: return MT_ALREADY_EXISTS;
        case EINVAL: return MT_INVALID_ARGS;
        case EMFILE: return MT_TOO_MANY_OPEN_FILES;
        case ENOENT: return MT_DOES_NOT_EXIST;
        case ENOMEM: return MT_OUT_OF_MEMORY;
        case EBADF:  return MT_INVALID_ARGS;   /* Can be returned by ftell() and family. */
        default: break;
    }

    return MT_ERROR;
}


mt_handle mt_dlopen(const char* filename)
{
    mt_handle handle;

#ifdef _WIN32
    handle = (mt_handle)LoadLibraryA(filename);
#else
    handle = (mt_handle)dlopen(filename, RTLD_NOW);
#endif

    return handle;
}

void mt_dlclose(mt_handle handle)
{
#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose((void*)handle);
#endif
}

mt_proc mt_dlsym(mt_handle handle, const char* symbol)
{
    mt_proc proc;

#ifdef _WIN32
    proc = (mt_proc)GetProcAddress((HMODULE)handle, symbol);
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif
    proc = (mt_proc)dlsym((void*)handle, symbol);
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #pragma GCC diagnostic pop
#endif
#endif

    return proc;
}



MT_INLINE int mt_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#ifdef _MSC_VER
    return strncpy_s(dst, dstSizeInBytes, src, count);
#else
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        // -1 = _TRUNCATE
        maxcount = dstSizeInBytes - 1;
    }

    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
#endif
}



/**************************************************************************************************************************************************************

Unicode

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
            pUTF8[0] = 0xC0 | ((utf32 & 0x07C0) >> 6);
            pUTF8[1] = 0x80 |  (utf32 & 0x003F);
            return 2;
        }
    }
    if (utf32 <= 0xFFFF) {
        if (utf8Cap >= 3) {
            pUTF8[0] = 0xE0 | ((utf32 & 0xF000) >> 12);
            pUTF8[1] = 0x80 | ((utf32 & 0x0FC0) >>  6);
            pUTF8[2] = 0x80 |  (utf32 & 0x003F);
            return 3;
        }
    }
    if (utf32 <= 0x10FFFF) {
        if (utf8Cap >= 4) {
            pUTF8[0] = 0xF0 | ((utf32 & 0x1C0000) >> 18);
            pUTF8[1] = 0x80 | ((utf32 & 0x03F000) >> 12);
            pUTF8[2] = 0x80 | ((utf32 & 0x000FC0) >>  6);
            pUTF8[3] = 0x80 |  (utf32 & 0x00003F);
            return 4;
        }
    }

    /* Getting here means there was not enough room in the output buffer. */
    return 0;
}

MT_INLINE size_t mt_utf32_cp_to_utf16_length(mt_utf32 utf32)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MT_ASSERT(utf32 <= MT_UNICODE_MAX_CODE_POINT);
    MT_ASSERT(mt_is_cp_in_surrogate_pair_range(utf32) == MT_FALSE);

    if (utf32 <= 0xFFFF) {
        return 1;
    } else {
        return 2;
    }

    /* Invalid. This function assume's the UTF-32 code point is valid so do an assert. */
    MT_ASSERT(MT_FALSE);
    return 0; /* Invalid. */
}

MT_INLINE size_t mt_utf32_cp_to_utf16(mt_utf32 utf32, mt_utf16* pUTF16, size_t utf16Cap)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MT_ASSERT(utf32 <= MT_UNICODE_MAX_CODE_POINT);
    MT_ASSERT(mt_is_cp_in_surrogate_pair_range(utf32) == MT_FALSE);
    MT_ASSERT(utf16Cap > 1);

    if (utf32 <= 0xFFFF) {
        if (utf16Cap >= 1) {
            pUTF16[0] = utf32;
            return 1;
        }
    } else {
        if (utf16Cap >= 2) {
            mt_utf32_cp_to_utf16_pair(utf32, pUTF16);
            return 2;
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
    mt_result result = MT_SUCCESS;
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

            if ((mt_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                pUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16. */
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        pUTF8    += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
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
                            result = MT_INVALID_CODE_POINT;
                            break;
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
        size_t iUTF8;
        for (iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if ((mt_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                iUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf16Len += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 > utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 > utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 > utf8Len) {
                            return MT_INVALID_ARGS;
                        }

                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
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
                            result = MT_INVALID_CODE_POINT;
                            break;
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

    return result;
}

mt_result mt_utf8_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result = MT_SUCCESS;
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
        const mt_utf8* pUTF8Original = pUTF8;
        while (pUTF8[0] != 0) {
            if (utf16Cap == 1) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if ((mt_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[0];
                pUTF16   += 1;
                utf16Cap -= 1;
                pUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
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
                                result = MT_INVALID_ARGS; /* Input string is too short. */
                                break;
                            }

                            mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                            if (!mt_is_valid_code_point(cp)) {
                                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    result = MT_INVALID_CODE_POINT;
                                    break;
                                } else {
                                    /* Replacement. */
                                    pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                                    pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
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
                            result = MT_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            pUTF8    += 1;
                        }
                    }
                }
            }
        }

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = (pUTF8 - pUTF8Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF8;
        for (iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (utf16Cap == 1) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if ((mt_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[iUTF8+0];
                pUTF16   += 1;
                utf16Cap -= 1;
                iUTF8    += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap -= MT_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 > utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 > utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        pUTF16[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (utf16Cap < 2) {
                            break;  /* No enough room. */
                        } else {
                            if (iUTF8+3 > utf8Len) {
                                result = MT_INVALID_ARGS;
                                break;
                            }

                            mt_uint32 cp = ((mt_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((mt_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                            if (!mt_is_valid_code_point(cp)) {
                                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    result = MT_INVALID_CODE_POINT;
                                    break;
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
                            result = MT_INVALID_CODE_POINT;
                            break;
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

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = iUTF8;
        }
    }

    /* Null terminate. */
    if (utf16Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF16[0] = 0;
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = (utf16CapOriginal - utf16Cap);
    }

    return result;
}

mt_result mt_utf8_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

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

mt_result mt_utf8_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

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
    mt_result result;
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
        while (pUTF8[0] != 0) {
            utf32Len += 1;
            if ((mt_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                            }
                        }
                        pUTF8 += 4;
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MT_INVALID_CODE_POINT;
                            break;
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
        size_t iUTF8;
        for (iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            utf32Len += 1;
            if ((mt_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                iUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }
                        mt_uint32 cp = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!mt_is_valid_code_point(cp)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                            }
                        }

                        iUTF8 += 4;
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MT_INVALID_CODE_POINT;
                            break;
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

    return result;
}

mt_result mt_utf8_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result = MT_SUCCESS;
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
        const mt_utf8* pUTF8Original = pUTF8;
        while (pUTF8[0] != 0) {
            if (utf32Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if ((mt_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[0];
                pUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[1] & 0x3F) << 6) | (pUTF8[2] & 0x3F);
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MT_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((mt_utf32)(pUTF8[0] & 0x07) << 18) | ((mt_utf32)(pUTF8[1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        pUTF8 += 4;

                        if (!mt_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;   /* No characters should be in the UTF-16 surrogate pair range. */
                                break;
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MT_INVALID_CODE_POINT;
                            break;
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

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = (pUTF8 - pUTF8Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF8;
        for (iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (utf32Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if ((mt_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[iUTF8+0];
                iUTF8 += 1;
            } else {
                if (mt_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((mt_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((mt_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            result = MT_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((mt_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((mt_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((mt_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                        iUTF8 += 4;

                        if (!mt_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MT_INVALID_CODE_POINT;
                            break;
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

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = iUTF8;
        }
    }

    /* Null terminate. */
    if (utf32Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF32[0] = 0;
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = (utf32CapOriginal - utf32Cap);
    }

    return result;
}

mt_result mt_utf8_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

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

mt_result mt_utf8_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, mt_uint32 flags)
{
    mt_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = mt_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

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
    mt_result result = MT_SUCCESS;
    size_t utf8Len = 0;
    mt_utf16 w1;
    mt_utf16 w2;
    mt_utf32 utf32;

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF16 == NULL) {
        return MT_INVALID_ARGS; /* Invalid input. */
    }

    if (utf16Len == 0 || pUTF16[0] == 0) {
        return MT_SUCCESS;  /* Empty input string. Length is always 0. */
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
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
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
                    if (iUTF16+1 > utf16Len) {
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
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

    return result;
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


mt_result mt_utf16_to_utf8_internal(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf8CapOriginal = utf8Cap;
    mt_utf16 w1;
    mt_utf16 w2;
    mt_utf32 utf32;
    size_t utf8cpLen;   /* Code point length in UTF-8 code units. */

    if (pUTF8 == NULL) {
        return mt_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, isLE);
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
        const mt_utf16* pUTF16Original = pUTF16;
        while (pUTF16[0] != 0) {
            if (utf8Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

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
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = (pUTF16 - pUTF16Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF16;
        for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
            if (utf8Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

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
                    if (iUTF16+1 > utf16Len) {
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = iUTF16;
        }
    }
    
    /* Null terminate. */
    if (utf8Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF8[0] = 0;
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = (utf8CapOriginal - utf8Cap);
    }

    return result;
}

mt_result mt_utf16ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    } else {
        return mt_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    }
}

mt_result mt_utf16le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    return mt_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MT_TRUE);
}

mt_result mt_utf16be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    return mt_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MT_FALSE);
}

mt_result mt_utf16_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
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
        mt_result result;
        size_t utf16LenProcessed;

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
            result = mt_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = mt_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = utf16LenProcessed + 1;    /* +1 for BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf16ne_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
}


mt_result mt_utf16_to_utf32_length_internal(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf32Len = 0;
    mt_utf16 w1;
    mt_utf16 w2;

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF16 == NULL) {
        return MT_INVALID_ARGS; /* Invalid input. */
    }

    if (utf16Len == 0 || pUTF16[0] == 0) {
        return MT_SUCCESS;  /* Empty input string. Length is always 0. */
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
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[1]);
                        }
                    
                        /* Check for invalid code point. */
                        if (!(w2 >= 0xDC00 && w2 <= 0xDFFF)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    }

                    pUTF16 += 1;
                }
            }

            utf32Len += 1;
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
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 > utf16Len) {
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        /* Check for invalid code point. */
                        if (!(w2 >= 0xDC00 && w2 <= 0xDFFF)) {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    }

                    iUTF16 += 1;
                }
            }

            utf32Len += 1;
        }
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    return result;
}

mt_result mt_utf16ne_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf16le_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    } else {
        return mt_utf16be_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    }
}

mt_result mt_utf16le_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, MT_TRUE);
}

mt_result mt_utf16be_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    return mt_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, MT_FALSE);
}

mt_result mt_utf16_to_utf32_length(size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, mt_uint32 flags)
{
    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
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
            return mt_utf16le_to_utf32_length(pUTF32Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return mt_utf16be_to_utf32_length(pUTF32Len, pUTF16+1, utf16Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf16ne_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
}


mt_result mt_utf16_to_utf32_internal(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf32CapOriginal = utf32Cap;
    mt_utf16 w1;
    mt_utf16 w2;
    mt_utf32 utf32;

    if (pUTF32 == NULL) {
        return mt_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, isLE);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF32 == NULL) {
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
        const mt_utf16* pUTF16Original = pUTF16;
        while (pUTF16[0] != 0) {
            if (utf32Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

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
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            if (isLE) {
                pUTF32[0] = mt_host2le_32(utf32);
            } else {
                pUTF32[0] = mt_host2be_32(utf32);
            }
            
            pUTF32   += 1;
            utf32Cap -= 1;
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = (pUTF16 - pUTF16Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF16;
        for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
            if (utf32Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

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
                    if (iUTF16+1 > utf16Len) {
                        result = MT_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = mt_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = mt_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MT_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MT_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            if (isLE) {
                pUTF32[0] = mt_host2le_32(utf32);
            } else {
                pUTF32[0] = mt_host2be_32(utf32);
            }

            pUTF32   += 1;
            utf32Cap -= 1;
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = iUTF16;
        }
    }
    
    /* Null terminate. */
    if (utf32Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF32[0] = 0;
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = (utf32CapOriginal - utf32Cap);
    }

    return result;
}

mt_result mt_utf16ne_to_utf32ne(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf16le_to_utf32le(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    } else {
        return mt_utf16be_to_utf32be(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    }
}

mt_result mt_utf16le_to_utf32le(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    return mt_utf16_to_utf32_internal(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MT_TRUE);
}

mt_result mt_utf16be_to_utf32be(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    return mt_utf16_to_utf32_internal(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MT_FALSE);
}

mt_result mt_utf16_to_utf32(mt_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const mt_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, mt_uint32 flags)
{
    if (pUTF32 == NULL) {
        return mt_utf16_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF32 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf16_bom((const mt_uint8*)pUTF16, utf16Len)) {
        mt_result result;
        size_t utf16LenProcessed;

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
            result = mt_utf16le_to_utf32le(pUTF32, utf32Cap, pUTF32Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = mt_utf16be_to_utf32be(pUTF32, utf32Cap, pUTF32Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = utf16LenProcessed + 1;    /* +1 for BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf16ne_to_utf32ne(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
}


mt_result mt_utf32_to_utf8_length_internal(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf8Len = 0;
    mt_utf32 utf32;

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF32 == NULL) {
        return MT_INVALID_ARGS; /* Invalid input. */
    }

    if (utf32Len == 0 || pUTF32[0] == 0) {
        return MT_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF32[0] != 0) {
            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[0]);
            } else {
                utf32 = mt_be2host_32(pUTF32[0]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8Len += mt_utf32_cp_to_utf8_length(utf32);
            pUTF32  += 1;
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = mt_be2host_32(pUTF32[iUTF32]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8Len += mt_utf32_cp_to_utf8_length(utf32);
            pUTF32  += 1;
        }
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = utf8Len;
    }

    return result;
}

mt_result mt_utf32ne_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf32le_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
    } else {
        return mt_utf32be_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
    }
}

mt_result mt_utf32le_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    return mt_utf32_to_utf8_length_internal(pUTF8Len, pUTF32, utf32Len, flags, MT_TRUE);
}

mt_result mt_utf32be_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    return mt_utf32_to_utf8_length_internal(pUTF8Len, pUTF32, utf32Len, flags, MT_FALSE);
}

mt_result mt_utf32_to_utf8_length(size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf32_bom_le((const mt_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            return mt_utf32le_to_utf8_length(pUTF8Len, pUTF32+1, utf32Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return mt_utf32be_to_utf8_length(pUTF8Len, pUTF32+1, utf32Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf32ne_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
}


mt_result mt_utf32_to_utf8_internal(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf8CapOriginal = utf8Cap;
    size_t utf8cpLen;   /* Code point length in UTF-8 code units. */
    mt_utf32 utf32;

    if (pUTF8 == NULL) {
        return mt_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, isLE);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        const mt_utf32* pUTF32Original = pUTF32;
        while (pUTF32[0] != 0) {
            if (utf8Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[0]);
            } else {
                utf32 = mt_be2host_32(pUTF32[0]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
            pUTF32  += 1;
        }

        if (pUTF32LenProcessed != NULL) {
            *pUTF32LenProcessed = (pUTF32 - pUTF32Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (utf8Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = mt_le2host_16(pUTF32[iUTF32]);
            } else {
                utf32 = mt_be2host_16(pUTF32[iUTF32]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8cpLen = mt_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            pUTF8   += utf8cpLen;
            utf8Cap -= utf8cpLen;
        }

        if (pUTF32LenProcessed != NULL) {
            *pUTF32LenProcessed = iUTF32;
        }
    }

    /* Null terminate. */
    if (utf8Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF8[0] = 0;
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = (utf8CapOriginal - utf8Cap);
    }

    return result;
}

mt_result mt_utf32ne_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf32le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    } else {
        return mt_utf32be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    }
}

mt_result mt_utf32le_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    return mt_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MT_TRUE);
}

mt_result mt_utf32be_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    return mt_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MT_FALSE);
}

mt_result mt_utf32_to_utf8(mt_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        mt_result result;
        size_t utf32LenProcessed;

        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf32_bom_le((const mt_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            result = mt_utf32le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = mt_utf32be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF32LenProcessed) {
            *pUTF32LenProcessed = utf32LenProcessed + 1;    /* +1 for the BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf32ne_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, pUTF32LenProcessed, flags);
}



mt_result mt_utf32_to_utf16_length_internal(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf16Len = 0;
    mt_utf32 utf32;

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF32 == NULL) {
        return MT_INVALID_ARGS; /* Invalid input. */
    }

    if (utf32Len == 0 || pUTF32[0] == 0) {
        return MT_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        while (pUTF32[0] != 0) {
            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[0]);
            } else {
                utf32 = mt_be2host_32(pUTF32[0]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16Len += mt_utf32_cp_to_utf16_length(utf32);
            pUTF32   += 1;
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = mt_be2host_32(pUTF32[iUTF32]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16Len += mt_utf32_cp_to_utf16_length(utf32);
            pUTF32   += 1;
        }
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    return result;
}

mt_result mt_utf32ne_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf32le_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
    } else {
        return mt_utf32be_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
    }
}

mt_result mt_utf32le_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    return mt_utf32_to_utf16_length_internal(pUTF16Len, pUTF32, utf32Len, flags, MT_TRUE);
}

mt_result mt_utf32be_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    return mt_utf32_to_utf16_length_internal(pUTF16Len, pUTF32, utf32Len, flags, MT_FALSE);
}

mt_result mt_utf32_to_utf16_length(size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, mt_uint32 flags)
{
    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf32_bom_le((const mt_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            return mt_utf32le_to_utf16_length(pUTF16Len, pUTF32+1, utf32Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return mt_utf32be_to_utf16_length(pUTF16Len, pUTF32+1, utf32Len-1, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf32ne_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
}


mt_result mt_utf32_to_utf16_internal(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags, mt_bool32 isLE)
{
    mt_result result = MT_SUCCESS;
    size_t utf16CapOriginal = utf16Cap;
    size_t utf16cpLen;  /* Code point length in UTF-8 code units. */
    mt_utf32 utf32;

    if (pUTF16 == NULL) {
        return mt_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, isLE);
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF16 == NULL) {
        return MT_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        const mt_utf32* pUTF32Original = pUTF32;
        while (pUTF32[0] != 0) {
            if (utf16Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = mt_le2host_32(pUTF32[0]);
            } else {
                utf32 = mt_be2host_32(pUTF32[0]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16cpLen = mt_utf32_cp_to_utf16(utf32, pUTF16, utf16Cap);
            if (utf16cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            if (isLE) {
                if (utf16cpLen == 1) {
                    pUTF16[0] = mt_host2le_16(pUTF16[0]);
                } else {
                    pUTF16[0] = mt_host2le_16(pUTF16[0]);
                    pUTF16[1] = mt_host2le_16(pUTF16[1]);
                }
            } else {
                if (utf16cpLen == 1) {
                    pUTF16[0] = mt_host2be_16(pUTF16[0]);
                } else {
                    pUTF16[0] = mt_host2be_16(pUTF16[0]);
                    pUTF16[1] = mt_host2be_16(pUTF16[1]);
                }
            }

            pUTF16   += utf16cpLen;
            utf16Cap -= utf16cpLen;
            pUTF32   += 1;
        }

        if (pUTF32LenProcessed != NULL) {
            *pUTF32LenProcessed = (pUTF32 - pUTF32Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (utf16Cap == 0) {
                result = MT_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = mt_le2host_16(pUTF32[iUTF32]);
            } else {
                utf32 = mt_be2host_16(pUTF32[iUTF32]);
            }

            if (mt_is_valid_code_point(utf32)) {
                if ((flags & MT_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MT_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MT_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16cpLen = mt_utf32_cp_to_utf16(utf32, pUTF16, utf16Cap);
            if (utf16cpLen == 0) {
                result = MT_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            if (isLE) {
                if (utf16cpLen == 1) {
                    pUTF16[0] = mt_host2le_16(pUTF16[0]);
                } else {
                    pUTF16[0] = mt_host2le_16(pUTF16[0]);
                    pUTF16[1] = mt_host2le_16(pUTF16[1]);
                }
            } else {
                if (utf16cpLen == 1) {
                    pUTF16[0] = mt_host2be_16(pUTF16[0]);
                } else {
                    pUTF16[0] = mt_host2be_16(pUTF16[0]);
                    pUTF16[1] = mt_host2be_16(pUTF16[1]);
                }
            }

            pUTF16   += utf16cpLen;
            utf16Cap -= utf16cpLen;
        }

        if (pUTF32LenProcessed != NULL) {
            *pUTF32LenProcessed = iUTF32;
        }
    }

    /* Null terminate. */
    if (utf16Cap == 0) {
        result = MT_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF16[0] = 0;
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = (utf16CapOriginal - utf16Cap);
    }

    return result;
}

mt_result mt_utf32ne_to_utf16ne(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    if (mt_is_little_endian()) {
        return mt_utf32le_to_utf16le(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    } else {
        return mt_utf32be_to_utf16be(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    }
}

mt_result mt_utf32le_to_utf16le(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    return mt_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MT_TRUE);
}

mt_result mt_utf32be_to_utf16be(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    return mt_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MT_FALSE);
}

mt_result mt_utf32_to_utf16(mt_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const mt_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, mt_uint32 flags)
{
    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    /* Check for BOM. */
    if (mt_has_utf32_bom((const mt_uint8*)pUTF32, utf32Len)) {
        mt_result result;
        size_t utf32LenProcessed;

        mt_bool32 isLE;
        if ((flags & MT_FORBID_BOM) != 0) {
            return MT_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = mt_is_utf32_bom_le((const mt_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            result = mt_utf32le_to_utf16le(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = mt_utf32be_to_utf16be(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MT_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF32LenProcessed) {
            *pUTF32LenProcessed = utf32LenProcessed + 1;    /* +1 for the BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return mt_utf32ne_to_utf16ne(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, pUTF32LenProcessed, flags);
}



/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************


 Platform Specific Section


 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/


/**************************************************************************************************************************************************************

 GDI + Uniscribe

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_GDI)
#include <usp10.h>  /* TODO: Get rid of this dependency. */
#include <math.h>   /* For cosf() and sinf(). May want to move this out of here and into the general implementation section. */

/* msimg32.dll */
typedef BOOL (WINAPI * MT_PFN_AlphaBlend)(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);


void mt_gc_get_size__gdi(mt_gc* pGC, int* pSizeX, int* pSizeY)
{
    MT_ASSERT(pSizeX != NULL);
    MT_ASSERT(pSizeY != NULL);

    /* If it's a bitmap we use the size of that. Otherwise, if it's a window we try that. If it's none of these, we use HORZRES/VERTRES. */
    if (pGC->gdi.hBitmap != NULL) {
        *pSizeX = pGC->gdi.bitmapSizeX;
        *pSizeY = pGC->gdi.bitmapSizeY;
    } else {
        HWND hWnd = WindowFromDC((HDC)pGC->gdi.hDC);
        if (hWnd != NULL) {
            RECT rect;
            GetClientRect(hWnd, &rect);
            *pSizeX = rect.right - rect.left;
            *pSizeY = rect.bottom - rect.top;
        } else {
            /* Fallback to HORZRES/VERTRES. */
            *pSizeX = GetDeviceCaps((HDC)pGC->gdi.hDC, HORZRES);
            *pSizeY = GetDeviceCaps((HDC)pGC->gdi.hDC, VERTRES);
        }
    }
}


/* Font */
mt_result mt_font_init__gdi(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont)
{
    BYTE slantGDI;
    LONG weightGDI;
    LOGFONTA logfont;
    HFONT hFont;

    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pFont != NULL);

    switch (pConfig->weight)
    {
    case mt_font_weight_medium:      weightGDI = FW_MEDIUM;     break;
    case mt_font_weight_thin:        weightGDI = FW_THIN;       break;
    case mt_font_weight_extra_light: weightGDI = FW_EXTRALIGHT; break;
    case mt_font_weight_light:       weightGDI = FW_LIGHT;      break;
    case mt_font_weight_semi_bold:   weightGDI = FW_SEMIBOLD;   break;
    case mt_font_weight_bold:        weightGDI = FW_BOLD;       break;
    case mt_font_weight_extra_bold:  weightGDI = FW_EXTRABOLD;  break;
    case mt_font_weight_heavy:       weightGDI = FW_HEAVY;      break;
    default:                         weightGDI = FW_REGULAR;    break;
    }

	slantGDI = FALSE;
    if (pConfig->slant == mt_font_slant_italic || pConfig->slant == mt_font_slant_oblique) {
        slantGDI = TRUE;
    }

	MT_ZERO_OBJECT(&logfont);
    logfont.lfHeight      = -(LONG)(pConfig->sizeInPixels);
	logfont.lfWeight      = weightGDI;
	logfont.lfItalic      = slantGDI;
	logfont.lfCharSet     = DEFAULT_CHARSET;
    logfont.lfQuality     = (pConfig->noClearType) ? ANTIALIASED_QUALITY : CLEARTYPE_QUALITY;
    logfont.lfEscapement  = 0;
    logfont.lfOrientation = 0;
    mt_strncpy_s(logfont.lfFaceName, sizeof(logfont.lfFaceName), pConfig->family, _TRUNCATE);

    hFont = CreateFontIndirectA(&logfont);
    if (hFont == NULL) {
        return MT_ERROR;    /* Failed to create font. */
    }

    /* Grab the font metrics. */
    SaveDC((HDC)pAPI->gdi.hGlobalDC);
    {
        TEXTMETRICW metrics;

        GetTextMetricsW((HDC)pAPI->gdi.hGlobalDC, &metrics);
        pFont->metrics.ascent     = metrics.tmAscent;
        pFont->metrics.descent    = metrics.tmDescent;
        pFont->metrics.lineHeight = metrics.tmHeight;
    }
    RestoreDC((HDC)pAPI->gdi.hGlobalDC, -1);

    pFont->gdi.hFont = (mt_handle)hFont;

    return MT_SUCCESS;
}

void mt_font_uninit__gdi(mt_font* pFont)
{
    MT_ASSERT(pFont != NULL);

    /* Free the script cache. */
    ScriptFreeCache((SCRIPT_CACHE*)&pFont->gdi.sc);
    pFont->gdi.sc = NULL;   /* Safety. */

    DeleteObject((HGDIOBJ)pFont->gdi.hFont);
}


/* Brush */
mt_result mt_brush_init__gdi(mt_api* pAPI, const mt_brush_config* pConfig, mt_brush* pBrush)
{
    HBRUSH    hBrush     = NULL;
    HBITMAP   hBitmap    = NULL;
    mt_bool32 ownsBitmap = MT_FALSE;

    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pBrush != NULL);

    switch (pConfig->type)
    {
        case mt_brush_type_solid:
        {
            hBrush = (HBRUSH)pAPI->gdi.hStockSolidFillBrush;
        } break;

#if 0
        case mt_brush_type_linear:
        {
            /* TODO: Implement me. */
            return MT_INVALID_OPERATION;
        } break;
#endif

        case mt_brush_type_gc:
        {
            /* If the HDC is writing to a HBITMAP we can retrieve that. Otherwise we need to create a new HBITMAP, BitBlt the source HDC over to it and use that. */
            if (pConfig->gc.pGC == NULL) {
                return MT_INVALID_ARGS; /* GC not set for mt_brush_type_gc. */
            }

            if (pConfig->gc.pGC->gdi.hBitmap != NULL) {
                ownsBitmap = MT_FALSE;
                hBitmap    = (HBITMAP)pConfig->gc.pGC->gdi.hBitmap;
                hBrush     = CreatePatternBrush(hBitmap);
            } else {
                HDC hTempDC;
                int sizeX;
                int sizeY;
                mt_gc_get_size__gdi(pConfig->gc.pGC, &sizeX, &sizeY);

                ownsBitmap = MT_TRUE;

                hBitmap = CreateCompatibleBitmap((HDC)pConfig->gc.pGC->gdi.hDC, sizeX, sizeY);
                if (hBitmap == NULL) {
                    return MT_ERROR;    /* Failed to create bitmap. */
                }

                hTempDC = CreateCompatibleDC((HDC)pConfig->gc.pGC->gdi.hDC);
                if (hTempDC == NULL) {
                    DeleteObject(hBitmap);
                    return MT_ERROR;    /* Failed to create temp DC. */
                }

                SelectObject(hTempDC, hBitmap);
                BitBlt(hTempDC, 0, 0, sizeX, sizeY, (HDC)pConfig->gc.pGC->gdi.hDC, 0, 0, SRCCOPY);

                DeleteDC(hTempDC);
                
                hBrush = CreatePatternBrush(hBitmap);
            }

            if (hBrush == NULL) {
                if (hBitmap != NULL && ownsBitmap) {
                    DeleteObject(hBitmap);
                }
                return MT_ERROR;
            }
        } break;

        default: return MT_INVALID_ARGS;
    }

    pBrush->gdi.hBrush     = hBrush;
    pBrush->gdi.hBitmap    = hBitmap;
    pBrush->gdi.ownsBitmap = ownsBitmap;

    return MT_SUCCESS;
}

void mt_brush_uninit__gdi(mt_brush* pBrush)
{
    MT_ASSERT(pBrush != NULL);

    if (pBrush->gdi.hBrush != pBrush->pAPI->gdi.hStockSolidFillBrush) {
        DeleteObject((HGDIOBJ)pBrush->gdi.hBrush);

        /* Destroy the bitmap after the brush to ensure it's not referenced anymore. */
        if (pBrush->gdi.hBitmap != NULL && pBrush->gdi.ownsBitmap) {
            DeleteObject((HGDIOBJ)pBrush->gdi.hBitmap);
        }
    }
}

void mt_brush_set_origin__gdi(mt_brush* pBrush, mt_int32 x, mt_int32 y)
{
    MT_ASSERT(pBrush != NULL);

    pBrush->gdi.originX = x;
    pBrush->gdi.originY = y;
}


/* Graphics */
mt_result mt_gc_init__gdi(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC)
{
    MT_ASSERT(pAPI != NULL);

    (void)pAPI;

    if (pConfig->gdi.hDC != NULL) {
        pGC->isTransient = MT_TRUE;
        pGC->format = mt_format_unknown;
        pGC->gdi.hDC = pConfig->gdi.hDC;
    } else {
        pGC->isTransient = MT_FALSE;
        pGC->format = pConfig->format;
        pGC->gdi.hDC = (mt_handle)CreateCompatibleDC(NULL);
        
        /* Now we need to create a bitmap that acts as the destination for graphics output. */
        {
            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(bmi));
            bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
            bmi.bmiHeader.biWidth       = (LONG)pConfig->sizeX;
            bmi.bmiHeader.biHeight      = (LONG)pConfig->sizeY;
            bmi.bmiHeader.biPlanes      = 1;
            bmi.bmiHeader.biBitCount    = 32;   /* Only supporting 32-bit formats. */
            bmi.bmiHeader.biCompression = BI_RGB;

            pGC->gdi.hBitmap = (mt_handle)CreateDIBSection((HDC)pGC->gdi.hDC, &bmi, DIB_RGB_COLORS, (void**)&pGC->gdi.pBitmapData, NULL, 0);
            if (pGC->gdi.hBitmap == NULL) {
                DeleteDC((HDC)pGC->gdi.hDC);
                pGC->gdi.hDC = NULL;
                return MT_ERROR;
            }

            if (pConfig->pInitialImageData != NULL) {
                mt_copy_and_flip_image_data_y(pGC->gdi.pBitmapData, pConfig->pInitialImageData, pConfig->sizeX, pConfig->sizeY, 0, mt_format_bgra, pConfig->stride, pConfig->format); /* GDI uses BGRA and a tightly packed stride internally. */
                GdiFlush();
            }

            pGC->gdi.bitmapSizeX = pConfig->sizeX;
            pGC->gdi.bitmapSizeY = pConfig->sizeY;

            SelectObject((HDC)pGC->gdi.hDC, (HGDIOBJ)pGC->gdi.hBitmap);
        }
    }

    SetGraphicsMode((HDC)pGC->gdi.hDC, GM_ADVANCED);    /* <-- Needed for world transforms (rotate and scale). */
    SetStretchBltMode((HDC)pGC->gdi.hDC, COLORONCOLOR); /* <-- COLORONCOLOR is better than the default when the image is scaled down. Not setting this results in black pixelation. */

    /* We need at least one item in the state stack. Unfortunate malloc() here - may want to think about optimizing this. Perhaps some optimized per-API scheme? */
    pGC->gdi.stateCount = 1;
    pGC->gdi.stateCap   = 1;
    pGC->gdi.pState = (mt_gc_state_gdi*)MT_CALLOC(pGC->gdi.stateCap, sizeof(*pGC->gdi.pState));
    if (pGC->gdi.pState == NULL) {
        if (!pGC->isTransient) {
            DeleteDC((HDC)pGC->gdi.hDC);
            DeleteObject((HGDIOBJ)pGC->gdi.hBitmap);
        }
        return MT_OUT_OF_MEMORY;
    }

    return MT_SUCCESS;
}

void mt_gc_uninit__gdi(mt_gc* pGC)
{
    mt_uint32 i;

    MT_ASSERT(pGC != NULL);

    if (!pGC->isTransient) {
        DeleteDC((HDC)pGC->gdi.hDC);
        DeleteObject((HGDIOBJ)pGC->gdi.hBitmap);
    }

    /* The state stack may be holding HPEN objects that need to be deleted. */
    for (i = 0; i < pGC->gdi.stateCount; ++i) {
        if (pGC->gdi.pState[i].hPen != NULL) {
            DeleteObject((HGDIOBJ)pGC->gdi.pState[i].hPen);
            pGC->gdi.pState[i].hPen = NULL;
        }
    }

    MT_FREE(pGC->gdi.pState);
}

mt_result mt_gc_save__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);
    MT_ASSERT(pGC->gdi.stateCount > 0);

    if (SaveDC((HDC)pGC->gdi.hDC) == 0) {
        return MT_ERROR;    /* Failed to save the HDC state. */
    }

    if (pGC->gdi.stateCount == pGC->gdi.stateCap) {
        mt_uint32 newCap = MT_MAX(1, pGC->gdi.stateCap * 2);
        mt_gc_state_gdi* pNewState = (mt_gc_state_gdi*)MT_REALLOC(pGC->gdi.pState, newCap * sizeof(*pNewState));
        if (pNewState == NULL) {
            return MT_OUT_OF_MEMORY;
        }

        pGC->gdi.pState   = pNewState;
        pGC->gdi.stateCap = newCap;
    }

    MT_ASSERT(pGC->gdi.stateCount < pGC->gdi.stateCap);

    pGC->gdi.pState[pGC->gdi.stateCount] = pGC->gdi.pState[pGC->gdi.stateCount-1];
    pGC->gdi.pState[pGC->gdi.stateCount].hPen = NULL;   /* The pen needs to be cleared to NULL and a new copy of the pen created when necessary. The pen will be deleted when the state is restored. */
    pGC->gdi.stateCount += 1;

    return MT_SUCCESS;
}

mt_result mt_gc_restore__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);
    MT_ASSERT(pGC->gdi.stateCount > 0);

    if (pGC->gdi.stateCount == 1) {
        return MT_INVALID_OPERATION;    /* Nothing to restore. */
    }

    pGC->gdi.stateCount -= 1;

    /* If we have a pen that's not the stock pen we need to delete it. */
    if (pGC->gdi.pState[pGC->gdi.stateCount].hPen != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_PEN));
        if (!DeleteObject((HGDIOBJ)pGC->gdi.pState[pGC->gdi.stateCount].hPen)) {
            MT_ASSERT(MT_FALSE);    /* For debugging. Should never happen. */
        }
        pGC->gdi.pState[pGC->gdi.stateCount].hPen = NULL;
    }

    if (RestoreDC((HDC)pGC->gdi.hDC, -1) == 0) {
        return MT_ERROR;    /* Failed to restore the HDC state. */
    }

    return MT_SUCCESS;
}

void mt_gc_translate__gdi(mt_gc* pGC, mt_int32 offsetX, mt_int32 offsetY)
{
    XFORM transform = {0};

    MT_ASSERT(pGC != NULL);

    transform.eM11 = 1;
    transform.eM22 = 1;
    transform.eDx = (float)offsetX;
    transform.eDy = (float)offsetY;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void mt_gc_rotate__gdi(mt_gc* pGC, float rotationInRadians)
{
    XFORM transform = {0};
    float c = cosf(rotationInRadians);
    float s = sinf(rotationInRadians);

    MT_ASSERT(pGC != NULL);

    transform.eM11 =  c;
    transform.eM12 =  s;
    transform.eM21 = -s;
    transform.eM22 =  c;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void mt_gc_scale__gdi(mt_gc* pGC, float scaleX, float scaleY)
{
    XFORM transform = {0};

    MT_ASSERT(pGC != NULL);

    transform.eM11 = scaleX;
    transform.eM22 = scaleY;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void mt_gc_set_miter_limit__gdi(mt_gc* pGC, float limit)
{
    MT_ASSERT(pGC != NULL);
    SetMiterLimit((HDC)pGC->gdi.hDC, limit, NULL);
}

MT_PRIVATE void mt_gc_delete_current_pen__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hPen != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_PEN));
        DeleteObject((HGDIOBJ)pGC->gdi.pState[pGC->gdi.stateCount-1].hPen);
        pGC->gdi.pState[pGC->gdi.stateCount-1].hPen = NULL;
    }
}

MT_PRIVATE void mt_gc_select_current_pen__gdi(mt_gc* pGC)
{
    mt_uint32 iState = pGC->gdi.stateCount-1;

    MT_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[iState].hPen == NULL) {
        HPEN hPen;
        DWORD iPenStyle;
        DWORD cWidth;
        LOGBRUSH lbrush;
        DWORD cStyle;
        DWORD pStyle[16];
        mt_uint32 i;

        iPenStyle = PS_GEOMETRIC;
        if (pGC->gdi.pState[iState].dashCount == 0) {
            iPenStyle |= PS_SOLID;
        } else {
            iPenStyle |= PS_USERSTYLE;
        }

        switch (pGC->gdi.pState[iState].lineCap) {
            case mt_line_cap_square: iPenStyle |= PS_ENDCAP_SQUARE;
            case mt_line_cap_round:  iPenStyle |= PS_ENDCAP_ROUND;
            case mt_line_cap_flat:
            default: iPenStyle |= PS_ENDCAP_FLAT;
        }

        switch (pGC->gdi.pState[iState].lineJoin) {
            case mt_line_join_bevel: iPenStyle |= PS_JOIN_BEVEL;
            case mt_line_join_round: iPenStyle |= PS_JOIN_ROUND;
            case mt_line_join_miter:
            default: iPenStyle |= PS_JOIN_MITER;
        }

        cWidth = pGC->gdi.pState[iState].lineWidth;
        cStyle = pGC->gdi.pState[iState].dashCount;

        for (i = 0; i < pGC->gdi.pState[iState].dashCount; ++i) {
            pStyle[i] = (DWORD)pGC->gdi.pState[iState].dashes[i];
        }

        MT_ZERO_OBJECT(&lbrush);

        if (pGC->gdi.pState[iState].pUserLineBrush == NULL) {
            mt_brush_config* pBrush = &pGC->gdi.pState[iState].lineStockBrush;
            if (pBrush->type == mt_brush_type_solid) {
                if (pBrush->solid.color.a > 0) {
                    lbrush.lbStyle = BS_SOLID;
                    lbrush.lbColor = RGB(pBrush->solid.color.r, pBrush->solid.color.g, pBrush->solid.color.b);
                } else {
                    lbrush.lbStyle = BS_NULL;
                }
            } else if (pBrush->type == mt_brush_type_gc) {
                if (pBrush->gc.pGC != NULL) {
                    lbrush.lbStyle = BS_PATTERN;
                    lbrush.lbHatch = (ULONG_PTR)pBrush->gc.pGC->gdi.hBitmap;
                } else {
                    lbrush.lbStyle = BS_NULL;
                }
            } else {
                return; /* Invalid state (unknown brush type). */
            }
        } else {
            mt_brush* pBrush = pGC->gdi.pState[iState].pUserLineBrush;
            if (pBrush->config.type == mt_brush_type_solid) {
                lbrush.lbStyle = BS_SOLID;
                lbrush.lbColor = RGB(pBrush->config.solid.color.r, pBrush->config.solid.color.g, pBrush->config.solid.color.b);
            } else if (/*pBrush->config.type == mt_brush_type_linear ||*/ pBrush->config.type == mt_brush_type_gc) {
                lbrush.lbStyle = BS_PATTERN;
                lbrush.lbHatch = (ULONG_PTR)pBrush->gdi.hBitmap;
            } else {
                return; /* Invalid state (unknown brush type). */
            }
        }
        
        hPen = ExtCreatePen(iPenStyle, cWidth, &lbrush, cStyle, (cStyle == 0) ? NULL : pStyle);
        if (hPen == NULL) {
            return; /* Failed to create the pen. */
        }

        SelectObject((HDC)pGC->gdi.hDC, hPen);

        pGC->gdi.pState[iState].hPen = (mt_handle)hPen;
    }
}


void mt_gc_set_line_width__gdi(mt_gc* pGC, mt_int32 width)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].lineWidth = width;
    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_cap__gdi(mt_gc* pGC, mt_line_cap cap)
{
    MT_ASSERT(pGC != NULL);
    
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineCap = cap;
    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_join__gdi(mt_gc* pGC, mt_line_join join)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].lineJoin = join;
    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_dash__gdi(mt_gc* pGC, mt_uint32 count, float* dashes)
{
    MT_ASSERT(pGC != NULL);

    if (count == 0 || dashes == NULL) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].dashCount = 0;
    } else {
        pGC->gdi.pState[pGC->gdi.stateCount-1].dashCount = count;
        MT_COPY_MEMORY(pGC->gdi.pState[pGC->gdi.stateCount-1].dashes, dashes, count * sizeof(float));
    }

    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_brush__gdi(mt_gc* pGC, mt_brush* pBrush)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = pBrush;
    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_brush_solid__gdi(mt_gc* pGC, mt_color color)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = NULL;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.type = mt_brush_type_solid;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.solid.color = color;

    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_line_brush_gc__gdi(mt_gc* pGC, mt_gc* pSrcGC)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = NULL;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.type = mt_brush_type_gc;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.gc.pGC = pSrcGC;

    mt_gc_delete_current_pen__gdi(pGC);
}

void mt_gc_set_fill_brush__gdi(mt_gc* pGC, mt_brush* pBrush)
{
    mt_uint32 iState = pGC->gdi.stateCount-1;

    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[iState].pUserFillBrush = pBrush;

    if (pBrush != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, pBrush->gdi.hBrush);
    } else {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_BRUSH));
    }

    if (pGC->gdi.pState[iState].hasTransientFillBrush) {
        mt_brush_uninit(&pGC->gdi.pState[iState].transientFillBrush);
        pGC->gdi.pState[iState].hasTransientFillBrush = MT_FALSE;
    }
}

MT_PRIVATE void mt_gc_set_fill_brush_transient__gdi(mt_gc* pGC, const mt_brush_config* pConfig)
{
    mt_uint32 iState = pGC->gdi.stateCount-1;
    mt_brush prevTransientBrush;
    mt_bool32 uninitPrevTransientBrush = MT_FALSE;

    MT_ASSERT(pGC != NULL);

    /* Need to make a copy of the previous transient fill brush so we can uninitialize later. */
    if (pGC->gdi.pState[iState].hasTransientFillBrush) {
        prevTransientBrush = pGC->gdi.pState[iState].transientFillBrush;
        uninitPrevTransientBrush = MT_TRUE;
    }

    if (mt_brush_init(pGC->pAPI, pConfig, &pGC->gdi.pState[iState].transientFillBrush) != MT_SUCCESS) {
        return; /* Failed to initialize transient fill brush. */
    }

    pGC->gdi.pState[iState].hasTransientFillBrush = MT_TRUE;

    SelectObject((HDC)pGC->gdi.hDC, pGC->gdi.pState[iState].transientFillBrush.gdi.hBrush);

    if (pGC->gdi.pState[iState].transientFillBrush.config.type == mt_brush_type_solid) {
        SetDCBrushColor((HDC)pGC->gdi.hDC, RGB(pConfig->solid.color.r, pConfig->solid.color.g, pConfig->solid.color.b));
    } else if (pGC->gdi.pState[iState].transientFillBrush.config.type == mt_brush_type_gc) {
        /* TODO: Do something. */
    }

    pGC->gdi.pState[iState].pUserFillBrush = NULL;

    /* We can now uninitialize the previous brush since it's no longer selected on the GDI side. */
    if (uninitPrevTransientBrush) {
        mt_brush_uninit(&prevTransientBrush);
    }
}

void mt_gc_set_fill_brush_solid__gdi(mt_gc* pGC, mt_color color)
{
    mt_brush_config config;
    config.type = mt_brush_type_solid;
    config.solid.color = color;
    mt_gc_set_fill_brush_transient__gdi(pGC, &config);
}

void mt_gc_set_fill_brush_gc__gdi(mt_gc* pGC, mt_gc* pSrcGC)
{
    mt_brush_config config;
    config.type = mt_brush_type_gc;
    config.gc.pGC = pSrcGC;
    mt_gc_set_fill_brush_transient__gdi(pGC, &config);
}

void mt_gc_set_font__gdi(mt_gc* pGC, mt_font* pFont)
{
    MT_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].pFont != pFont) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].pFont  = pFont;
        SelectObject((HDC)pGC->gdi.hDC, (HFONT)pFont->gdi.hFont);
    }
}

void mt_gc_set_text_fg_color__gdi(mt_gc* pGC, mt_color bgColor)
{
    MT_ASSERT(pGC != NULL);

    SetTextColor((HDC)pGC->gdi.hDC, RGB(bgColor.r, bgColor.g, bgColor.b));
}

void mt_gc_set_text_bg_color__gdi(mt_gc* pGC, mt_color bgColor)
{
    MT_ASSERT(pGC != NULL);

    if (bgColor.a == 0) {
        SetBkMode((HDC)pGC->gdi.hDC, TRANSPARENT);
    } else {
        SetBkMode((HDC)pGC->gdi.hDC, OPAQUE);
        SetBkColor((HDC)pGC->gdi.hDC, RGB(bgColor.r, bgColor.g, bgColor.b));
    }
}

void mt_gc_set_blend_op__gdi(mt_gc* pGC, mt_blend_op op)
{
    MT_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp = op;
}

void mt_gc_set_antialias_mode__gdi(mt_gc* pGC, mt_antialias_mode mode)
{
    MT_ASSERT(pGC != NULL);

    /* The anti-aliasing mode is always mt_antialias_mode_none with GDI since it doesn't support anti-aliasing. */
    (void)pGC;
    (void)mode;
}

void mt_gc_set_stretch_filter__gdi(mt_gc* pGC, mt_stretch_filter filter)
{
    MT_ASSERT(pGC != NULL);

    if (filter == mt_stretch_filter_linear) {
        SetStretchBltMode((HDC)pGC->gdi.hDC, HALFTONE);
        SetBrushOrgEx((HDC)pGC->gdi.hDC, 0, 0, NULL);       /* <-- Calling this because the documentation tells me to after SetStretchBltMode() with HALFTONE. I have no idea why on Earth this is necessary... */
    } else {
        SetStretchBltMode((HDC)pGC->gdi.hDC, COLORONCOLOR);
    }
}


MT_PRIVATE void mt_gc_begin_path_if_required__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun == MT_FALSE) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun = MT_TRUE;
        BeginPath((HDC)pGC->gdi.hDC);
    }
}

MT_PRIVATE void mt_gc_end_path__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun == MT_TRUE) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun = MT_FALSE;
        EndPath((HDC)pGC->gdi.hDC);
    }
}

void mt_gc_move_to__gdi(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    MT_ASSERT(pGC != NULL);

    mt_gc_begin_path_if_required__gdi(pGC);
    MoveToEx((HDC)pGC->gdi.hDC, x, y, NULL);
}

void mt_gc_line_to__gdi(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    MT_ASSERT(pGC != NULL);

    mt_gc_begin_path_if_required__gdi(pGC);
    LineTo((HDC)pGC->gdi.hDC, x, y);
}

void mt_gc_rectangle__gdi(mt_gc* pGC, mt_int32 left, mt_int32 top, mt_int32 right, mt_int32 bottom)
{
    MT_ASSERT(pGC != NULL);

    mt_gc_begin_path_if_required__gdi(pGC);
    Rectangle((HDC)pGC->gdi.hDC, left, top, right, bottom);
}

void mt_gc_arc__gdi(mt_gc* pGC, mt_int32 x, mt_int32 y, mt_int32 radius, float angle1InRadians, float angle2InRadians)
{
    MT_ASSERT(pGC != NULL);

    mt_gc_begin_path_if_required__gdi(pGC);
    AngleArc((HDC)pGC->gdi.hDC, x, y, (DWORD)radius, angle1InRadians, angle2InRadians);
}

void mt_gc_curve_to__gdi(mt_gc* pGC, mt_int32 x1, mt_int32 y1, mt_int32 x2, mt_int32 y2, mt_int32 x3, mt_int32 y3)
{
    POINT pt[3];

    MT_ASSERT(pGC != NULL);

    pt[0].x = x1;
    pt[0].y = y1;
    pt[1].x = x2;
    pt[1].y = y2;
    pt[2].x = x3;
    pt[2].y = y3;

    mt_gc_begin_path_if_required__gdi(pGC);
    PolyBezierTo((HDC)pGC->gdi.hDC, pt, 3);
}

void mt_gc_close_path__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    CloseFigure((HDC)pGC->gdi.hDC);
}

void mt_gc_clip__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    mt_gc_end_path__gdi(pGC);
    SelectClipPath((HDC)pGC->gdi.hDC, RGN_AND);
}

void mt_gc_reset_clip__gdi(mt_gc* pGC)
{
    MT_ASSERT(pGC != NULL);

    SelectClipRgn((HDC)pGC->gdi.hDC, NULL);
}

mt_bool32 mt_gc_is_point_inside_clip__gdi(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    MT_ASSERT(pGC != NULL);

    return PtVisible((HDC)pGC->gdi.hDC, x, y);
}

void mt_gc_fill__gdi(mt_gc* pGC)
{
    mt_uint32 iState;
    mt_bool32 adjustBrushOrigin = MT_FALSE;
    mt_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin;

    MT_ASSERT(pGC != NULL);

    mt_gc_end_path__gdi(pGC);

    iState = pGC->gdi.stateCount-1;

    pBrushToUseForOrigin = pGC->gdi.pState[iState].pUserFillBrush;
    if (pBrushToUseForOrigin != NULL) {
        if (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0) {
            adjustBrushOrigin = MT_TRUE;
            SetBrushOrgEx((HDC)pGC->gdi.hDC, pBrushToUseForOrigin->gdi.originX, pBrushToUseForOrigin->gdi.originY, &prevBrushOrigin);
        }
    }

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp == mt_blend_op_src) {
        FillPath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        FillPath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void mt_gc_stroke__gdi(mt_gc* pGC)
{
    mt_uint32 iState;
    mt_bool32 adjustBrushOrigin = MT_FALSE;
    mt_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin;

    MT_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    
    mt_gc_end_path__gdi(pGC);
    mt_gc_select_current_pen__gdi(pGC);

    pBrushToUseForOrigin = pGC->gdi.pState[iState].pUserLineBrush;
    if (pBrushToUseForOrigin != NULL) {
        if (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0) {
            adjustBrushOrigin = MT_TRUE;
            SetBrushOrgEx((HDC)pGC->gdi.hDC, pBrushToUseForOrigin->gdi.originX, pBrushToUseForOrigin->gdi.originY, &prevBrushOrigin);
        }
    }

    if (pGC->gdi.pState[iState].blendOp == mt_blend_op_src) {
        StrokePath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        StrokePath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void mt_gc_fill_and_stroke__gdi(mt_gc* pGC)
{
    mt_uint32 iState;
    mt_bool32 adjustBrushOrigin = MT_FALSE;
    mt_brush* pUserLineBrush;
    mt_brush* pUserFillBrush;
    mt_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin;

    MT_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;

    mt_gc_end_path__gdi(pGC);
    mt_gc_select_current_pen__gdi(pGC);

    pUserLineBrush = pGC->gdi.pState[iState].pUserLineBrush;
    pUserFillBrush = pGC->gdi.pState[iState].pUserFillBrush;

    pBrushToUseForOrigin = pUserFillBrush;
    if (pBrushToUseForOrigin == NULL) {
        pBrushToUseForOrigin = pUserLineBrush;
    } else {
        if (pBrushToUseForOrigin->config.type == mt_brush_type_solid) {
            pBrushToUseForOrigin = pUserLineBrush;
        }
    }

    if (pBrushToUseForOrigin != NULL && (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0)) {
        adjustBrushOrigin = MT_TRUE;
        SetBrushOrgEx((HDC)pGC->gdi.hDC, pBrushToUseForOrigin->gdi.originX, pBrushToUseForOrigin->gdi.originY, &prevBrushOrigin);
    }


    /*
    BUG: If the line and fill brushes are both GC brushes and they have different origins, the origin of the fill brush will
         be used for _both_ the stroke and the fill. The reason for this is that SetBrushOrgEx() is a global API, and there's
         no way that I'm aware of to set this separately for pens and brushes with StrokeAndFillPath().

         An idea may be to separate out the stroke and fill by calling FillPath(), followed by StrokePath(), however this has
         two problems:
           1) This will cause flickering in single-buffered environments; and
           2) When FillPath() is called, the path is discarded which means it would need to be recreated again before calling
              StrokePath(). This would require a call to GetPath() which may actually work OK.
         
         The current implementation will work correctly in the following cases:
           - When only one or none of either the line brush or the fill brush is a GC-type brush and have a non-0 origin.
           - When both the line brush and fill brush use the same origin.
    */

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp == mt_blend_op_src) {
        StrokeAndFillPath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        StrokeAndFillPath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void mt_gc_draw_gc__gdi(mt_gc* pGC, mt_gc* pSrcGC, mt_int32 srcX, mt_int32 srcY)
{
    HDC hDstDC;
    HDC hSrcDC;
    int srcSizeX;
    int srcSizeY;

    hDstDC = (HDC)pGC->gdi.hDC;
    hSrcDC = (HDC)pSrcGC->gdi.hDC;

    mt_gc_get_size__gdi(pSrcGC, &srcSizeX, &srcSizeY);

    /* If the image has an alpha channel we need to use AlphaBlend(). */
    if (pSrcGC->gdi.hBitmap != NULL && mt_format_has_alpha(pSrcGC->format)) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ((MT_PFN_AlphaBlend)pGC->pAPI->gdi.AlphaBlend)(hDstDC, 0, 0, srcSizeX, srcSizeY, hSrcDC, srcX, srcY, srcSizeX, srcSizeY, blend);
    } else {
        StretchBlt(hDstDC, 0, 0, srcSizeX, srcSizeY, hSrcDC, srcX, srcX, srcSizeX, srcSizeY, SRCCOPY);
    }
}

#pragma comment(lib, "usp10.lib")  /* TODO: Remove this and replace with runtime linking. */

/* API */
mt_result mt_itemize_utf16__gdi(mt_api* pAPI, const mt_utf16* pTextUTF16, size_t textLength, mt_item* pItems, mt_uint32* pItemCount)
{
    HRESULT hResult;
    mt_uint32 itemCapacity;
    int scriptItemCount = 0;
    MT_SCRIPT_ITEM  pScriptItemsStack[1024];
    MT_SCRIPT_ITEM* pScriptItemsHeap = NULL;
    MT_SCRIPT_ITEM* pScriptItems = NULL;   /* Set to either pScriptItemsStack or pScriptItemsHeap. */

    MT_ASSERT(pAPI != NULL);
    MT_ASSERT(pTextUTF16 != NULL);
    MT_ASSERT(pTextUTF16[0] != '\0');
    MT_ASSERT(textLength > 0);
    MT_ASSERT(pItems != NULL);
    MT_ASSERT(pItemCount != NULL);

    /* ScriptItemize() uses an integer for the text length, so keep it simple and return an error if we're asking for more. */
    if (textLength > 0x7FFFFFFF) {
        return MT_INVALID_ARGS;
    }

    itemCapacity = *pItemCount;

    /*
    Unfortunately ScriptItemize() is kind of bad when it comes to counting the number of items in the string. From the documentation:
    
        If the function returns E_OUTOFMEMORY, the application can call it again with a larger pItems buffer.

    This means that the way to do this is to repeatedly call ScriptItemize() with ever increasing buffer sizes. We first try using a
    stack-allocated buffer. If this is too small we will use the heap.
    */

    /* Try itemizing into the stack first. If this fails we fall back to the heap. */
    hResult = ScriptItemize((const wchar_t*)pTextUTF16, (int)textLength, MT_COUNTOF(pScriptItemsStack)-1, NULL, NULL, (SCRIPT_ITEM*)pScriptItemsStack, &scriptItemCount);   /* Subtract 1 from item capacity because the last item is always used as a null terminator. */
    if (hResult == S_OK) {
        pScriptItems = &pScriptItemsStack[0];
    } else {
        if (hResult == E_OUTOFMEMORY) {
            /* Not enough room in the stack so try the heap. We just continuously increase the size of the buffer until it's big enough. */
            size_t heapCap = MT_COUNTOF(pScriptItemsStack);
            while (hResult == E_OUTOFMEMORY) {
                MT_SCRIPT_ITEM* pNewScriptItemsHeap;

                heapCap *= 2;
                if (heapCap > 0x7FFFFFFF) {
                    MT_FREE(pScriptItemsHeap);
                    return MT_INVALID_ARGS; /* String is too long for Uniscribe. */
                }

                pNewScriptItemsHeap = (MT_SCRIPT_ITEM*)MT_REALLOC(pScriptItemsHeap, sizeof(*pNewScriptItemsHeap) * heapCap);
                if (pNewScriptItemsHeap == NULL) {
                    MT_FREE(pScriptItemsHeap);
                    return MT_OUT_OF_MEMORY;
                }

                pScriptItemsHeap = pNewScriptItemsHeap;

                hResult = ScriptItemize((const wchar_t*)pTextUTF16, (int)textLength, heapCap-1, NULL, NULL, (SCRIPT_ITEM*)pScriptItemsHeap, &scriptItemCount);   /* Subtract 1 from item capacity because the last item is always used as a null terminator. */
            }

            pScriptItems = pScriptItemsHeap;
        }
    }

    if (hResult != S_OK) {
        return mt_result_from_HRESULT(hResult); /* Something bad happened. */
    }


    /*
    Make sure the item count is always set, even in the event of future errors. This ensures the count is available in the event that the input capacity is too small in which
    case the caller can resize their input buffer accordingly.
    */
    *pItemCount = (mt_uint32)scriptItemCount;


    /* At this point we have our SCRIPT_ITEM objects, so now we need to convert the SCRIPT_ITEM objects to mt_item objects. */
    if (pItems != NULL) {
        if (itemCapacity >= (mt_uint32)scriptItemCount) {
            int iItem;
            for (iItem = 0; iItem < scriptItemCount; ++iItem) {
                pItems[iItem].offset         = (size_t)pScriptItems[iItem].iCharPos;
                pItems[iItem].length         = (size_t)(pScriptItems[iItem+1].iCharPos - pScriptItems[iItem].iCharPos);
                pItems[iItem].backend.gdi.sa = pScriptItems[iItem].a;
            }
        } else {
            MT_FREE(pScriptItemsHeap);
            return MT_NO_SPACE; /* Not enough room*/
        }
    }

    MT_FREE(pScriptItemsHeap);
    return MT_SUCCESS;
}

mt_result mt_shape_utf16__gdi(mt_font* pFont, mt_item* pItem, const mt_utf16* pTextUTF16, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters)
{
    HRESULT hResult;
    HDC hDC = NULL;
    int glyphCount;
    size_t outputGlyphCap;
    void* pGlyphsAndSVAHeap = NULL;                 /* Single heap allocation for both pUniscribeGlyphsHeap and pUniscribeClustersHeap. */
    WORD  pUniscribeGlyphsStack[4096];
    WORD* pUniscribeGlyphsHeap = NULL;              /* Offset of pGlyphsAndSVAHeap. */
    WORD* pUniscribeGlyphs;
    WORD  pUniscribeClustersStack[4096];
    WORD* pUniscribeClustersHeap = NULL;
    WORD* pUniscribeClusters = NULL;
    MT_SCRIPT_VISATTR  pUniscribeSVAStack[4096];
    MT_SCRIPT_VISATTR* pUniscribeSVAHeap = NULL;    /* Offset of pGlyphsAndSVAHeap. */
    MT_SCRIPT_VISATTR* pUniscribeSVA = NULL;

    MT_ASSERT(pItem != NULL);
    MT_ASSERT(pFont != NULL);
    MT_ASSERT(pTextUTF16 != NULL);
    MT_ASSERT(pGlyphCount != NULL);

    /* ScriptItemize() uses an integer for the text length, so keep it simple and return an error if we're asking for more. */
    if (textLength > 0x7FFFFFFF) {
        return MT_INVALID_ARGS;
    }
    if (*pGlyphCount > 0x7FFFFFFF) {
        return MT_INVALID_ARGS;
    }

    outputGlyphCap = *pGlyphCount;

    /* We need the same number of clusters as textLength. */
    if (textLength <= MT_COUNTOF(pUniscribeClustersStack)) {
        pUniscribeClusters = &pUniscribeClustersStack[0];
    } else {
        pUniscribeClustersHeap = (WORD*)MT_MALLOC(sizeof(*pUniscribeClustersHeap) * textLength);
        if (pUniscribeClustersHeap == NULL) {
            return MT_OUT_OF_MEMORY;
        }

        pUniscribeClusters = pUniscribeClustersHeap;
    }

    /* We first try with a NULL DC. If this fails we need to select the font into the global DC and try again. */
    hResult = ScriptShape(NULL, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, MT_COUNTOF(pUniscribeGlyphsStack), (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsStack, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAStack, &glyphCount);
    if (hResult == E_PENDING) {
        /* Select the font into the global DC and try again, this time with the DC set to the global DC. */
        hDC = (HDC)pFont->pAPI->gdi.hGlobalDC;
        SelectObject(hDC, (HGDIOBJ)pFont->gdi.hFont);
        hResult = ScriptShape(hDC, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, MT_COUNTOF(pUniscribeGlyphsStack), (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsStack, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAStack, &glyphCount);
    }

    if (hResult == S_OK) {
        pUniscribeGlyphs = &pUniscribeGlyphsStack[0];
        pUniscribeSVA = &pUniscribeSVAStack[0];
    } else {
        if (hResult == E_OUTOFMEMORY) {
            /* Not enough room in the stack, so try the heap. We need to continuously call ScriptShape() with ever increasing buffer sizes until it works. */
            size_t heapCap = MT_COUNTOF(pUniscribeGlyphsStack);
            while (hResult == E_OUTOFMEMORY) {
                void* pNewGlyphsAndSVAHeap;

                heapCap *= 2;
                if (heapCap > 0x7FFFFFFF) {
                    MT_FREE(pUniscribeClustersHeap);
                    MT_FREE(pGlyphsAndSVAHeap);
                    return MT_INVALID_ARGS; /* String is too long for Uniscribe. */
                }

                pNewGlyphsAndSVAHeap = MT_REALLOC(pGlyphsAndSVAHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap)) * heapCap);
                if (pNewGlyphsAndSVAHeap == NULL) {
                    MT_FREE(pUniscribeClustersHeap);
                    MT_FREE(pGlyphsAndSVAHeap);
                    return MT_OUT_OF_MEMORY;
                }

                pGlyphsAndSVAHeap = pNewGlyphsAndSVAHeap;

                pUniscribeGlyphsHeap = (WORD*)pGlyphsAndSVAHeap;
                pUniscribeSVAHeap = (MT_SCRIPT_VISATTR*)MT_OFFSET_PTR(pGlyphsAndSVAHeap, sizeof(*pUniscribeGlyphsHeap) * heapCap);

                hResult = ScriptShape(hDC, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, (int)heapCap, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsHeap, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAHeap, &glyphCount);
            }

            pUniscribeGlyphs = pUniscribeGlyphsHeap;
            pUniscribeSVA = pUniscribeSVAHeap;
        } else {
            MT_FREE(pUniscribeClustersHeap);
            return mt_result_from_HRESULT(hResult);
        }
    }
    
    /* At this point we have the glyphs in Uniscribe format, so now we need to convert that to minitype format. */
    *pGlyphCount = glyphCount;  /* <-- Always make sure this is set to that the client can know how many glyphs are required in the event that MT_NO_SPACE is returned. */

    /* The output array can be null in which case we just ignore it. */
    if (pGlyphs != NULL && (size_t)glyphCount <= outputGlyphCap) {
        size_t iGlyph;
        for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
            pGlyphs[iGlyph].backend.gdi.sv = pUniscribeSVA[iGlyph];
            pGlyphs[iGlyph].index = pUniscribeGlyphs[iGlyph];
            pGlyphs[iGlyph].advance = 0;
        }
    }

    if (pClusters != NULL) {
        size_t iCluster;
        for (iCluster = 0; iCluster < textLength; ++iCluster) {
            pClusters[iCluster] = pUniscribeClusters[iCluster];
        }
    }


    MT_FREE(pUniscribeClustersHeap);
    MT_FREE(pGlyphsAndSVAHeap);

    /* Make sure MT_NO_SPACE is returned in the event that the output glyph count exceeds the capacity of the output buffer. */
    if ((size_t)glyphCount > outputGlyphCap) {
        return MT_NO_SPACE;
    } else {
        return MT_SUCCESS;
    }
}

mt_result mt_place__gdi(mt_font* pFont, mt_item* pItem, mt_glyph* pGlyphs, size_t glyphCount, mt_text_metrics* pRunMetrics)
{
    HRESULT hResult;
    ABC abc;
    size_t iGlyph;
    void* pHeap = NULL;
    WORD  pUniscribeGlyphsStack[4096];
    WORD* pUniscribeGlyphsHeap = NULL;              /* Offset of pHead. */
    WORD* pUniscribeGlyphs;
    MT_SCRIPT_VISATTR  pUniscribeSVAStack[4096];
    MT_SCRIPT_VISATTR* pUniscribeSVAHeap = NULL;    /* Offset of pHeap. */
    MT_SCRIPT_VISATTR* pUniscribeSVA = NULL;
    int  pUniscribeAdvancesStack[4096];
    int* pUniscribeAdvancesHeap = NULL;             /* Offset of pHeap. */
    int* pUniscribeAdvances = NULL;
    GOFFSET  pUniscribeOffsetsStack[4096];
    GOFFSET* pUniscribeOffsetsHeap = NULL;          /* Offset of pHeap. */
    GOFFSET* pUniscribeOffsets = NULL;

    /* ScriptItemize() uses an integer for the glyph count, so keep it simple and return an error if we're asking for more. */
    if (glyphCount > 0x7FFFFFFF) {
        return MT_INVALID_ARGS;
    }

    if (glyphCount <= MT_COUNTOF(pUniscribeAdvancesStack)) {
        /* Stack allocation. */
        pUniscribeGlyphs   = &pUniscribeGlyphsStack[0];
        pUniscribeSVA      = &pUniscribeSVAStack[0];
        pUniscribeAdvances = &pUniscribeAdvancesStack[0];
        pUniscribeOffsets  = &pUniscribeOffsetsStack[0];
    } else {
        /* Heap allocation. */
        pHeap = MT_MALLOC((sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap)+sizeof(*pUniscribeAdvancesHeap)+sizeof(*pUniscribeOffsetsHeap)) * glyphCount);
        if (pHeap == NULL) {
            return MT_OUT_OF_MEMORY;
        }

        pUniscribeGlyphs   =              (WORD*)MT_OFFSET_PTR(pHeap, 0);
        pUniscribeSVA      = (MT_SCRIPT_VISATTR*)MT_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap))                                                            * glyphCount);
        pUniscribeAdvances =               (int*)MT_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap))                                 * glyphCount);
        pUniscribeOffsets  =           (GOFFSET*)MT_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap)+sizeof(*pUniscribeAdvancesHeap)) * glyphCount);
    }

    for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
        pUniscribeGlyphs[iGlyph] = (WORD)pGlyphs[iGlyph].index;
        pUniscribeSVA[iGlyph] = pGlyphs[iGlyph].backend.gdi.sv;
    }

    hResult = ScriptPlace(NULL, (SCRIPT_CACHE*)&pFont->gdi.sc, pUniscribeGlyphs, (int)glyphCount, (const SCRIPT_VISATTR*)pUniscribeSVA, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeAdvances, pUniscribeOffsets, &abc);
    if (hResult == E_PENDING) {
        /* Select the font into the global DC and try again, this time with the DC set to the global DC. */
        SelectObject((HDC)pFont->pAPI->gdi.hGlobalDC, (HGDIOBJ)pFont->gdi.hFont);
        hResult = ScriptPlace((HDC)pFont->pAPI->gdi.hGlobalDC, (SCRIPT_CACHE*)&pFont->gdi.sc, pUniscribeGlyphs, (int)glyphCount, (const SCRIPT_VISATTR*)pUniscribeSVA, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeAdvances, pUniscribeOffsets, &abc);
    }

    if (hResult != S_OK) {
        MT_FREE(pHeap);
        return mt_result_from_HRESULT(hResult);
    }

    /* At this point we have the glyphs in Uniscribe format, so now we need to convert that to minitype format. */
    if (pRunMetrics != NULL) {
        pRunMetrics->lPadding = abc.abcA;
        pRunMetrics->rPadding = abc.abcC;
        pRunMetrics->sizeX = abc.abcA + abc.abcB + abc.abcC;
        pRunMetrics->sizeY = pFont->metrics.lineHeight;
    }

    if (pGlyphs != NULL) {
        for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
            pGlyphs[iGlyph].advance = pUniscribeAdvances[iGlyph];
            pGlyphs[iGlyph].backend.gdi.offsetX = pUniscribeOffsets[iGlyph].du;
            pGlyphs[iGlyph].backend.gdi.offsetY = pUniscribeOffsets[iGlyph].dv;
        }
    }

    MT_FREE(pHeap);
    return MT_SUCCESS;
}

void mt_gc_draw_glyphs__gdi(mt_gc* pGC, const mt_item* pItem, const mt_glyph* pGlyphs, size_t glyphCount, mt_int32 x, mt_int32 y)
{
    HRESULT hResult;
    size_t iGlyph;
    mt_font* pFont;
    void* pHeap = NULL;
    WORD  pUniscribeGlyphsStack[4096];
    WORD* pUniscribeGlyphsHeap = NULL;              /* Offset of pHead. */
    WORD* pUniscribeGlyphs;
    int  pUniscribeAdvancesStack[4096];
    int* pUniscribeAdvancesHeap = NULL;             /* Offset of pHeap. */
    int* pUniscribeAdvances = NULL;
    GOFFSET  pUniscribeOffsetsStack[4096];
    GOFFSET* pUniscribeOffsetsHeap = NULL;          /* Offset of pHeap. */
    GOFFSET* pUniscribeOffsets = NULL;

    /* ScriptItemize() uses an integer for the glyph count, so keep it simple and return an error if we're asking for more. */
    if (glyphCount > 0x7FFFFFFF) {
        return; /* MT_INVALID_ARGS */
    }

    pFont = pGC->gdi.pState[pGC->gdi.stateCount-1].pFont;
    if (pFont == NULL) {
        return; /* MT_INVALID_OPERATION. No font selected. */
    }

    if (glyphCount <= MT_COUNTOF(pUniscribeAdvancesStack)) {
        /* Stack allocation. */
        pUniscribeGlyphs   = &pUniscribeGlyphsStack[0];
        pUniscribeAdvances = &pUniscribeAdvancesStack[0];
        pUniscribeOffsets  = &pUniscribeOffsetsStack[0];
    } else {
        /* Heap allocation. */
        pHeap = MT_MALLOC((sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeAdvancesHeap)+sizeof(*pUniscribeOffsetsHeap)) * glyphCount);
        if (pHeap == NULL) {
            return; /* MT_OUT_OF_MEMORY */
        }

        pUniscribeGlyphs   =    (WORD*)MT_OFFSET_PTR(pHeap, 0);
        pUniscribeAdvances =     (int*)MT_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap))                                 * glyphCount);
        pUniscribeOffsets  = (GOFFSET*)MT_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeAdvancesHeap)) * glyphCount);
    }

    for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
        pUniscribeGlyphs[iGlyph]     = (WORD)pGlyphs[iGlyph].index;
        pUniscribeAdvances[iGlyph]   = pGlyphs[iGlyph].advance;
        pUniscribeOffsets[iGlyph].du = pGlyphs[iGlyph].backend.gdi.offsetX;
        pUniscribeOffsets[iGlyph].dv = pGlyphs[iGlyph].backend.gdi.offsetY;
    }

    hResult = ScriptTextOut((HDC)pGC->gdi.hDC, &pFont->gdi.sc, x, y, /* fuOptions */ETO_OPAQUE, NULL, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, NULL, 0, pUniscribeGlyphs, (int)glyphCount, pUniscribeAdvances, NULL, pUniscribeOffsets);
    if (hResult != S_OK) {
        return; /* Error occurred. */
    }
}


void mt_uninit__gdi(mt_api* pAPI)
{
    MT_ASSERT(pAPI != NULL);

    if (pAPI->gdi.ownsGlobalDC) {
        DeleteDC((HDC)pAPI->gdi.hGlobalDC);
    }

    mt_dlclose(pAPI->gdi.hMsimg32DLL);
    mt_dlclose(pAPI->gdi.hGdi32DLL);
    mt_dlclose(pAPI->gdi.hUsp10DLL);
}

mt_result mt_init__gdi(const mt_api_config* pConfig, mt_api* pAPI)
{
    MT_ASSERT(pConfig != NULL);
    MT_ASSERT(pAPI != NULL);

    /* usp10.dll. Always do this before gdi32.dll. */
    pAPI->gdi.hUsp10DLL = mt_dlopen("usp10.dll");
    if (pAPI->gdi.hUsp10DLL == NULL) {
        return MT_ERROR;    /* Failed to load usp10.dll. */
    }


    /* gdi32.dll */
    pAPI->gdi.hGdi32DLL = mt_dlopen("gdi32.dll");
    if (pAPI->gdi.hGdi32DLL == NULL) {
        mt_dlclose(pAPI->gdi.hUsp10DLL);
        return MT_ERROR;    /* Failed to load gdi32.dll. */
    }


    /* msimg32.dll */
    pAPI->gdi.hMsimg32DLL = mt_dlopen("msimg32.dll");
    if (pAPI->gdi.hMsimg32DLL == NULL) {
        mt_dlclose(pAPI->gdi.hGdi32DLL);
        mt_dlclose(pAPI->gdi.hUsp10DLL);
        return MT_ERROR;
    }

    pAPI->gdi.AlphaBlend = mt_dlsym(pAPI->gdi.hMsimg32DLL, "AlphaBlend");


    /* We need a DC to use for creating a font and performing glyph shaping and placement. */
    if (pConfig->gdi.hDC != NULL) {
        pAPI->gdi.ownsGlobalDC = MT_FALSE;
        pAPI->gdi.hGlobalDC = pConfig->gdi.hDC;
    } else {
        pAPI->gdi.ownsGlobalDC = MT_TRUE;
        pAPI->gdi.hGlobalDC = CreateCompatibleDC(NULL);
        if (pAPI->gdi.hGlobalDC == NULL) {
            mt_dlclose(pAPI->gdi.hMsimg32DLL);
            mt_dlclose(pAPI->gdi.hGdi32DLL);
            mt_dlclose(pAPI->gdi.hUsp10DLL);
            return MT_ERROR;    /* Failed to create global DC. */
        }
    }


    /* Stock objects. */
    pAPI->gdi.hStockSolidFillBrush = GetStockObject(DC_BRUSH);    /* The brush to use for solid brushes. */

    pAPI->uninit              = mt_uninit__gdi;
    pAPI->itemizeUTF8         = NULL;   /* No native support for UTF-8 with Uniscribe. */
    pAPI->itemizeUTF16        = mt_itemize_utf16__gdi;
    pAPI->itemizeUTF32        = NULL;   /* No native support for UTF-32 with Uniscribe. */
    pAPI->shapeUTF8           = NULL;   /* No native support for UTF-8 with Uniscribe. */
    pAPI->shapeUTF16          = mt_shape_utf16__gdi;
    pAPI->shapeUTF32          = NULL;   /* No native support for UTF-32 with Uniscribe. */
    pAPI->place               = mt_place__gdi;
    pAPI->fontInit            = mt_font_init__gdi;
    pAPI->fontUninit          = mt_font_uninit__gdi;
    pAPI->brushInit           = mt_brush_init__gdi;
    pAPI->brushUninit         = mt_brush_uninit__gdi;
    pAPI->brushSetOrigin      = mt_brush_set_origin__gdi;
    pAPI->gcInit              = mt_gc_init__gdi;
    pAPI->gcUninit            = mt_gc_uninit__gdi;
    pAPI->gcSave              = mt_gc_save__gdi;
    pAPI->gcRestore           = mt_gc_restore__gdi;
    pAPI->gcTranslate         = mt_gc_translate__gdi;
    pAPI->gcRotate            = mt_gc_rotate__gdi;
    pAPI->gcScale             = mt_gc_scale__gdi;
    pAPI->gcSetLineWidth      = mt_gc_set_line_width__gdi;
    pAPI->gcSetLineCap        = mt_gc_set_line_cap__gdi;
    pAPI->gcSetLineJoin       = mt_gc_set_line_join__gdi;
    pAPI->gcSetMiterLimit     = mt_gc_set_miter_limit__gdi;
    pAPI->gcSetLineDash       = mt_gc_set_line_dash__gdi;
    pAPI->gcSetLineBrush      = mt_gc_set_line_brush__gdi;
    pAPI->gcSetLineBrushSolid = mt_gc_set_line_brush_solid__gdi;
    pAPI->gcSetLineBrushGC    = mt_gc_set_line_brush_gc__gdi;
    pAPI->gcSetFillBrush      = mt_gc_set_fill_brush__gdi;
    pAPI->gcSetFillBrushSolid = mt_gc_set_fill_brush_solid__gdi;
    pAPI->gcSetFillBrushGC    = mt_gc_set_fill_brush_gc__gdi;
    pAPI->gcSetFont           = mt_gc_set_font__gdi;
    pAPI->gcSetTextFGColor    = mt_gc_set_text_fg_color__gdi;
    pAPI->gcSetTextBGColor    = mt_gc_set_text_bg_color__gdi;
    pAPI->gcSetBlendOp        = mt_gc_set_blend_op__gdi;
    pAPI->gcSetAntialiasMode  = mt_gc_set_antialias_mode__gdi;
    pAPI->gcSetStretchFilter  = mt_gc_set_stretch_filter__gdi;
    pAPI->gcMoveTo            = mt_gc_move_to__gdi;
    pAPI->gcLineTo            = mt_gc_line_to__gdi;
    pAPI->gcRectangle         = mt_gc_rectangle__gdi;
    pAPI->gcArc               = mt_gc_arc__gdi;
    pAPI->gcCurveTo           = mt_gc_curve_to__gdi;
    pAPI->gcClosePath         = mt_gc_close_path__gdi;
    pAPI->gcClip              = mt_gc_clip__gdi;
    pAPI->gcResetClip         = mt_gc_reset_clip__gdi;
    pAPI->gcIsPointInsideClip = mt_gc_is_point_inside_clip__gdi;
    pAPI->gcFill              = mt_gc_fill__gdi;
    pAPI->gcStroke            = mt_gc_stroke__gdi;
    pAPI->gcFillAndStroke     = mt_gc_fill_and_stroke__gdi;
    pAPI->gcDrawGC            = mt_gc_draw_gc__gdi;
    pAPI->gcDrawGlyphs        = mt_gc_draw_glyphs__gdi;

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

 Xft

 **************************************************************************************************************************************************************/
#if defined(MT_HAS_XFT)
#include <X11/Xft/Xft.h>

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

mt_result mt_itemize_utf8(mt_api* pAPI, const mt_utf8* pTextUTF8, size_t textLength, mt_item* pItems, mt_uint32* pItemCount)
{
    mt_result result;
    mt_uint32 itemCount;

    if (pItemCount != NULL) {
        *pItemCount = 0;    /* Safety. */
    }

    if (pAPI == NULL || pTextUTF8 == NULL || textLength == 0 || pItemCount == NULL) {
        return MT_INVALID_ARGS;
    }

    /*
    We could do some generic itemization here such as line breaks and tabs, however it makes things awkward with memory management. I'm therefore
    leaving this to each of the backends.
    */

    /* If the backend directly supports UTF-8, just pass it straight through. Otherwise we need to convert. */
    if (pAPI->itemizeUTF8) {
        /* UTF-8 natively supported. */
        result = pAPI->itemizeUTF8(pAPI, pTextUTF8, textLength, pItems, &itemCount);
    } else {
        /* UTF-8 not natively supported. Need to convert the string to another format. */
        if (pAPI->itemizeUTF16) {
            /*
            Convert the input string to UTF-16, itemize, then convert offsets back to UTF-8 equivalents. To make things more efficient for small
            strings we can try converting to a stack-allocated buffer first. If this is too small we will fall back to the heap which will be
            slightly less efficient.
            */
            size_t utf16Len;
            mt_utf16  pUTF16Stack[4096];
            mt_utf16* pUTF16Heap = NULL;
            mt_utf16* pUTF16 = NULL;    /* Will be set to either pUTF16Stack or pUTF16Heap. */

            result = mt_utf8_to_utf16ne(pUTF16Stack, MT_COUNTOF(pUTF16Stack), &utf16Len, pTextUTF8, textLength, NULL, MT_ERROR_ON_INVALID_CODE_POINT);
            if (result == MT_SUCCESS) {
                pUTF16 = &pUTF16Stack[0];
            } else {
                /* Fall back to the heap. */
                result = mt_utf8_to_utf16ne_length(&utf16Len, pTextUTF8, textLength, 0);
                if (result == MT_SUCCESS) {
                    pUTF16Heap = (mt_utf16*)MT_MALLOC(sizeof(*pUTF16Heap) * (utf16Len+1));
                    if (pUTF16Heap != NULL) {
                        result = mt_utf8_to_utf16ne(pUTF16Heap, utf16Len+1, &utf16Len, pTextUTF8, textLength, NULL, MT_ERROR_ON_INVALID_CODE_POINT);
                        if (result == MT_SUCCESS) {
                            pUTF16 = pUTF16Heap;
                        }
                    } else {
                        result = MT_OUT_OF_MEMORY;
                    }
                }

                if (result != MT_SUCCESS) {
                    return result;  /* An error occurred when converting to UTF-16. */
                }
            }

            /* We have the UTF-16 string, so now we need to itemize these. */
            result = pAPI->itemizeUTF16(pAPI, pUTF16, utf16Len, pItems, &itemCount);
            if (result == MT_SUCCESS) {
                /* We have the items, so now we need to convert the offsets and lengths to the UTF-8 equivalents. */
                if (pItems != NULL) {
                    size_t runningOffset = 0;
                    mt_uint32 iItem;
                    for (iItem = 0; iItem < itemCount; ++iItem) {
                        size_t itemLengthUTF8;
                        result = mt_utf16ne_to_utf8_length(&itemLengthUTF8, pUTF16 + pItems[iItem].offset, pItems[iItem].length, 0);
                        if (result != MT_SUCCESS) {
                            break;  /* Conversion error. */
                        }

                        pItems[iItem].offset = runningOffset;
                        pItems[iItem].length = itemLengthUTF8;
                        runningOffset += itemLengthUTF8;
                    }
                }
            }
            
            /* Done. */
            MT_FREE(pUTF16Heap);
        } else if (pAPI->itemizeUTF32) {
            /* TODO: Convert the input string to UTF-32, itemize, then convert offsets back to UTF-8 equivalents. */
            return MT_INVALID_OPERATION;
        } else {
            return MT_INVALID_OPERATION;    /* Itemization not supported by the backend. */
        }
    }

    *pItemCount = itemCount;
    return result;
}

mt_result mt_shape_utf8(mt_font* pFont, mt_item* pItem, const mt_utf8* pTextUTF8, size_t textLength, mt_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters)
{
    mt_result result;

    if (pFont == NULL || pItem == NULL || pTextUTF8 == NULL || pGlyphCount == NULL) {
        return MT_INVALID_ARGS;
    }

    /* If the backend directly supports UTF-8, just pass it straight through. Otherwise we need to convert. */
    if (pFont->pAPI->shapeUTF8) {
        result = pFont->pAPI->shapeUTF8(pFont, pItem, pTextUTF8, textLength, pGlyphs, pGlyphCount, pClusters);
    } else {
        if (pFont->pAPI->shapeUTF16) {
            /* Convert to UTF-16. We can try using the stack first, and then fall back to a heap-allocation if necessary. */
            size_t    utf16Len;
            void*     pHeap = NULL;
            mt_utf16  pUTF16Stack[4096];
            mt_utf16* pUTF16Heap = NULL;
            mt_utf16* pUTF16 = NULL;    /* Will be set to either pUTF16Stack or pUTF16Heap. */
            size_t    pUTF16ClustersStack[4096];
            size_t*   pUTF16ClustersHeap = NULL;
            size_t*   pUTF16Clusters = NULL;

            result = mt_utf8_to_utf16ne(pUTF16Stack, MT_COUNTOF(pUTF16Stack), &utf16Len, pTextUTF8, textLength, NULL, MT_ERROR_ON_INVALID_CODE_POINT);
            if (result == MT_SUCCESS) {
                pUTF16         = &pUTF16Stack[0];
                pUTF16Clusters = &pUTF16ClustersStack[0];
            } else {
                /* Fall back to the heap. */
                result = mt_utf8_to_utf16ne_length(&utf16Len, pTextUTF8, textLength, 0);
                if (result == MT_SUCCESS) {
                    pHeap = MT_MALLOC((sizeof(*pUTF16Heap)+sizeof(*pUTF16ClustersHeap)) * (utf16Len+1));
                    if (pHeap != NULL) {
                        pUTF16Heap         = (mt_utf16*)pHeap;
                        pUTF16ClustersHeap = (size_t*)MT_OFFSET_PTR(pHeap, sizeof(*pUTF16Heap) * (utf16Len+1));

                        result = mt_utf8_to_utf16ne(pUTF16Heap, utf16Len+1, &utf16Len, pTextUTF8, textLength, NULL, MT_ERROR_ON_INVALID_CODE_POINT);
                        if (result == MT_SUCCESS) {
                            pUTF16         = pUTF16Heap;
                            pUTF16Clusters = pUTF16ClustersHeap;
                        } else {
                            MT_FREE(pHeap);
                            pHeap = NULL;   /* Safety. */
                        }
                    } else {
                        result = MT_OUT_OF_MEMORY;
                    }
                }

                if (result != MT_SUCCESS) {
                    return result;  /* An error occurred when converting to UTF-16. */
                }
            }

            MT_ASSERT(utf16Len <= textLength);

            /* We have the UTF-16 string, so now we perform shaping. */
            result = pFont->pAPI->shapeUTF16(pFont, pItem, pUTF16, utf16Len, pGlyphs, pGlyphCount, pUTF16Clusters);
            if (result == MT_SUCCESS) {
                /* The values in pClusters need to be expanded so that they map to the original UTF-8 string. */
                if (pClusters != NULL) {
                    mt_utf32 utf32;
                    size_t utf16Count;
                    size_t utf8Count;
                    size_t iClusterUTF8;
                    size_t iUTF8 = 0;
                    size_t iUTF16;
                    for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
                        /* First check if the UTF-16 code unit indicates a surrogate pair. If so, we need to consume the next UTF-16 code unit before we can determine the length. */
                        if (pUTF16[iUTF16] < 0xD800 || pUTF16[iUTF16] > 0xDFFF) {
                            /* 1 UTF-16 code unit. */
                            utf32 = pUTF16[iUTF16];
                            utf16Count = 1;
                        } else {
                            /* 2 UTF-16 code units. Errors will have been handled in the conversion step above, so no need to handle that here. */
                            utf32 = mt_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                            utf16Count = 2;
                        }

                        utf8Count = mt_utf32_cp_to_utf8_length(utf32);
                        for (iClusterUTF8 = 0; iClusterUTF8 < utf8Count; ++iClusterUTF8) {
                            pClusters[iUTF8] = pUTF16Clusters[iUTF16];
                            iUTF8 += 1;
                        }

                        iUTF16 += utf16Count;
                    }
                }
            }

            MT_FREE(pHeap);
        } else if (pFont->pAPI->shapeUTF32) {
            /* TODO: Implement me. */
            return MT_INVALID_OPERATION;
        } else {
            return MT_INVALID_OPERATION;    /* Shaping not supported by the backend. */
        }
    }

    return result;
}

mt_result mt_place(mt_font* pFont, mt_item* pItem, mt_glyph* pGlyphs, size_t glyphCount, mt_text_metrics* pRunMetrics)
{
    if (pRunMetrics != NULL) {
        pRunMetrics->sizeX = 0;
        pRunMetrics->sizeY = 0;
    }

    if (pFont == NULL || pItem == NULL || pGlyphs == NULL) {
        return MT_INVALID_ARGS;
    }

    if (pFont->pAPI->place == NULL) {
        return MT_INVALID_OPERATION;    /* Placement not supported by the backend. */
    }

    return pFont->pAPI->place(pFont, pItem, pGlyphs, glyphCount, pRunMetrics);
}

mt_result mt_measure_x(mt_api* pAPI, mt_item* pItem, size_t glyphCount, const mt_glyph* pGlyphs, mt_int32* pX)
{
    size_t iGlyph;
    mt_int32 runningX;

    if (pAPI == NULL || pItem == NULL || pGlyphs == NULL) {
        return MT_SUCCESS;
    }

    runningX = 0;
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        runningX += pGlyphs[iGlyph].advance;
    }

    if (pX != NULL) {
        *pX = runningX;
    }

    return MT_SUCCESS;
}

mt_result mt_x_to_cp(mt_api* pAPI, mt_item* pItem, mt_int32 x, size_t textLength, const size_t* pClusters, size_t glyphCount, const mt_glyph* pGlyphs, size_t* piCP, mt_int32* pOffsetToEdge)
{
    /* Naive implementation for now while I figure things out. */

    size_t iGlyph;
    size_t iCluster;
    mt_int32 runningX;

    if (piCP != NULL) {
        *piCP = (size_t)0;
    }
    if (pOffsetToEdge != NULL) {
        *pOffsetToEdge = 0;
    }

    /* If x is negative we need to clamp it to edge of the run. */
    if (x < 0) {
        return MT_SUCCESS;
    }

    if (pAPI == NULL || pItem == NULL || pClusters == NULL || pGlyphs == NULL) {
        return MT_INVALID_ARGS;
    }

    runningX = 0;
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        mt_int32 nextRunningX = runningX + pGlyphs[iGlyph].advance;
        if (x >= runningX && x < nextRunningX) {
            /* It's somewhere on this glyph. */
            mt_bool32 found = MT_FALSE;
            for (iCluster = 0; iCluster < textLength; ++iCluster) {
                if (pClusters[iCluster] == iGlyph) {
                    if (piCP != NULL) {
                        *piCP = iCluster;
                    }
                        
                    found = MT_TRUE;
                }
            }

            if (pOffsetToEdge != NULL) {
                *pOffsetToEdge = runningX - x;
            }

            return MT_SUCCESS;
        }

        runningX = nextRunningX;
    }

    /* Geting here means the point is beyond the string. We just place it at the end. */
    if (piCP != NULL) {
        *piCP = textLength;
    }
    if (pOffsetToEdge != NULL) {
        *pOffsetToEdge = 0;
    }

    return MT_SUCCESS;
}

mt_result mt_cp_to_x(mt_api* pAPI, mt_item* pItem, size_t iCP, size_t textLength, const size_t* pClusters, size_t glyphCount, const mt_glyph* pGlyphs, mt_int32* pX)
{
    /* Naive implementation for now while I figure things out. */

    size_t iGlyph;
    mt_int32 runningX;

    if (pX != NULL) {
        *pX = 0;
    }

    if (pAPI == NULL || pClusters == NULL || pGlyphs == NULL) {
        return MT_INVALID_ARGS;
    }

    if (glyphCount == 0) {
        return MT_SUCCESS;
    }

    /* Normalize the index if it's out of range. */
    if (iCP > textLength) {
        iCP = textLength;
    }

    /* Special case when the code unit is beyond the length of the text. In this case we set pX to end of the run. */
    runningX = 0;
    if (iCP == textLength) {
        size_t iTargetGlyph = pClusters[iCP];
        if (iTargetGlyph > 0) {
            for (iGlyph = 0; iGlyph < iTargetGlyph; ++iTargetGlyph) {
                runningX += pGlyphs[iGlyph].advance;
            }
        }

        if (pX != NULL) {
            *pX = runningX;
        }
    } else {
        /* Go to the end. */
        MT_ASSERT(iCP == textLength);
        return mt_measure_x(pAPI, pItem, glyphCount, pGlyphs, pX);
    }

    return MT_SUCCESS;
}


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
mt_result mt_font_init(mt_api* pAPI, const mt_font_config* pConfig, mt_font* pFont)
{
    mt_result result;

    if (pFont == NULL) {
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

    if (pFont->pAPI->fontUninit) {
        pFont->pAPI->fontUninit(pFont);
    }
    
    MT_ZERO_OBJECT(pFont);  /* Safety. */
}


/**************************************************************************************************************************************************************

Graphics

**************************************************************************************************************************************************************/
mt_result mt_brush_init(mt_api* pAPI, const mt_brush_config* pConfig, mt_brush* pBrush)
{
    mt_result result;

    if (pBrush == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pBrush);

    if (pAPI == NULL || pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    pBrush->pAPI = pAPI;
    pBrush->config = *pConfig;

    if (pAPI->brushInit) {
        result = pAPI->brushInit(pAPI, pConfig, pBrush);
        if (result != MT_SUCCESS) {
            return result;
        }
    }

    return MT_SUCCESS;
}

void mt_brush_uninit(mt_brush* pBrush)
{
    if (pBrush == NULL) {
        return;
    }

    MT_ASSERT(pBrush->pAPI != NULL);

    if (pBrush->pAPI->brushUninit) {
        pBrush->pAPI->brushUninit(pBrush);
    }

    MT_ZERO_OBJECT(pBrush);  /* Safety. */
}

void mt_brush_set_origin(mt_brush* pBrush, mt_int32 x, mt_int32 y)
{
    if (pBrush == NULL) {
        return;
    }

    MT_ASSERT(pBrush->pAPI != NULL);

    if (pBrush->pAPI->brushSetOrigin) {
        pBrush->pAPI->brushSetOrigin(pBrush, x, y);
    }
}

mt_result mt_gc_init(mt_api* pAPI, const mt_gc_config* pConfig, mt_gc* pGC)
{
    mt_result result;

    if (pGC == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pGC);

    if (pAPI == NULL || pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    pGC->pAPI = pAPI;

    if (pAPI->gcInit) {
        result = pAPI->gcInit(pAPI, pConfig, pGC);
        if (result != MT_SUCCESS) {
            return result;
        }
    }

    return MT_SUCCESS;
}

void mt_gc_uninit(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcUninit) {
        pGC->pAPI->gcUninit(pGC);
    }
}

mt_result mt_gc_save(mt_gc* pGC)
{
    if (pGC == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSave) {
        return pGC->pAPI->gcSave(pGC);
    }
    
    return MT_SUCCESS;
}

mt_result mt_gc_restore(mt_gc* pGC)
{
    if (pGC == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcRestore) {
        return pGC->pAPI->gcRestore(pGC);
    }
    
    return MT_SUCCESS;
}

void mt_gc_translate(mt_gc* pGC, mt_int32 offsetX, mt_int32 offsetY)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcTranslate) {
        pGC->pAPI->gcTranslate(pGC, offsetX, offsetY);
    }
}

void mt_gc_rotate(mt_gc* pGC, float rotationInRadians)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcRotate) {
        pGC->pAPI->gcRotate(pGC, rotationInRadians);
    }
}

void mt_gc_scale(mt_gc* pGC, float scaleX, float scaleY)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcScale) {
        pGC->pAPI->gcScale(pGC, scaleX, scaleY);
    }
}

void mt_gc_set_miter_limit(mt_gc* pGC, float limit)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetMiterLimit) {
        pGC->pAPI->gcSetMiterLimit(pGC, limit);
    }
}

void mt_gc_set_line_width(mt_gc* pGC, mt_int32 width)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineWidth) {
        pGC->pAPI->gcSetLineWidth(pGC, width);
    }
}

void mt_gc_set_line_cap(mt_gc* pGC, mt_line_cap cap)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineCap) {
        pGC->pAPI->gcSetLineCap(pGC, cap);
    }
}

void mt_gc_set_line_join(mt_gc* pGC, mt_line_join join)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineJoin) {
        pGC->pAPI->gcSetLineJoin(pGC, join);
    }
}

void mt_gc_set_line_dash(mt_gc* pGC, mt_uint32 count, float* dashes)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (count > 16) {
        return; /* Count cannot be more than 16. */
    }

    if (pGC->pAPI->gcSetLineDash) {
        pGC->pAPI->gcSetLineDash(pGC, count, dashes);
    }
}

void mt_gc_set_line_brush(mt_gc* pGC, mt_brush* pBrush)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineBrush) {
        pGC->pAPI->gcSetLineBrush(pGC, pBrush);
    }
}

void mt_gc_set_line_brush_solid(mt_gc* pGC, mt_color color)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineBrushSolid) {
        pGC->pAPI->gcSetLineBrushSolid(pGC, color);
    }
}

void mt_gc_set_line_brush_gc(mt_gc* pGC, mt_gc* pSrcGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetLineBrushGC) {
        pGC->pAPI->gcSetLineBrushGC(pGC, pSrcGC);
    }
}

void mt_gc_set_fill_brush(mt_gc* pGC, mt_brush* pBrush)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetFillBrush) {
        pGC->pAPI->gcSetFillBrush(pGC, pBrush);
    }
}

void mt_gc_set_fill_brush_solid(mt_gc* pGC, mt_color color)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetFillBrushSolid) {
        pGC->pAPI->gcSetFillBrushSolid(pGC, color);
    }
}

void mt_gc_set_fill_brush_gc(mt_gc* pGC, mt_gc* pSrcGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetFillBrushGC) {
        pGC->pAPI->gcSetFillBrushGC(pGC, pSrcGC);
    }
}

void mt_gc_set_font(mt_gc* pGC, mt_font* pFont)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetFont) {
        pGC->pAPI->gcSetFont(pGC, pFont);
    }
}

void mt_gc_set_text_fg_color(mt_gc* pGC, mt_color fgColor)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetTextFGColor) {
        pGC->pAPI->gcSetTextFGColor(pGC, fgColor);
    }
}

void mt_gc_set_text_bg_color(mt_gc* pGC, mt_color bgColor)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetTextBGColor) {
        pGC->pAPI->gcSetTextBGColor(pGC, bgColor);
    }
}

void mt_gc_set_blend_op(mt_gc* pGC, mt_blend_op op)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetBlendOp) {
        pGC->pAPI->gcSetBlendOp(pGC, op);
    }
}

void mt_gc_set_antialias_mode(mt_gc* pGC, mt_antialias_mode mode)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetAntialiasMode) {
        pGC->pAPI->gcSetAntialiasMode(pGC, mode);
    }
}

void mt_gc_set_stretch_filter(mt_gc* pGC, mt_stretch_filter filter)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcSetStretchFilter) {
        pGC->pAPI->gcSetStretchFilter(pGC, filter);
    }
}

void mt_gc_move_to(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcMoveTo) {
        pGC->pAPI->gcMoveTo(pGC, x, y);
    }
}

void mt_gc_line_to(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcLineTo) {
        pGC->pAPI->gcLineTo(pGC, x, y);
    }
}

void mt_gc_rectangle(mt_gc* pGC, mt_int32 left, mt_int32 top, mt_int32 right, mt_int32 bottom)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcRectangle) {
        pGC->pAPI->gcRectangle(pGC, left, top, right, bottom);
    }
}

void mt_gc_arc(mt_gc* pGC, mt_int32 x, mt_int32 y, mt_int32 radius, float angle1InRadians, float angle2InRadians)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcArc) {
        pGC->pAPI->gcArc(pGC, x, y, radius, angle1InRadians, angle2InRadians);
    }
}

void mt_gc_curve_to(mt_gc* pGC, mt_int32 x1, mt_int32 y1, mt_int32 x2, mt_int32 y2, mt_int32 x3, mt_int32 y3)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcCurveTo) {
        pGC->pAPI->gcCurveTo(pGC, x1, y1, x2, y2, x3, y3);
    }
}

void mt_gc_close_path(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcClosePath) {
        pGC->pAPI->gcClosePath(pGC);
    }
}

void mt_gc_clip(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcClip) {
        pGC->pAPI->gcClip(pGC);
    }
}

void mt_gc_reset_clip(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcResetClip) {
        pGC->pAPI->gcResetClip(pGC);
    }
}

mt_bool32 mt_gc_is_point_inside_clip(mt_gc* pGC, mt_int32 x, mt_int32 y)
{
    if (pGC == NULL) {
        return MT_FALSE;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcIsPointInsideClip) {
        return pGC->pAPI->gcIsPointInsideClip(pGC, x, y);
    }

    return MT_FALSE;
}

void mt_gc_fill(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcFill) {
        pGC->pAPI->gcFill(pGC);
    }
}

void mt_gc_stroke(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcStroke) {
        pGC->pAPI->gcStroke(pGC);
    }
}

void mt_gc_fill_and_stroke(mt_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->gcFillAndStroke) {
        pGC->pAPI->gcFillAndStroke(pGC);
    }
}

void mt_gc_draw_gc(mt_gc* pGC, mt_gc* pSrcGC, mt_int32 srcX, mt_int32 srcY)
{
    if (pGC == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (pSrcGC == NULL) {
        return; /* Nothing to draw. */
    }

    if (pGC->pAPI->gcDrawGC) {
        pGC->pAPI->gcDrawGC(pGC, pSrcGC, srcX, srcY);
    }
}

void mt_gc_draw_glyphs(mt_gc* pGC, const mt_item* pItem, const mt_glyph* pGlyphs, size_t glyphCount, mt_int32 x, mt_int32 y)
{
    if (pGC == NULL || pItem == NULL || pGlyphs == NULL) {
        return;
    }

    MT_ASSERT(pGC->pAPI != NULL);

    if (glyphCount == 0) {
        return; /* Nothing to draw. */
    }

    if (pGC->pAPI->gcDrawGlyphs) {
        pGC->pAPI->gcDrawGlyphs(pGC, pItem, pGlyphs, glyphCount, x, y);
    }
}



void mt_copy_image_data__no_conversion(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride, mt_format format)
{
    mt_uint32 bpp = mt_get_bytes_per_pixel(format);

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    if (dstStride == srcStride && dstStride == sizeX) {
        MT_COPY_MEMORY(pDst, pSrc, sizeX*sizeY*bpp);
    } else {
        mt_uint32 x;
        mt_uint32 y;

        /* Optimized path for 4 bytes per pixel. Can move everything per-pixel rather than per-byte. */
        if (bpp == 4) {
                  mt_uint32* pDstRow =       (mt_uint32*)pDst;
            const mt_uint32* pSrcRow = (const mt_uint32*)pSrc;

            for (y = 0; y < sizeY; ++y) {
                for (x = 0; x < sizeX; ++x) {
                    pDstRow[x] = pSrcRow[x];
                }

                pDstRow += dstStride;
                pSrcRow += srcStride;
            }
        } else {
                  mt_uint8* pDstRow =       (mt_uint8*)pDst;
            const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

            for (y = 0; y < sizeY; ++y) {
                for (x = 0; x < sizeX; ++x) {
                    mt_uint32 b;
                    for (b = 0; b < bpp; ++b) {
                        pDstRow[x*bpp + b] = pSrcRow[x*bpp + b];
                    }
                }

                pDstRow += dstStride*bpp;
                pSrcRow += srcStride*bpp;
            }
        }
    }
}

void mt_copy_image_data__rgba_to_rgb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 0];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 2];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__rgba_to_bgra(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 2];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 0];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 3];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__rgba_to_bgr(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 2];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 0];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__rgba_to_argb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 3];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 0];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 2];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}


void mt_copy_image_data__rgb_to_rgba(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*3 + 0];
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 2];
            pDstRow[x*4 + 3] = 0xFF;
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__rgb_to_bgra(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*3 + 2];
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 0];
            pDstRow[x*4 + 3] = 0xFF;
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__rgb_to_bgr(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*3 + 2];
            pDstRow[x*3 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*3 + 0];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__rgb_to_argb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = 0xFF;
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 0];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 3] = pSrcRow[x*3 + 2];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}


void mt_copy_image_data__bgra_to_rgba(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 2];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 0];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 3];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__bgra_to_rgb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 2];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 0];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__bgra_to_bgr(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 0];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 2];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__bgra_to_argb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 3];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 2];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 0];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}


void mt_copy_image_data__bgr_to_rgba(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*3 + 2];
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 0];
            pDstRow[x*4 + 3] = 0xFF;
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__bgr_to_rgb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*3 + 2];
            pDstRow[x*3 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*3 + 2] = pSrcRow[x*3 + 0];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__bgr_to_bgra(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*3 + 0];
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 2];
            pDstRow[x*4 + 3] = 0xFF;
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}

void mt_copy_image_data__bgr_to_argb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = 0xFF;
            pDstRow[x*4 + 1] = pSrcRow[x*3 + 2];
            pDstRow[x*4 + 2] = pSrcRow[x*3 + 1];
            pDstRow[x*4 + 3] = pSrcRow[x*3 + 0];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*3;
    }
}


void mt_copy_image_data__argb_to_rgba(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 2];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 3];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 0];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__argb_to_rgb(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 1];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 2];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 3];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__argb_to_bgra(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 0] = pSrcRow[x*4 + 3];
            pDstRow[x*4 + 1] = pSrcRow[x*4 + 2];
            pDstRow[x*4 + 2] = pSrcRow[x*4 + 1];
            pDstRow[x*4 + 3] = pSrcRow[x*4 + 0];
        }

        pDstRow += dstStride*4;
        pSrcRow += srcStride*4;
    }
}

void mt_copy_image_data__argb_to_bgr(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_uint32 srcStride)
{
    mt_uint32 x;
    mt_uint32 y;
    mt_uint8* pDstRow = (mt_uint8*)pDst;
    const mt_uint8* pSrcRow = (const mt_uint8*)pSrc;

    MT_ASSERT(pDst != NULL);
    MT_ASSERT(pSrc != NULL);

    for (y = 0; y < sizeY; ++y) {
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*3 + 0] = pSrcRow[x*4 + 3];
            pDstRow[x*3 + 1] = pSrcRow[x*4 + 2];
            pDstRow[x*3 + 2] = pSrcRow[x*4 + 1];
        }

        pDstRow += dstStride*3;
        pSrcRow += srcStride*4;
    }
}



void mt_copy_image_data(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_format dstFormat, mt_uint32 srcStride, mt_format srcFormat)
{
    if (pDst == NULL || pSrc == NULL) {
        return;
    }

    /* Normalize the strides to simplify things further down. */
    if (dstStride == 0) {
        dstStride = sizeX;
    }
    if (srcStride == 0) {
        srcStride = sizeX;
    }

    if (dstFormat == srcFormat) {
        mt_copy_image_data__no_conversion(pDst, pSrc, sizeX, sizeY, dstStride, srcStride, dstFormat);   /* Simple case. No conversion, just a copy. */
    } else {
        switch (dstFormat)
        {
            case mt_format_rgba:
            {
                if (srcFormat == mt_format_rgb) {
                    mt_copy_image_data__rgb_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgra) {
                    mt_copy_image_data__bgra_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgr) {
                    mt_copy_image_data__bgr_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_argb) {
                    mt_copy_image_data__argb_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case mt_format_rgb:
            {
                if (srcFormat == mt_format_rgba) {
                    mt_copy_image_data__rgba_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgra) {
                    mt_copy_image_data__bgra_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgr) {
                    mt_copy_image_data__bgr_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_argb) {
                    mt_copy_image_data__argb_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case mt_format_bgra:
            {
                if (srcFormat == mt_format_rgba) {
                    mt_copy_image_data__rgba_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_rgb) {
                    mt_copy_image_data__rgb_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgr) {
                    mt_copy_image_data__bgr_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_argb) {
                    mt_copy_image_data__argb_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case mt_format_bgr:
            {
                if (srcFormat == mt_format_rgba) {
                    mt_copy_image_data__rgba_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_rgb) {
                    mt_copy_image_data__rgb_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgra) {
                    mt_copy_image_data__bgra_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_argb) {
                    mt_copy_image_data__argb_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case mt_format_argb:
            {
                if (srcFormat == mt_format_rgba) {
                    mt_copy_image_data__rgba_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_rgb) {
                    mt_copy_image_data__rgb_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgra) {
                    mt_copy_image_data__bgra_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == mt_format_bgr) {
                    mt_copy_image_data__bgr_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case mt_format_unknown:
            default:
            {
            } break;
        }
    }
}

void mt_flip_image_data_y(void* pDst, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 stride, mt_format format)
{
    mt_uint32 bpp;

    if (pDst == NULL) {
        return;
    }

    if (stride == 0) {
        stride = sizeX;
    }

    bpp = mt_get_bytes_per_pixel(format);
    
    /*if (bpp == 4) {
        
    } else*/ {
        /* Generic naive implementation. */
        mt_uint32 iRow;
        mt_uint32 iCol;
        mt_uint32 halfSizeY = sizeY >> 1;
        mt_uint32 strideInBytes = stride * bpp;

        for (iRow = 0; iRow < halfSizeY; ++iRow) {
            mt_uint8* pUpperRow = (mt_uint8*)pDst + ((        iRow    ) * strideInBytes);
            mt_uint8* pLowerRow = (mt_uint8*)pDst + ((sizeY - iRow - 1) * strideInBytes);

            for (iCol = 0; iCol < sizeX; ++iCol) {
                mt_uint32 iByte;
                for (iByte = 0; iByte < bpp; ++iByte) {
                    mt_uint8 tmp = pUpperRow[iCol*bpp + iByte];
                    pUpperRow[iCol*bpp + iByte] = pLowerRow[iCol*bpp + iByte];
                    pLowerRow[iCol*bpp + iByte] = tmp;
                }
            }
        }
    }
}

void mt_copy_and_flip_image_data_y(void* pDst, const void* pSrc, mt_uint32 sizeX, mt_uint32 sizeY, mt_uint32 dstStride, mt_format dstFormat, mt_uint32 srcStride, mt_format srcFormat)
{
    mt_copy_image_data(pDst, pSrc, sizeX, sizeY, dstStride, dstFormat, srcStride, srcFormat);
    mt_flip_image_data_y(pDst, sizeX, sizeY, dstStride, dstFormat);
}

mt_uint32 mt_get_bytes_per_pixel(mt_format format)
{
    switch (format)
    {
        case mt_format_rgba: return 4;
        case mt_format_rgb:  return 3;
        case mt_format_bgra: return 4;
        case mt_format_bgr:  return 3;
        case mt_format_argb: return 4;
        default: return 0;
    }
}

mt_bool32 mt_format_has_alpha(mt_format format)
{
    switch (format)
    {
        case mt_format_rgba:
        case mt_format_bgra:
        case mt_format_argb:
            return MT_TRUE;

        case mt_format_rgb:
        case mt_format_bgr:
        default:
            return MT_FALSE;
    }
}



/**************************************************************************************************************************************************************

Utilities

**************************************************************************************************************************************************************/
mt_bool32 mt_is_null_or_empty_utf8(const mt_utf8* pUTF8)
{
    return pUTF8 == NULL || pUTF8[0] == '\0';
}

mt_bool32 mt_is_null_or_whitespace_utf8(const mt_utf8* pUTF8)
{
    if (pUTF8 == NULL) {
        return MT_TRUE;
    }

    while (pUTF8[0] != '\0') {
        /* BEG AUTOGEN: is_null_or_whitespace_utf8 */
        if ((mt_uint8)pUTF8[0] >= 0x9 && (mt_uint8)pUTF8[0] <= 0xD) {
            pUTF8 += 1;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0x20) {
            pUTF8 += 1;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xC2 && (mt_uint8)pUTF8[1] == 0x85) {
            pUTF8 += 2;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xC2 && (mt_uint8)pUTF8[1] == 0xA0) {
            pUTF8 += 2;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE1 && (mt_uint8)pUTF8[1] == 0x9A && (mt_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x81) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x82) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x83) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x84) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x85) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x86) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x87) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x88) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x89) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x8A) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0xA8) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0xA9) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0xAF) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE2 && (mt_uint8)pUTF8[1] == 0x81 && (mt_uint8)pUTF8[2] == 0x9F) {
            pUTF8 += 3;
            continue;
        }
        if ((mt_uint8)pUTF8[0] == 0xE3 && (mt_uint8)pUTF8[1] == 0x80 && (mt_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        /* END AUTOGEN: is_null_or_whitespace_utf8 */

        return MT_FALSE;    /* Not whitespace. */
    }

    return MT_TRUE;
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
