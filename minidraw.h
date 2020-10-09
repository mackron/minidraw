/*
2D graphics library. Choice of public domain or MIT-0. See license statements at the end of this file.
minidraw - v0.x.x - 2019-xx-xx

David Reid - davidreidsoftware@gmail.com
*/

/* WORK IN PROGRESS */

#ifndef MINIDRAW_H
#define MINIDRAW_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4324)   /* structure was padded due to alignment specifier */
    #pragma warning(disable:4214)   /* nonstandard extension used: bit field types other than int */
#endif

/* Platform/backend detection. */
#ifdef _WIN32
    #define MD_WIN32
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        #define MD_WIN32_UWP
    #else
        #define MD_WIN32_DESKTOP
    #endif
#else
    #define MD_POSIX
    #include <pthread.h>    /* Unfortunate #include, but needed for pthread_t, pthread_mutex_t and pthread_cond_t types. */

    #ifdef __unix__
        #define MD_UNIX
        #if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #define MD_BSD
        #endif
    #endif
    #ifdef __linux__
        #define MD_LINUX
    #endif
    #ifdef __APPLE__
        #define MD_APPLE
    #endif
#endif

#include <stddef.h> /* For size_t. */

/* Sized types. Prefer built-in types. Fall back to stdint. */
#ifdef _MSC_VER
    #if defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
        #pragma GCC diagnostic ignored "-Wlong-long"
    #endif
    typedef   signed __int8  md_int8;
    typedef unsigned __int8  md_uint8;
    typedef   signed __int16 md_int16;
    typedef unsigned __int16 md_uint16;
    typedef   signed __int32 md_int32;
    typedef unsigned __int32 md_uint32;
    typedef   signed __int64 md_int64;
    typedef unsigned __int64 md_uint64;
    #if defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
#else
    #define MD_HAS_STDINT
    #include <stdint.h>
    typedef int8_t   md_int8;
    typedef uint8_t  md_uint8;
    typedef int16_t  md_int16;
    typedef uint16_t md_uint16;
    typedef int32_t  md_int32;
    typedef uint32_t md_uint32;
    typedef int64_t  md_int64;
    typedef uint64_t md_uint64;
#endif

#ifdef MD_HAS_STDINT
    typedef uintptr_t md_uintptr;
#else
    #if defined(_WIN32)
        #if defined(_WIN64)
            typedef md_uint64 md_uintptr;
        #else
            typedef md_uint32 md_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
            typedef md_uint64 md_uintptr;
        #else
            typedef md_uint32 md_uintptr;
        #endif
    #else
        typedef md_uint64 md_uintptr;   /* Fallback. */
    #endif
#endif
typedef md_uint8    md_bool8;
typedef md_uint32   md_bool32;
#define MD_TRUE     1
#define MD_FALSE    0

typedef void* md_handle;
typedef void* md_ptr;
typedef void (* md_proc)(void);

#if defined(_MSC_VER) && !defined(_WCHAR_T_DEFINED)
typedef md_uint16 wchar_t;
#endif

typedef char      md_utf8;
typedef md_uint16 md_utf16;
typedef md_uint32 md_utf32;

/* Define NULL for some compilers. */
#ifndef NULL
#define NULL 0
#endif

#if defined(SIZE_MAX)
    #define MD_SIZE_MAX     SIZE_MAX
#else
    #define MD_SIZE_MAX     0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif

#define MD_PI       3.14159265358979323846
#define MD_PIF      3.14159265358979323846f

#define MD_DEGREES(radians)  ((double)((radians) * 57.29577951308232087685))
#define MD_DEGREESF(radians) ( (float)((radians) * 57.29577951308232087685f))
#define MD_RADIANS(degrees)  ((double)((degrees) *  0.01745329251994329577))
#define MD_RADIANSF(degrees) ( (float)((degrees) *  0.01745329251994329577f))


#ifdef _MSC_VER
#define MD_INLINE __forceinline
#else
#ifdef __GNUC__
#define MD_INLINE __inline__ __attribute__((always_inline))
#else
#define MD_INLINE
#endif
#endif

#define MD_PRIVATE


/* Backend Support */
#if defined(MD_WIN32)
    #define MD_SUPPORT_GDI
    #define MD_SUPPORT_DIRECT2D
#endif
#if defined(MD_APPLE)
    #define MD_SUPPORT_COREGRAPHICS
#endif
#if defined(MD_UNIX) || defined(MD_LINUX)
    #define MD_SUPPORT_CAIRO
    #define MD_SUPPORT_XFT
#endif

/* Backends */
#if !defined(MD_NO_GDI) && defined(MD_SUPPORT_GDI)
    #define MD_ENABLE_GDI
#endif
#if !defined(MD_NO_DIRECT2D) && defined(MD_SUPPORT_DIRECT2D)
    #define MD_ENABLE_DIRECT2D
#endif
#if !defined(MD_NO_COREGRAPHICS) && defined(MD_SUPPORT_COREGRAPHICS)
    #define MD_ENABLE_COREGRAPHICS
#endif
#if !defined(MD_NO_CAIRO) && defined(MD_SUPPORT_CAIRO)
    #define MD_ENABLE_CAIRO
#endif
#if !defined(MD_NO_XFT) && defined(MD_SUPPORT_XFT)
    #define MD_ENABLE_XFT
#endif


/* Result Codes */
typedef int md_result;
#define MD_SUCCESS                   0

/* General errors. */
#define MD_ERROR                    -1  /* A generic error. */
#define MD_INVALID_ARGS             -2
#define MD_INVALID_OPERATION        -3
#define MD_OUT_OF_MEMORY            -4
#define MD_OUT_OF_RANGE             -5
#define MD_TOO_BIG                  -6
#define MD_ACCESS_DENIED            -7
#define MD_DOES_NOT_EXIST           -8
#define MD_ALREADY_EXISTS           -9
#define MD_TOO_MANY_OPEN_FILES      -10
#define MD_INVALID_FILE             -11
#define MD_FAILED_TO_OPEN_FILE      -12
#define MD_FAILED_TO_READ_FILE      -13
#define MD_FAILED_TO_WRITE_FILE     -14
#define MD_END_OF_FILE              -15
#define MD_NO_SPACE                 -16
#define MD_NEGATIVE_SEEK            -17
#define MD_PATH_TOO_LONG            -18
#define MD_NAME_TOO_LONG            -19
#define MD_MEMORY_ALREADY_MAPPED    -20
#define MD_TIMEOUT                  -21
#define MD_CANCELLED                -22
#define MD_INVALID_BOM              -200
#define MD_INVALID_CODE_POINT       -201


/* Forward Declarations */
typedef struct md_api md_api;
typedef struct md_api_config md_api_config;
typedef struct md_font md_font;
typedef struct md_font_config md_font_config;
typedef struct md_brush md_brush;
typedef struct md_brush_config md_brush_config;
typedef struct md_gc md_gc;
typedef struct md_gc_config md_gc_config;

/* Enumerators */
typedef enum
{
    md_backend_gdi,            /* Typography via Uniscribe */
    /*md_backend_direct2d,*/       /* Typography via DirectWrite */
    /*md_backend_coregraphics,*/   /* Typography via Core Text */
    md_backend_cairo,         /* Typography via Pango */
    /*md_backend_xft*/             /* Typography via minidraw */
    md_backend_custom
} md_backend;

typedef enum
{
    md_font_weight_medium = 0,
    md_font_weight_thin,
    md_font_weight_extra_light,
    md_font_weight_light,
    md_font_weight_semi_light,
    md_font_weight_book,
    md_font_weight_semi_bold,
    md_font_weight_bold,
    md_font_weight_extra_bold,
    md_font_weight_heavy,
    md_font_weight_extra_heavy,
    md_font_weight_normal  = md_font_weight_medium,
    md_font_weight_default = md_font_weight_medium
} md_font_weight;

typedef enum
{
    md_font_slant_none = 0,
    md_font_slant_italic,
    md_font_slant_oblique
} md_font_slant;

typedef enum
{
    md_line_cap_flat   = 0,
    md_line_cap_round  = 1,
    md_line_cap_square = 2
} md_line_cap;

typedef enum
{
    md_line_join_miter = 0,
    md_line_join_round = 1,
    md_line_join_bevel = 2
} md_line_join;

typedef enum
{
    md_brush_type_solid  = 0,       /* RGB or RGBA */
    md_brush_type_linear = 1,       /* Source is a linear gradient. */
    md_brush_type_radial = 2,       /* Source is a radial gradient. */
    md_brush_type_gc     = 3        /* Source is a graphics context. */
} md_brush_type;

typedef enum
{
    md_blend_op_src      = 0,       /* Default. Draws the source over the top of the destination with no blending. */
    md_blend_op_src_over = 1        /* Standard alpha blending. */
} md_blend_op;

typedef enum
{
    md_antialias_mode_default  = 0, /* Let the backend decide, but prefer anti-aliasing if available. Will be the same as md_antialias_none on GDI since GDI does not support anti-aliasing. */
    md_antialias_mode_none     = 1, /* Anti-aliasing will be disabled. Useful for straight-edge primitives like un-rotated rectangles or where performance is a concern. */
    md_antialias_mode_gray     = 2, /* Standard grayscale anti-aliasing. */
    md_antialias_mode_subpixel = 3  /* ClearType RGB style anti-aliasing. Falls back to md_antialias_mode_gray if unavailable. */
} md_antialias_mode;

typedef enum
{
    md_fill_mode_winding = 0,       /* Default. */
    md_fill_mode_evenodd
} md_fill_mode;

typedef enum
{
    md_stretch_filter_nearest = 0,  /* Default. */
    md_stretch_filter_linear  = 1
} md_stretch_filter;

typedef enum
{
    md_format_unknown = 0,
    md_format_rgba,
    md_format_rgb,
    md_format_bgra, /* Optimal format for GDI. */
    md_format_bgr,
    md_format_argb  /* Optimal format for Cairo. */
} md_format;

typedef enum
{
    md_alignment_left,
    md_alignment_right,
    md_alignment_top,
    md_alignment_bottom,
    md_alignment_center,
} md_alignment;

/* Structures */
#if defined(MD_SUPPORT_GDI)
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic" /* type of bit-field is a GCC extension */
#endif
typedef struct 
{
    md_uint16 uJustification : 4;
    md_uint16 fClusterStart  : 1;
    md_uint16 fDiacritic     : 1;
    md_uint16 fZeroWidth     : 1;
    md_uint16 fReserved      : 1;
    md_uint16 fShapeReserved : 8;
} MD_SCRIPT_VISATTR;

typedef struct
{
    md_uint16 uBidiLevel         : 5;
    md_uint16 fOverrideDirection : 1;
    md_uint16 fInhibitSymSwap    : 1;
    md_uint16 fCharShape         : 1;
    md_uint16 fDigitSubstitute   : 1;
    md_uint16 fInhibitLigate     : 1;
    md_uint16 fDisplayZWG        : 1;
    md_uint16 fArabicNumContext  : 1;
    md_uint16 fGcpClusters       : 1;
    md_uint16 fReserved          : 1;
    md_uint16 fEngineReserved    : 2;
} MD_SCRIPT_STATE;

typedef struct
{
    md_uint16 eScript       : 10;
    md_uint16 fRTL          : 1;
    md_uint16 fLayoutRTL    : 1;
    md_uint16 fLinkBefore   : 1;
    md_uint16 fLinkAfter    : 1;
    md_uint16 fLogicalOrder : 1;
    md_uint16 fNoGlyphIndex : 1;
    MD_SCRIPT_STATE s;
} MD_SCRIPT_ANALYSIS;
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

typedef struct
{
    int iCharPos;
    MD_SCRIPT_ANALYSIS a;
} MD_SCRIPT_ITEM;
#endif

typedef struct
{
    md_uint8 r;
    md_uint8 g;
    md_uint8 b;
    md_uint8 a;
} md_color;

typedef struct
{
    md_int32 left;
    md_int32 top;
    md_int32 right;
    md_int32 bottom;
} md_rect;

typedef struct
{
    float m00;  /* Rotation cosine. Horizontal scale. */
    float m01;  /* Rotation sine. */
    float m10;  /* Rotation negative sine. */
    float m11;  /* Rotation cosine. Vertical scale. */
    float dx;   /* X translation. */
    float dy;   /* Y translation. */
} md_matrix;

typedef struct
{
    md_uint32 index;
    md_int32 advance;
    union
    {
    #if defined(MD_SUPPORT_GDI)
        struct
        {
            MD_SCRIPT_VISATTR sv;   /* Passed around to ScriptShape() and ScriptPlace(). */
            long offsetX;           /* For filling in the GOFFSET parameter of ScriptTextOut(). */
            long offsetY;           /* For filling in the GOFFSET parameter of ScriptTextOut(). */
        } gdi;
    #endif
    #if defined(MD_SUPPORT_CAIRO)
        struct
        {
            md_int32  width;                /* From PangoGlyphGeometry. */
            md_int32  offsetX;              /* From PangoGlyphGeometry. */
            md_int32  offsetY;              /* From PangoGlyphGeometry. */
            md_uint32 isClusterStart : 1;   /* From PangoGlyphVisAttr */
        } cairo;
    #endif
        md_uint32 _unused;
    } backend;
} md_glyph;

typedef struct
{
    md_int32 lPadding;
    md_int32 rPadding;
    md_int32 sizeX;
    md_int32 sizeY;
} md_text_metrics;

typedef struct
{
    md_int32 ascent;
    md_int32 descent;
} md_font_metrics;

typedef struct
{
    md_int32 sizeX;
    md_int32 sizeY;
    md_int32 bearingX;
    md_int32 bearingY;
    md_int32 advanceX;
    md_int32 advanceY;
} md_glyph_metrics;

typedef struct
{
    md_int32 boundsX;
    md_int32 boundsY;
    md_int32 boundsSizeX;
    md_int32 boundsSizeY;
    md_int32 textOffsetX;
    md_int32 textOffsetY;
    md_rect padding;
    md_alignment alignmentX;
    md_alignment alignmentY;
    md_int32 borderWidth;       /* When positive, draws the border on the inside of the bounds. When negative draws it on the outside. */
    md_color borderColor;
    md_int32 tabWidthInPixels;  /* When 0, falls back to tabSizeInSpaces. */
    md_int32 tabWidthInSpaces;  /* Used when tabSizeInPixels is 0. */
    md_bool32 fillBackground : 1;
    md_bool32 singleLine     : 1;
} md_text_layout;

typedef struct
{
    size_t offset;  /* Offset in code units (bytes for UTF-8, shorts for UTF-16, integers for UTF-32). */
    size_t length;  /* Length in code units. */

    /* Backend-specific data. */
    union
    {
    #if defined(MD_SUPPORT_GDI)
        struct
        {
            MD_SCRIPT_ANALYSIS sa;          /* Passed around to Script*() APIs. */
            /*HFONT*/ md_handle hFont;      /* The font to use when drawing this item. */
            /*SCRIPT_CACHE*/ md_ptr sc;     /* The SCRIPT_CACHE object passed around to ScriptShape(), ScriptPlace() and ScriptTextOut(). */
        } gdi;
    #endif
    #if defined(MD_SUPPORT_CAIRO)
        struct
        {
            /*PangoItem**/ md_ptr pPangoItem;
        } cairo;
    #endif
        md_uint32 _unused;
    } backend;
} md_item;

typedef struct
{
    md_api* pAPI;
    union
    {
    #if defined(MD_SUPPORT_CAIRO)
        struct
        {
            /*GList**/ md_ptr pPangoItems;
        } cairo;
    #endif
        int _unused;
    } backend;
} md_itemize_state;


typedef void      (* md_uninit_proc)                      (md_api* pAPI);
typedef md_result (* md_itemize_utf8_proc)                (md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState);
typedef md_result (* itemize_utf16_proc)                  (md_font* pFont, const md_utf16* pTextUTF16, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState);
typedef md_result (* itemize_utf32_proc)                  (md_font* pFont, const md_utf32* pTextUTF32, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState);
typedef void      (* free_itemize_state_proc)             (md_itemize_state* pItemizeState);
typedef md_result (* shape_utf8_proc)                     (md_font* pFont, md_item* pItem, const md_utf8* pTextUTF8, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics);
typedef md_result (* shape_utf16_proc)                    (md_font* pFont, md_item* pItem, const md_utf16* pTextUTF16, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics);
typedef md_result (* shape_utf32_proc)                    (md_font* pFont, md_item* pItem, const md_utf32* pTextUTF32, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics);
typedef md_result (* font_init_proc)                      (md_api* pAPI, const md_font_config* pConfig, md_font* pFont);
typedef void      (* font_uninit_proc)                    (md_font* pFont);
typedef md_result (* font_get_glyph_metrics_proc)         (md_font* pFont, const md_glyph* pGlyphs, size_t glyphCount, md_glyph_metrics* pGlyphMetrics);
typedef md_result (* font_get_glyph_metrics_by_index_proc)(md_font* pFont, const md_uint32* pGlyphIndices, size_t glyphCount, md_glyph_metrics* pGlyphMetrics);
typedef md_result (* brush_init_proc)                     (md_api* pAPI, const md_brush_config* pConfig, md_brush* pBrush);
typedef void      (* brush_uninit_proc)                   (md_brush* pBrush);
typedef void      (* brush_set_origin_proc)               (md_brush* pBrush, md_int32 x, md_int32 y);
typedef md_result (* gc_init_proc)                        (md_api* pAPI, const md_gc_config* pConfig, md_gc* pGC);
typedef void      (* gc_uninit_proc)                      (md_gc* pGC);
typedef md_result (* gc_get_image_data_proc)              (md_gc* pGC, md_format outputFormat, void* pImageData);
typedef md_result (* gc_get_size_proc)                    (md_gc* pGC, md_uint32* pSizeX, md_uint32* pSizeY);
typedef md_result (* gc_save_proc)                        (md_gc* pGC);
typedef md_result (* gc_restore_proc)                     (md_gc* pGC);
typedef void      (* gc_set_matrix_proc)                  (md_gc* pGC, const md_matrix* pMatrix);
typedef void      (* gc_get_matrix_proc)                  (md_gc* pGC, md_matrix* pMatrix);
typedef void      (* gc_set_matrix_identity_proc)         (md_gc* pGC);
typedef void      (* gc_transform_proc)                   (md_gc* pGC, const md_matrix* pMatrix);
typedef void      (* gc_translate_proc)                   (md_gc* pGC, md_int32 offsetX, md_int32 offsetY);
typedef void      (* gc_rotate_proc)                      (md_gc* pGC, float rotationInRadians);
typedef void      (* gc_scale_proc)                       (md_gc* pGC, float scaleX, float scaleY);
typedef void      (* gc_set_line_width_proc)              (md_gc* pGC, md_int32 width);
typedef void      (* gc_set_line_cap_proc)                (md_gc* pGC, md_line_cap cap);
typedef void      (* gc_set_line_join_proc)               (md_gc* pGC, md_line_join join);
typedef void      (* gc_set_miter_limit_proc)             (md_gc* pGC, float limit);
typedef void      (* gc_set_line_dash_proc)               (md_gc* pGC, const float* dashes, md_uint32 count);
typedef void      (* gc_set_line_brush_proc)              (md_gc* pGC, md_brush* pBrush);
typedef void      (* gc_set_line_brush_solid_proc)        (md_gc* pGC, md_color color);
typedef void      (* gc_set_line_brush_gc_proc)           (md_gc* pGC, md_gc* pSrcGC);
typedef void      (* gc_set_fill_brush_proc)              (md_gc* pGC, md_brush* pBrush);
typedef void      (* gc_set_fill_brush_solid_proc)        (md_gc* pGC, md_color color);
typedef void      (* gc_set_fill_brush_gc_proc)           (md_gc* pGC, md_gc* pSrcGC);
typedef void      (* gc_set_text_fg_color_proc)           (md_gc* pGC, md_color fgColor);
typedef void      (* gc_set_text_bg_color_proc)           (md_gc* pGC, md_color bgColor);
typedef md_color  (* gc_get_text_fg_color_proc)           (md_gc* pGC);
typedef md_color  (* gc_get_text_bg_color_proc)           (md_gc* pGC);
typedef void      (* gc_set_blend_op_proc)                (md_gc* pGC, md_blend_op op);
typedef void      (* gc_set_antialias_mode_proc)          (md_gc* pGC, md_antialias_mode mode);
typedef void      (* gc_set_fill_mode_proc)               (md_gc* pGC, md_fill_mode mode);
typedef void      (* gc_set_stretch_filter_proc)          (md_gc* pGC, md_stretch_filter filter);
typedef void      (* gc_move_to_proc)                     (md_gc* pGC, md_int32 x, md_int32 y);
typedef void      (* gc_line_to_proc)                     (md_gc* pGC, md_int32 x, md_int32 y);
typedef void      (* gc_rectangle_proc)                   (md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom);
typedef void      (* gc_arc_proc)                         (md_gc* pGC, md_int32 x, md_int32 y, md_int32 radius, float angle1InRadians, float angle2InRadians);
typedef void      (* gc_curve_to_proc)                    (md_gc* pGC, md_int32 x1, md_int32 y1, md_int32 x2, md_int32 y2, md_int32 x3, md_int32 y3);
typedef void      (* gc_close_path_proc)                  (md_gc* pGC);
typedef void      (* gc_clip_proc)                        (md_gc* pGC);
typedef void      (* gc_reset_clip_proc)                  (md_gc* pGC);
typedef md_bool32 (* gc_is_point_inside_clip_proc)        (md_gc* pGC, md_int32 x, md_int32 y);
typedef void      (* gc_fill_proc)                        (md_gc* pGC);
typedef void      (* gc_stroke_proc)                      (md_gc* pGC);
typedef void      (* gc_fill_and_stroke_proc)             (md_gc* pGC);
typedef void      (* gc_draw_gc_proc)                     (md_gc* pGC, md_gc* pSrcGC, md_int32 srcX, md_int32 srcY);
typedef void      (* gc_draw_glyphs_proc)                 (md_gc* pGC, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32 x, md_int32 y);
typedef void      (* gc_clear_proc)                       (md_gc* pGC, md_color color);

typedef struct
{
    md_uninit_proc                       uninit;
    md_itemize_utf8_proc                 itemizeUTF8;
    itemize_utf16_proc                   itemizeUTF16;
    itemize_utf32_proc                   itemizeUTF32;
    free_itemize_state_proc              freeItemizeState;
    shape_utf8_proc                      shapeUTF8;
    shape_utf16_proc                     shapeUTF16;
    shape_utf32_proc                     shapeUTF32;
    font_init_proc                       fontInit;
    font_uninit_proc                     fontUninit;
    font_get_glyph_metrics_proc          fontGetGlyphMetrics;
    font_get_glyph_metrics_by_index_proc fontGetGlyphMetricsByIndex;
    brush_init_proc                      brushInit;
    brush_uninit_proc                    brushUninit;
    brush_set_origin_proc                brushSetOrigin;
    gc_init_proc                         gcInit;
    gc_uninit_proc                       gcUninit;
    gc_get_image_data_proc               gcGetImageData;
    gc_get_size_proc                     gcGetSize;
    gc_save_proc                         gcSave;
    gc_restore_proc                      gcRestore;
    gc_set_matrix_proc                   gcSetMatrix;
    gc_get_matrix_proc                   gcGetMatrix;
    gc_set_matrix_identity_proc          gcSetMatrixIdentity;
    gc_transform_proc                    gcTransform;
    gc_translate_proc                    gcTranslate;
    gc_rotate_proc                       gcRotate;
    gc_scale_proc                        gcScale;
    gc_set_line_width_proc               gcSetLineWidth;
    gc_set_line_cap_proc                 gcSetLineCap;
    gc_set_line_join_proc                gcSetLineJoin;
    gc_set_miter_limit_proc              gcSetMiterLimit;
    gc_set_line_dash_proc                gcSetLineDash;
    gc_set_line_brush_proc               gcSetLineBrush;
    gc_set_line_brush_solid_proc         gcSetLineBrushSolid;
    gc_set_line_brush_gc_proc            gcSetLineBrushGC;
    gc_set_fill_brush_proc               gcSetFillBrush;
    gc_set_fill_brush_solid_proc         gcSetFillBrushSolid;
    gc_set_fill_brush_gc_proc            gcSetFillBrushGC;
    gc_set_text_fg_color_proc            gcSetTextFGColor;
    gc_set_text_bg_color_proc            gcSetTextBGColor;
    gc_get_text_fg_color_proc            gcGetTextFGColor;
    gc_get_text_bg_color_proc            gcGetTextBGColor;
    gc_set_blend_op_proc                 gcSetBlendOp;
    gc_set_antialias_mode_proc           gcSetAntialiasMode;
    gc_set_fill_mode_proc                gcSetFillMode;
    gc_set_stretch_filter_proc           gcSetStretchFilter;
    gc_move_to_proc                      gcMoveTo;
    gc_line_to_proc                      gcLineTo;
    gc_rectangle_proc                    gcRectangle;
    gc_arc_proc                          gcArc;
    gc_curve_to_proc                     gcCurveTo;
    gc_close_path_proc                   gcClosePath;
    gc_clip_proc                         gcClip;
    gc_reset_clip_proc                   gcResetClip;
    gc_is_point_inside_clip_proc         gcIsPointInsideClip;
    gc_fill_proc                         gcFill;
    gc_stroke_proc                       gcStroke;
    gc_fill_and_stroke_proc              gcFillAndStroke;
    gc_draw_gc_proc                      gcDrawGC;
    gc_draw_glyphs_proc                  gcDrawGlyphs;
    gc_clear_proc                        gcClear;
} md_api_procs;

struct md_api_config
{
    md_backend backend;
    void* pUserData;
    struct
    {
        /*HDC*/ md_handle hDC;  /* Optional pre-created global device context. */
    } gdi;
    struct
    {
        /*PangoContext**/ md_ptr pPangoContext; /* Optional application-defined PangoContext. When set to null, the pango context is set to pango_font_map_create_context(pango_cairo_font_map_get_default()) */
    } cairo;
    struct
    {
        md_api_procs procs;
    } custom;
};

struct md_api
{
    md_backend backend;
    void* pUserData;
    md_api_procs procs;
#if defined(MD_WIN32)
    struct
    {
        int _unused;
    } win32;
#endif
#if defined(MD_APPLE)
    struct
    {
        int _unused;
    } apple;
#endif

#if defined(MD_SUPPORT_GDI)
    struct
    {
        /*HDC*/ md_handle hGlobalDC;    /* Global DC for font creation and glyph shaping and placement. */
        /*HBRUSH*/ md_handle hStockSolidFillBrush;  /* The GDI stock brush to use for solid brushes. */

        /* usp10.dll */
        md_handle hUsp10DLL;
        md_proc ScriptItemize;
        md_proc ScriptShape;
        md_proc ScriptPlace;
        md_proc ScriptTextOut;
        md_proc ScriptFreeCache;

        /* gdi32.dll */
        md_handle hGdi32DLL;

        /* msimg32.dll */
        md_handle hMsimg32DLL;
        md_proc AlphaBlend;

        md_bool32 ownsGlobalDC : 1;
    } gdi;
#endif
#if defined(MD_SUPPORT_DIRECT2D)
    struct
    {
        int _unused;
    } direct2d;
#endif
#if defined(MD_SUPPORT_COREGRAPHICS)
    struct
    {
        int _unused;
    } coregraphics;
#endif
#if defined(MD_SUPPORT_CAIRO)
    struct
    {
        /*PangoContext**/ md_ptr pPangoContext; /* Created with the PangoFontMap retrieved with pango_cairo_font_map_get_default(). */
        md_bool32 ownsPangoContext : 1;
    } cairo;
#endif
#if defined(MD_SUPPORT_XFT)
    struct
    {
        int _unused;
    } xft;
#endif
    struct
    {
        int _unused;
    } custom;
};

struct md_font_config
{
    const char* family;
    md_uint32 sizeInPixels; /* If set to 0, sizeInPoints will be used instead. */
    md_uint32 sizeInPoints; /* If set to 0, sizeInPixels will be used instead. */
    md_font_weight weight;
    md_font_slant slant;
    md_antialias_mode antialiasMode;    /* Preferred anti-aliasing to use with this font. This is just a hint. */
    void* pUserData;
    struct
    {
        /*PangoFontMap**/ md_ptr pPangoFontMap; /* Custom application-defined PangoFontMap to use when creating the font. */
        /*PangoContext**/ md_ptr pPangoContext; /* Custom application-defined PangoContext to use when creating the font. */
    } cairo;
};

struct md_font
{
    md_api* pAPI;   /* The md_api object that was used to initialize the font. */
    void* pUserData;
    md_font_metrics metrics;

#if defined(MD_SUPPORT_GDI)
    struct
    {
        /*HFONT*/ md_handle hFont;
        /*SCRIPT_CACHE*/ md_ptr sc; /* The SCRIPT_CACHE object passed around to ScriptShape(), ScriptPlace() and ScriptTextOut(). */
    } gdi;
#endif
#if defined(MD_SUPPORT_DIRECT2D)
    struct
    {
        int _unused;
    } direct2d;
#endif
#if defined(MD_SUPPORT_COREGRAPHICS)
    struct
    {
        int _unused;
    } coregraphics;
#endif
#if defined(MD_SUPPORT_CAIRO)
    struct
    {
        /*PangoFont**/ md_ptr pPangoFont;
        /*PangoFontDescription**/ md_ptr pPangoFontDesc;
        /*PangoAttrList**/ md_ptr pPangoAttrList;   /* Passed into pango_itemize(). */
    } cairo;
#endif
#if defined(MD_SUPPORT_XFT)
    struct
    {
        int _unused;
    } xft;
#endif
};

typedef struct
{
    float offset;   /* Between 0 and 1. */
    md_color color;
} md_color_stop;

struct md_brush_config
{
    md_brush_type type;
    void* pUserData;
    md_color_stop* pColorStops; /* Only used with linear and radial brushes. */
    md_uint32 colorStopCount;
    struct
    {
        md_color color;
    } solid;
    struct
    {
        md_int32 x0;
        md_int32 y0;
        md_int32 x1;
        md_int32 x2;
    } linear;
    struct
    {
        md_int32 cx0;       /* Center of the start circle. */
        md_int32 cy0;
        md_int32 radius0;   /* Radius of the start circle. */
        md_int32 cx1;       /* Center of the end circle. */
        md_int32 cy1;
        md_int32 radius1;   /* Radius of the end circle. */
    } radial;
    struct
    {
        md_gc* pGC;
    } gc;
};

struct md_brush
{
    md_api* pAPI;
    void* pUserData;
    md_brush_config config;
    
#if defined(MD_SUPPORT_GDI)
    struct
    {
        /*HBRUSH*/  md_handle hBrush;
        /*HBITMAP*/ md_handle hBitmap;  /* Only used for complex brushes. Owned by the md_brush object and deleted in md_brush_uninit(). */
        md_int32 originX;
        md_int32 originY;
        md_bool32 ownsBitmap : 1;       /* When set to true, hBitmap will be destroyed in md_brush_uninit(). */
    } gdi;
#endif
#if defined(MD_SUPPORT_CAIRO)
    struct
    {
        /*cairo_pattern_t**/ md_ptr pCairoPattern;
        /*cairo_surface_t**/ md_ptr pCairoSurface;  /* Only used when the source is a cairo surface of a transient GC. */
    } cairo;
#endif
};

struct md_gc_config
{
    md_uint32 sizeX;                    /* You should set this for Cairo even when passing in a pre-existing cairo_t object. */
    md_uint32 sizeY;                    /* You should set this for Cairo even when passing in a pre-existing cairo_t object. */
    md_uint32 stride;                   /* Stride in pixels. Only used when pInitialImageData is not null. */
    md_format format;                   /* The format of the data contained in pInitialImageData (if any) and the preferred internal format. Cannot be md_format_unkonwn if pInitialImageData is not null. */
    const void* pInitialImageData;      /* Can be null in which case the initial contents are undefined. */
    void* pUserData;

#if defined(MD_SUPPORT_GDI)
    struct
    {
        /*HDC*/ md_handle hDC;          /* Existing HDC to use as the rendering target. Set this for transient context's such as those used in BeginPaint()/EndPaint() pairs. */
    } gdi;
#endif
#if defined(MD_SUPPORT_CAIRO)
    struct
    {
        /*cairo_t**/ md_ptr pCairoContext;
    } cairo;
#endif
};

#if defined(MD_SUPPORT_GDI)
typedef struct
{
    /*HPEN*/ md_handle hPen;            /* Lazily initialized. Set to NULL to indicate the case when a new pen handle needs to be created. */
    md_int32 lineWidth;
    md_line_cap lineCap;
    md_line_join lineJoin;
    md_uint32 dashCount;
    float dashes[16];
    md_blend_op blendOp;
    md_color textFGColor;
    md_color textBGColor;
    md_brush_config lineStockBrush;     /* For use with solid and GC brushes. Only used when pUserLineBrush is NULL. */
    md_brush* pUserLineBrush;           /* For use with user-defined brushes. */
    md_brush transientFillBrush;        /* For use with solid and GC brushes. Only used when pUserFillBrush is NULL. */
    md_brush* pUserFillBrush;           /* For use with user-defined brushes. */
    md_bool32 hasTransientFillBrush : 1;
    md_bool32 hasPathBegun : 1;         /* Determines whether or not BeginPath has been called. */
} md_gc_state_gdi;
#endif
#if defined(MD_SUPPORT_CAIRO)
typedef struct
{
    md_stretch_filter stretchFilter;
    md_color textFGColor;
    md_color textBGColor;
    md_brush* pFillBrush;
    md_brush* pLineBrush;
    md_brush_config transientFillBrush;
    md_brush_config transientLineBrush;
    md_bool32 hasTransientFillBrush : 1;
    md_bool32 hasTransientLineBrush : 1;
} md_gc_state_cairo;
#endif

struct md_gc
{
    md_api* pAPI;                       /* The md_api object that was used to initialize the graphics context. */
    void* pUserData;
    md_format format;
    md_bool32 isTransient : 1;

#if defined(MD_SUPPORT_GDI)
    struct
    {
        /*HDC*/ md_handle hDC;
        /*HBITMAP*/ md_handle hBitmap;  /* The HBITMAP object that will be used as the target for graphics output. */
        void* pBitmapData;              /* A pointer to the raw bitmap data. Owned by hBitmap. Freed by GDI when hBitmap is deleted. */
        md_uint32 bitmapSizeX;
        md_uint32 bitmapSizeY;
        md_gc_state_gdi* pState;        /* Heap allocated via realloc() for now. May change to a per-API allocation scheme. */
        md_uint32 stateCap;             /* The capacity of pState. */
        md_uint32 stateCount;           /* The number of valid items in pState. */
    } gdi;
#endif
#if defined(MD_SUPPORT_DIRECT2D)
    struct
    {
        int _unused;
    } direct2d;
#endif
#if defined(MD_SUPPORT_COREGRAPHICS)
    struct
    {
        int _unused;
    } coregraphics;
#endif
#if defined(MD_SUPPORT_CAIRO)
    struct
    {
        /*cairo_t**/ md_ptr pCairoContext;
        /*cairo_surface_t**/ md_ptr pCairoSurface;
        void* pCairoSurfaceData;
        md_uint32 cairoSurfaceSizeX;
        md_uint32 cairoSurfaceSizeY;
        md_gc_state_cairo* pState;      /* Heap allocated via realloc() for now. May change to a per-API allocation scheme. */
        md_uint32 stateCap;             /* The capacity of pState. */
        md_uint32 stateCount;           /* The number of valid items in pState. */
    } cairo;
#endif
#if defined(MD_SUPPORT_XFT)
    struct
    {
        int _unused;
    } xft;
#endif
};



/**************************************************************************************************************************************************************

Unicode

**************************************************************************************************************************************************************/
#define MD_UNICODE_MIN_CODE_POINT                       0x000000
#define MD_UNICODE_MAX_CODE_POINT                       0x10FFFF
#define MD_UNICODE_REPLACEMENT_CHARACTER                0x00FFFD
#define MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF8    3
#define MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16   1
#define MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF32   1

#define MD_FORBID_BOM                                   (1 << 1)
#define MD_ERROR_ON_INVALID_CODE_POINT                  (1 << 2)

/*
Checks if the given UTF-16 byte order mark indicates little endian.
*/
MD_INLINE md_bool32 md_is_utf16_bom_le(const md_uint8 bom[2])
{
    /* RFC 2781 - Section 3.2 */
    return bom[0] == 0xFF && bom[1] == 0xFE;
}

/*
Checks if the given UTF-16 byte order mark indicates big endian.
*/
MD_INLINE md_bool32 md_is_utf16_bom_be(const md_uint8 bom[2])
{
    /* RFC 2781 - Section 3.2 */
    return bom[0] == 0xFE && bom[1] == 0xFF;
}

/*
Checks if the given UTF-32 byte order mark indicates little endian.
*/
MD_INLINE md_bool32 md_is_utf32_bom_le(const md_uint8 bom[4])
{
    return bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00;
}

/*
Checks if the given UTF-32 byte order mark indicates big endian.
*/
MD_INLINE md_bool32 md_is_utf32_bom_be(const md_uint8 bom[4])
{
    return bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF;
}

/*
Checks whether or not the given bytes start with a UTF-8 byte order mark.
*/
MD_INLINE md_bool32 md_has_utf8_bom(const md_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MD_FALSE;
    }

    if (len < 3) {
        return MD_FALSE;
    }

    return (pBytes[0] == 0xEF && pBytes[1] == 0xBB && pBytes[2] == 0xBF);
}

/*
Checks whether or not the given bytes start with a UTF-16 byte order mark.
*/
MD_INLINE md_bool32 md_has_utf16_bom(const md_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MD_FALSE;
    }

    if (len < 2) {
        return MD_FALSE;
    }

    return md_is_utf16_bom_le(pBytes) || md_is_utf16_bom_be(pBytes);
}

/*
Checks whether or not the given bytes start with a UTF-16 byte order mark.
*/
MD_INLINE md_bool32 md_has_utf32_bom(const md_uint8* pBytes, size_t len)
{
    if (pBytes == NULL) {
        return MD_FALSE;
    }

    if (len < 4) {
        return MD_FALSE;
    }

    return md_is_utf32_bom_le(pBytes) || md_is_utf32_bom_be(pBytes);
}

/*
Swaps the endianness of the given UTF-16 string.
*/
void md_swap_endian_utf16(md_utf16* pUTF16, size_t count);

/*
Swaps the endianness of the given UTF-32 string.
*/
void md_swap_endian_utf32(md_utf32* pUTF32, size_t count);


/*
Calculates the number of UTF-16's required to fully contain the given UTF-8 string after conversion, not including the null temrinator.

Remarks
-------
This is the same as md_utf8_to_utf16ne/le/be(), except it doesn't actually do any conversion.
*/
md_result md_utf8_to_utf16_length(size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags);
md_result md_utf8_to_utf16ne_length(size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }
md_result md_utf8_to_utf16le_length(size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }
md_result md_utf8_to_utf16be_length(size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags); }

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
        MD_FORBID_BOM
            Causes an error if the string starts with a BOM.
        MD_ERROR_ON_INVALID_CODE_POINT
            When an invalid character is encounted, return an error instead of replacing it with the replacement character.
    This can be 0 in which case it will use defaults.


Return Value
------------
MD_SUCCESS if successful; an error result otherwise. Converting an empty string is valid.

MD_INVALID_BOM will be returned if the string starts with a byte order mark and MD_FORBID_BOM is set in [flags].

MD_INVALID_ARGS will be returned if [pUTF8] is null.

MD_INVALID_ARGS will be returned if [pUTF8] is not big enough to fully convert the last code point.

MD_OUT_OF_MEMORY will be returned if the [pUTF16] buffer is too small to contain the entire converted string including the null terminated.

MD_INVALID_CODE_POINT will be returned if an invalid code point is encounted and MD_ERROR_ON_INVALID_CODE_POINT is set in [flags].


Remarks
-------
The output string will always be null terminated. If the output buffer is not large enough to store the null terminator MD_OUT_OF_MEMORY will be returned.

When the output string ([pUTF16]) is null, this is equivalent to `md_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags)`.

The byte order mark will _not_ be included in the output string nor it's length.

Invalid characters are replaced with the Unicode replacement character (MD_UNICODE_REPLACEMENT_CHARACTER/U+FFFD) unless the MD_ERROR_ON_INVALID_CODE_POINT
flag is set in which case an error will be returned.

You can look at [pUTF16Len] to know how many output characters were processed and [pUTF8LenProcessed] to know how many input characters were processed even
when an error occurs. This is useful if you want to implement a streaming type of system where you have a fixed sized buffer and want to continously process
a large string in a loop. Note that when an error is returned, the output buffer will _not_ be null terminated. You should only do this if MD_OUT_OF_MEMORY
or MD_SUCCESS is returned. MD_OUT_OF_MEMORY will be returned when the output buffer is not large enough to store the entire output string with the null
terminator.
*/
md_result md_utf8_to_utf16ne(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);
md_result md_utf8_to_utf16le(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);
md_result md_utf8_to_utf16be(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);


/*
Calculates the number of UTF-32's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as md_utf8_to_utf32ne/le/be(), except it doesn't actually do any conversion.
*/
md_result md_utf8_to_utf32_length(size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags);
md_result md_utf8_to_utf32ne_length(size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }
md_result md_utf8_to_utf32le_length(size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }
md_result md_utf8_to_utf32be_length(size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags) { return md_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags); }

/*
Converts a UTF-8 string to a UTF-32 string.
*/
md_result md_utf8_to_utf32ne(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);
md_result md_utf8_to_utf32le(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);
md_result md_utf8_to_utf32be(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags);


/*
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as md_utf16/be/le_to_utf8(), except it doesn't actually do any conversion.
*/
md_result md_utf16ne_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);
md_result md_utf16le_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);
md_result md_utf16be_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);

/*
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf16_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string.
*/
md_result md_utf16ne_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);
md_result md_utf16le_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);
md_result md_utf16be_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);

/*
Converts a UTF-16 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf16_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);


/*
Calculates the number of UTF-16's required to fully contain the given UTF-8 string after conversion, not including the null terminator.

Remarks
-------
This is the same as md_utf16/be/le_to_utf32/be/le(), except it doesn't actually do any conversion.
*/
md_result md_utf16ne_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);
md_result md_utf16le_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);
md_result md_utf16be_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);
md_result md_utf16ne_to_utf32ne_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags) { return md_utf16ne_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }
md_result md_utf16le_to_utf32le_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags) { return md_utf16le_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }
md_result md_utf16be_to_utf32be_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags) { return md_utf16be_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags); }

/*
Calculates the number of UTF-8's required to fully contain the given UTF-8 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf16_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags);

/*
Converts a UTF-16 string to a UTF-32 string.
*/
md_result md_utf16ne_to_utf32ne(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);
md_result md_utf16le_to_utf32le(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);
md_result md_utf16be_to_utf32be(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);

/*
Converts a UTF-16 string to a UTF-32 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf16_to_utf32(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags);



/*
Calculates the number of UTF-8's required to fully contain the given UTF-32 string after conversion, not including the null terminator.

Remarks
-------
This is the same as md_utf32/be/le_to_utf8(), except it doesn't actually do any conversion.
*/
md_result md_utf32ne_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);
md_result md_utf32le_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);
md_result md_utf32be_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);

/*
Calculates the number of UTF-8's required to fully contain the given UTF-32 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf32_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);

/*
Converts a UTF-32 string to a UTF-8 string.
*/
md_result md_utf32ne_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);
md_result md_utf32le_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);
md_result md_utf32be_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);

/*
Converts a UTF-32 string to a UTF-8 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf32_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);



/*
Calculates the number of UTF-16's required to fully contain the given UTF-32 string after conversion, not including the null terminator.

Remarks
-------
This is the same as md_utf32/be/le_to_utf16/be/le(), except it doesn't actually do any conversion.
*/
md_result md_utf32ne_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);
md_result md_utf32le_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);
md_result md_utf32be_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);
md_result md_utf32ne_to_utf16ne_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags) { return md_utf32ne_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }
md_result md_utf32le_to_utf16le_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags) { return md_utf32le_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }
md_result md_utf32be_to_utf16be_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags) { return md_utf32be_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags); }

/*
Calculates the number of UTF-16's required to fully contain the given UTF-32 string after conversion, not including the null terminator, with the
endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf32_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags);

/*
Converts a UTF-32 string to a UTF-16 string.
*/
md_result md_utf32ne_to_utf16ne(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);
md_result md_utf32le_to_utf16le(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);
md_result md_utf32be_to_utf16be(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);

/*
Converts a UTF-32 string to a UTF-16 string, with the endianness defined by the BOM if present, and if not, assuming native/host endian.
*/
md_result md_utf32_to_utf16(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags);



/**************************************************************************************************************************************************************

API

**************************************************************************************************************************************************************/
md_result md_init(const md_api_config* pConfig, md_api* pAPI);
void md_uninit(md_api* pAPI);


/**************************************************************************************************************************************************************

Itemization, Shaping and Placement

**************************************************************************************************************************************************************/
/*
Itemizes a UTF-8 string in preparation for shaping and drawing.

[pFont](in)
    The base font to use as the basis for font fallback. See remarks.

[pTextUTF8](in)
    A pointer to the text to itemize.

[textLength](in)
    The length in bytes (not characters or Unicode code points) of [pTextUTF8]. This can be set to (size_t)-1 in which case it will be treated as null-
    terminated. Note that this is less efficient, so if you know the length of the string ahead of time you should pass that in.

[pItems](out, optional)
    A pointer to a buffer that will receive the items. Can be NULL, in which case only the item count is returned.

[pItemCount](in, out)
    A pointer to an unsigned integer that, on input, contains the capacity of [pItems] and on output will receive the actual item count. Cannot be NULL.

[pItemizeState](out)
    A pointer to an md_itemize_state object that is used for storing backend-specific data which needs to persist until the application is done with each
    item. When you are finished with each item in the [pItems] array, you need to free this data with md_free_itemize_state(). The application need not be
    concerned with the contents of the object, nor do anything with it except free it.


Return Value
------------
MD_SUCCESS is returned on success.

MD_INVALID_ARGS will be returned if [pFont] is NULL, [pText] is NULL, [textLength] is zero or [pItemCount] is NULL.

MD_NO_SPACE will be returned when [pItems] is not large enough to contain the items. In this case, [pItemCount] will be set to the required count.

Remarks
-------
This is the first function you should call when preparing text for drawing.

Some backends will perform font fallback at this point. This is why [pFont] is required. Other backends may defer font fallback to md_shape_*().

Example
-------
```
md_itemize_state itemizeState;
md_items items[1024];
size_t itemCount = MD_COUNTOF(items);
md_result result = md_itemize_utf8(&myFont, "Hello, World!", (size_t)-1, items, &itemCount, &itemizeState);
if (result == MD_SUCCESS) {
    ... do something with items ...

    // We're done. Free the itemize state.
    md_free_itemize_state(&itemizeState);
}
```
*/
md_result md_itemize_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState);

/*
Frees the itemization state returned from md_itemize_*(). You should call this when you are finished with the items returned from md_itemize_*().
*/
void md_free_itemize_state(md_itemize_state* pItemizeState);

/*
Performs text shaping and placement on the given text run. This function is used to convert text to glyphs and calculate their relative positions.

[pFont](in)
    The font to use for the shaping.

[pItem](in, out)
    A pointer to the md_item object from a previous call to md_itemize*().

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

[pRunMetrics](out, optional)
    A pointer to a md_text_metrics object that will receive the width and height of the entire run as a whole.

Remarks
-------
You should call md_itemize_utf8() before calling this function.

You can determine the number glyphs required for the input string by calling this with [pGlyphs] set the NULL.
*/
md_result md_shape_utf8(md_font* pFont, md_item* pItem, const md_utf8* pTextUTF8, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics);


/*
Measure the length of a glyph string in pixels.
*/
md_result md_measure_x(md_api* pAPI, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32* pX);

/*
Retrieves the index of the Unicode code point under the given pixel position.
*/
md_result md_x_to_index(md_api* pAPI, const md_item* pItem, md_int32 x, size_t textLength, const size_t* pClusters, const md_glyph* pGlyphs, size_t glyphCount, size_t* pIndex, md_int32* pOffsetToEdge);

/*
Retrieves the position within a string that contains the given Unicode code point.
*/
md_result md_index_to_x(md_api* pAPI, const md_item* pItem, size_t index, size_t textLength, const size_t* pClusters, const md_glyph* pGlyphs, size_t glyphCount, md_int32* pX);


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
/*
Initializes a font.
*/
md_result md_font_init(md_api* pAPI, const md_font_config* pConfig, md_font* pFont);

/*
Uninitializes a font.

Remarks
-------
Do not uninitialize a font while it is being used in a GC.
*/
void md_font_uninit(md_font* pFont);

/*
Retrieves the metrics of the given glyphs.
*/
md_result md_font_get_glyph_metrics(md_font* pFont, const md_glyph* pGlyphs, size_t glyphCount, md_glyph_metrics* pGlyphMetrics);

/*
Retrieves the metrics of the given glyphs by their index.
*/
md_result md_font_get_glyph_metrics_by_index(md_font* pFont, const md_uint32* pGlyphIndices, size_t glyphCount, md_glyph_metrics* pGlyphMetrics);

/*
Measures a string of text.
*/
md_result md_font_get_text_metrics_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_text_metrics* pTextMetrics);


/**************************************************************************************************************************************************************

Graphics

The graphics API uses a path + fill/stroke system for drawing primitives. It is roughly based on the APIs of GDI and Cairo.

The background of a path is filled using the current brush which is set with one of the following APIs:
    - md_gc_set_fill_brush / md_gc_set_line_brush
        Sets the current brush to a md_brush object.
    - md_gc_set_fill_brush_solid / md_gc_set_line_brush_solid
        Sets the current brush to a solid color.
    - md_gc_set_fill_brush_gc / md_gc_set_line_brush_gc
        Sets the current brush to the current state of another graphics context. This is how you draw the contents of one GC to another.

Filling and stroking use their own separate brushes. The reason for this is to support the md_gc_fill_and_stroke() API. This API performs a fill, followed by
a stroke with the important property that it does not cause flickering for single buffered environments. Since this is a single API, it's useful to have
separate brushes for the fill and stroke.

The graphics API uses a state system to determine how to draw something. For example, to set the width of a line you would call md_gc_set_line_width() before
stroking the current path. The current state will remain until it's changed, unless otherwise noted. In order to simplify graphics backends, there are currently
no APIs for retrieving the current state - you will need to track this manually if required.

**************************************************************************************************************************************************************/

/******************************************************************************

Brushes

******************************************************************************/
md_result md_brush_init(md_api* pAPI, const md_brush_config* pConfig, md_brush* pBrush);

/*
Uninitializes a brush.

Remarks
-------
Do not uninitialize a brush while it is being used in a GC.
*/
void md_brush_uninit(md_brush* pBrush);
void md_brush_set_origin(md_brush* pBrush, md_int32 x, md_int32 y);

/******************************************************************************

Graphics Context

******************************************************************************/
md_result md_gc_init(md_api* pAPI, const md_gc_config* pConfig, md_gc* pGC);
void md_gc_uninit(md_gc* pGC);
md_result md_gc_get_image_data_size_in_bytes(md_gc* pGC, md_format outputFormat, size_t* pSizeInBytes);
md_result md_gc_get_image_data(md_gc* pGC, md_format outputFormat, void* pImageData);

/******************************************************************************

State Management

******************************************************************************/
md_result md_gc_get_size(md_gc* pGC, md_uint32* pSizeX, md_uint32* pSizeY);
md_result md_gc_save(md_gc* pGC);
md_result md_gc_restore(md_gc* pGC);
void md_gc_set_matrix(md_gc* pGC, const md_matrix* pMatrix);
void md_gc_get_matrix(md_gc* pGC, md_matrix* pMatrix);
void md_gc_set_matrix_identity(md_gc* pGC);
void md_gc_transform(md_gc* pGC, const md_matrix* pMatrix);
void md_gc_translate(md_gc* pGC, md_int32 offsetX, md_int32 offsetY);
void md_gc_rotate(md_gc* pGC, float rotationInRadians);
void md_gc_scale(md_gc* pGC, float scaleX, float scaleY);
void md_gc_set_miter_limit(md_gc* pGC, float limit);
void md_gc_set_line_width(md_gc* pGC, md_int32 width);
void md_gc_set_line_cap(md_gc* pGC, md_line_cap cap);
void md_gc_set_line_join(md_gc* pGC, md_line_join join);
void md_gc_set_line_dash(md_gc* pGC, const float* dashes, md_uint32 count);  /* Max value for <count> is 16. */
void md_gc_set_line_brush(md_gc* pGC, md_brush* pBrush);
void md_gc_set_line_brush_solid(md_gc* pGC, md_color color);
void md_gc_set_line_brush_gc(md_gc* pGC, md_gc* pSrcGC);
void md_gc_set_fill_brush(md_gc* pGC, md_brush* pBrush);
void md_gc_set_fill_brush_solid(md_gc* pGC, md_color color);
void md_gc_set_fill_brush_gc(md_gc* pGC, md_gc* pSrcGC);
void md_gc_set_text_fg_color(md_gc* pGC, md_color fgColor);
void md_gc_set_text_bg_color(md_gc* pGC, md_color bgColor);
md_color md_gc_get_text_fg_color(md_gc* pGC);
md_color md_gc_get_text_bg_color(md_gc* pGC);
void md_gc_set_blend_op(md_gc* pGC, md_blend_op op);
void md_gc_set_antialias_mode(md_gc* pGC, md_antialias_mode mode);
void md_gc_set_fill_mode(md_gc* pGC, md_fill_mode mode);
void md_gc_set_stretch_filter(md_gc* pGC, md_stretch_filter filter);

/******************************************************************************

Paths

******************************************************************************/
void md_gc_move_to(md_gc* pGC, md_int32 x, md_int32 y);
void md_gc_line_to(md_gc* pGC, md_int32 x, md_int32 y);
void md_gc_rectangle(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom);
void md_gc_arc(md_gc* pGC, md_int32 x, md_int32 y, md_int32 radius, float angle1InRadians, float angle2InRadians);
void md_gc_curve_to(md_gc* pGC, md_int32 x1, md_int32 y1, md_int32 x2, md_int32 y2, md_int32 x3, md_int32 y3);  /* Cubic Bézier. Starting poing is the current position. x3y3 is the end point. x1y1 and x2y2 are the control points. */
void md_gc_close_path(md_gc* pGC);

/*
Helper for defining a path for the inner border of a rectangle.
*/
void md_gc_rectangle_border_inner(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom, md_int32 borderWidth);

/*
Helper for defining a path for the outer border of a rectangle.
*/
void md_gc_rectangle_border_outer(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom, md_int32 borderWidth);

/******************************************************************************

Filling, Stroking and Clipping

******************************************************************************/
/*
Sets the clipping region to the current path as if it were filled. Anything outside of this region will be clipped, including
subsequent calls to md_gc_clip(). You can use md_gc_reset_clip() to clear the clipping region. You can also use md_gc_push()
and md_gc_pop() to save and restore clipping regions.

Use md_gc_is_point_inside_clip() to determine if a point is within the clipping region.

This will clear the path.
*/
void md_gc_clip(md_gc* pGC);

/*
Clears the clipping region.
*/
void md_gc_reset_clip(md_gc* pGC);

/*
Determines if a point is within the current clipping region.
*/
md_bool32 md_gc_is_point_inside_clip(md_gc* pGC, md_int32 x, md_int32 y);

/*
Fills the current path using the current fill settings.

This will clear the path.
*/
void md_gc_fill(md_gc* pGC);

/*
Strokes the current path using the current line settings and the current brush.

This will clear the path.
*/
void md_gc_stroke(md_gc* pGC);

/*
Fills the current path, then strokes it. The fill will never overlap the stroke.

This will clear the path.
*/
void md_gc_fill_and_stroke(md_gc* pGC);

/*
Draws the contents of the given graphics context.

This will be positioned, rotated and scaled based on the current transform. srcX and srcY are the offset inside pSrcGC to use as the origin.

The image will be clipped against the current clipping region. Use the clip to restrict the iamge to a particular subregion of the source
image or if you want to draw a non-rectangular shape.
*/
void md_gc_draw_gc(md_gc* pGC, md_gc* pSrcGC, md_int32 srcX, md_int32 srcY);

/*
Draws a glyph string using the font defined by [pItem].

Parameters
----------
[pGC](in)
    A pointer to the graphics context to draw to.

[pItem](in)
    The item that was retrieved with prior calls to md_itemize_*() and md_shape_*(). The item will be associated with a font, which is the
    font that's used to draw the glyphs.

[pGlyphs](in)
    The array of glyphs to draw.

[glyphCount](in)
    The number of glyphs in [pGlyphs].

[x](in)
    The x position to draw the glyphs.

[y](in)
    The y position to draw the glyphs. This is relative to the top left of the bounding box of the text. Offset this against the ascent of the
    font to know how to offset this value if you want to draw relative to the base line. You can use md_item_get_font_metrics() to know this.

Remarks
-------
Use md_gc_set_text_bg_color() and md_gc_set_text_fg_color() to configure the color to draw the font.

GDI/Unicode Specific: This will change the current font on the internal HDC.
*/
void md_gc_draw_glyphs(md_gc* pGC, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32 x, md_int32 y);

/*
Helper API for drawing a string of text.

Remarks
-------
This function does not handle tabs. If your text includes tabs you may want to consider using mt_gc_draw_text_layout_utf8(), or handling them
yourself using low level APIs.
*/
void md_gc_draw_text_utf8(md_gc* pGC, md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_int32 x, md_int32 y, md_alignment originAlignmentX, md_alignment originAlignmentY, md_text_metrics* pMetrics);

/*
Helper API for initializing a default text layout.
*/
md_text_layout md_text_layout_init_default();

/*
Retrieves the X/Y coordinates of the code point at the given position.
*/
void md_text_layout_cp_to_xy_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout, size_t cp, md_int32* pX, md_int32* pY);

/*
Helper API for laying out and drawing text inside a bounds.
*/
void md_gc_draw_text_layout_utf8(md_gc* pGC, md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout);

/*
Clears the surface to the given color.
*/
void md_gc_clear(md_gc* pGC, md_color color);


/******************************************************************************

Graphics Helpers

******************************************************************************/
/* Helper API for creating an RGBA color. */
MD_INLINE md_color md_rgba(md_uint8 r, md_uint8 g, md_uint8 b, md_uint8 a)
{
    md_color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

/* Helper API for creating an opaque RGB color. */
MD_INLINE md_color md_rgb(md_uint8 r, md_uint8 g, md_uint8 b)
{
    return md_rgba(r, g, b, 255);
}

/*
Copies and converts image data.

You are allowed to specify the same format for the source and target in which case this performs a direct copy with no conversion.
*/
void md_copy_image_data(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_format dstFormat, md_uint32 srcStride, md_format srcFormat);

/*
Copies the given image data and flips it.
*/
void md_copy_and_flip_image_data_y(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_format dstFormat, md_uint32 srcStride, md_format srcFormat);

/*
Flips the given image data horizontally, in place.
*/
void md_flip_image_data_y(void* pDst, md_uint32 sizeX, md_uint32 sizeY, md_uint32 stride, md_format format);

/*
Retrieves the number of bytes per pixel for the given format.
*/
md_uint32 md_get_bytes_per_pixel(md_format format);

/*
Determines whether or not there's an alpha channel in the given format.
*/
md_bool32 md_format_has_alpha(md_format format);


/**************************************************************************************************************************************************************

Utilities

**************************************************************************************************************************************************************/
/*
Checks if the given string is NULL or empty.
*/
md_bool32 md_is_null_or_empty_utf8(const md_utf8* pUTF8);
md_bool32 md_is_null_or_empty(const md_utf8* pUTF8) { return md_is_null_or_empty_utf8(pUTF8); }

/*
Checks if the given string is NULL or contains only whitespace characters.
*/
md_bool32 md_is_null_or_whitespace_utf8(const md_utf8* pUTF8);
md_bool32 md_is_null_or_whitespace(const md_utf8* pUTF8) { return md_is_null_or_whitespace_utf8(pUTF8); }

#ifdef __cplusplus
}
#endif
#endif  /* MINIDRAW_H */


/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 IMPLEMENTATION

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#if defined(MINIDRAW_IMPLEMENTATION)
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if !defined(MD_WIN32)
#include <dlfcn.h>
#endif

/* CPU Architecture */
#if defined(__x86_64__) || defined(_M_X64)
    #define MD_X64
#elif defined(__i386) || defined(_M_IX86)
    #define MD_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define MD_ARM
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1300
    #define MD_HAS_BYTESWAP16_INTRINSIC
    #define MD_HAS_BYTESWAP32_INTRINSIC
    #define MD_HAS_BYTESWAP64_INTRINSIC
#elif defined(__clang__)
    #if __has_builtin(__builtin_bswap16)
        #define MD_HAS_BYTESWAP16_INTRINSIC
    #endif
    #if __has_builtin(__builtin_bswap32)
        #define MD_HAS_BYTESWAP32_INTRINSIC
    #endif
    #if __has_builtin(__builtin_bswap64)
        #define MD_HAS_BYTESWAP64_INTRINSIC
    #endif
#elif defined(__GNUC__)
    #if ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
        #define MD_HAS_BYTESWAP32_INTRINSIC
        #define MD_HAS_BYTESWAP64_INTRINSIC
    #endif
    #if ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        #define MD_HAS_BYTESWAP16_INTRINSIC
    #endif
#endif


/* Standard library stuff. */
#ifndef MD_MALLOC
#ifdef MD_WIN32
#define MD_MALLOC(sz) HeapAlloc(GetProcessHeap(), 0, (sz))
#else
#define MD_MALLOC(sz) malloc((sz))
#endif
#endif

#ifndef MD_CALLOC
#ifdef MD_WIN32
#define MD_CALLOC(c, sz) HeapAlloc(GetProcessHeap(), 0x00000008, (c)*(sz))
#else
#define MD_CALLOC(c, sz) calloc((c), (sz))
#endif
#endif

#ifndef MD_REALLOC
#ifdef MD_WIN32
#define MD_REALLOC(p, sz) (((sz) > 0) ? ((p) ? HeapReAlloc(GetProcessHeap(), 0, (p), (sz)) : HeapAlloc(GetProcessHeap(), 0, (sz))) : ((VOID*)(size_t)(HeapFree(GetProcessHeap(), 0, (p)) & 0)))
#else
#define MD_REALLOC(p, sz) realloc((p), (sz))
#endif
#endif

#ifndef MD_FREE
#ifdef MD_WIN32
#define MD_FREE(p) HeapFree(GetProcessHeap(), 0, (p))
#else
#define MD_FREE(p) free((p))
#endif
#endif

#ifndef MD_ZERO_MEMORY
#ifdef MD_WIN32
#define MD_ZERO_MEMORY(p, sz) ZeroMemory((p), (sz))
#else
#define MD_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif
#endif

#ifndef MD_COPY_MEMORY
#ifdef MD_WIN32
#define MD_COPY_MEMORY(dst, src, sz) CopyMemory((dst), (src), (sz))
#else
#define MD_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef MD_MOVE_MEMORY
#ifdef MD_WIN32
#define MD_MOVE_MEMORY(dst, src, sz) MoveMemory((dst), (src), (sz))
#else
#define MD_MOVE_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef MD_ASSERT
#include <assert.h>
#ifdef MD_WIN32
#define MD_ASSERT(condition) assert(condition)
#else
#define MD_ASSERT(condition) assert(condition)
#endif
#endif

#define MD_ZERO_OBJECT(p)           MD_ZERO_MEMORY((p), sizeof(*(p)))
#define MD_COUNTOF(x)               (sizeof(x) / sizeof((x)[0]))
#define MD_MAX(x, y)                (((x) > (y)) ? (x) : (y))
#define MD_MIN(x, y)                (((x) < (y)) ? (x) : (y))
#define MD_CLAMP(x, lo, hi)         (MD_MAX((lo), MD_MIN((x), (hi))))
#define MD_OFFSET_PTR(p, offset)    (((md_uint8*)(p)) + (offset))



/* Although some backends may be enabled, they may not necessarily be usable (due to missing headers, compiler support, etc.). */
#if defined(MD_ENABLE_GDI) 
    #define MD_HAS_GDI          /* All relevant Windows platforms should support GDI + Uniscribe. */
#endif
#if defined(MD_ENABLE_DIRECT2D)
    #define MD_HAS_DIRECT2D     /* Headers are required for Direct2D + DirectWrite for now. TODO: Remove this dependency. */
    #if defined(__has_include)
        #if !__has_include(<d2d1.h>) || !__has_include(<dwrite.h>)
            #undef MD_HAS_DIRECT2D
        #endif
    #endif
#endif
#if defined(MD_ENABLE_COREGRAPHICS)
    #define MD_HAS_COREGRAPHICS /* All relevant Apple platforms should support Core Graphics and Core Text. */
#endif
#if defined(MD_ENABLE_CAIRO)
    #define MD_HAS_CAIRO        /* Headers are required for Cairo + Pango. */
    #if defined(__has_include)
        #if !__has_include(<pango/pangocairo.h>)
            #undef MD_HAS_CAIRO
        #endif
    #endif
#endif
#if defined(MD_ENABLE_XFT)
    #define MD_HAS_XFT          /* Some headers are required for XFT. */
    #if defined(__has_include)
        #if !__has_include(<ft2build.h>) || !__has_include(<X11/Xft/Xft.h>) || !__has_include(<harfbuzz/hb-ft.h>)
            #undef MD_HAS_XFT
        #endif
    #endif
#endif

#if defined(MD_WIN32)
#include <windows.h>

md_result md_result_from_win32_error(DWORD error)
{
    switch (error)
    {
        case ERROR_SUCCESS:             return MD_SUCCESS;
        case ERROR_PATH_NOT_FOUND:      return MD_DOES_NOT_EXIST;
        case ERROR_TOO_MANY_OPEN_FILES: return MD_TOO_MANY_OPEN_FILES;
        case ERROR_NOT_ENOUGH_MEMORY:   return MD_OUT_OF_MEMORY;
        case ERROR_DISK_FULL:           return MD_NO_SPACE;
        case ERROR_HANDLE_EOF:          return MD_END_OF_FILE;
        case ERROR_NEGATIVE_SEEK:       return MD_NEGATIVE_SEEK;
        case ERROR_INVALID_PARAMETER:   return MD_INVALID_ARGS;
        case ERROR_ACCESS_DENIED:       return MD_ACCESS_DENIED;
        case ERROR_SEM_TIMEOUT:         return MD_TIMEOUT;
        case ERROR_FILE_NOT_FOUND:      return MD_DOES_NOT_EXIST;
        default: break;
    }

    return MD_ERROR;
}

md_result md_result_from_HRESULT(HRESULT hr)
{
    switch (hr)
    {
        case S_OK:          return MD_SUCCESS;
        case E_INVALIDARG:  return MD_INVALID_ARGS;
        case E_OUTOFMEMORY: return MD_OUT_OF_MEMORY;
        default: break;
    }

    if (SUCCEEDED(hr)) {
        return MD_SUCCESS;
    } else {
        return MD_ERROR;
    }
}
#endif


static MD_INLINE md_bool32 md_is_little_endian()
{
#if defined(MD_X86) || defined(MD_X64)
    return MD_TRUE;
#elif defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN
    return MD_TRUE;
#else
    int n = 1;
    return (*(char*)&n) == 1;
#endif
}

static MD_INLINE md_bool32 md_is_big_endian()
{
    return !md_is_little_endian();
}

static MD_INLINE md_uint16 md_swap_endian_uint16(md_uint16 n)
{
#ifdef MD_HAS_BYTESWAP16_INTRINSIC
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

static MD_INLINE md_uint32 md_swap_endian_uint32(md_uint32 n)
{
#ifdef MD_HAS_BYTESWAP32_INTRINSIC
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


static MD_INLINE md_uint16 md_be2host_16(md_uint16 n)
{
    if (md_is_little_endian()) {
        return md_swap_endian_uint16(n);
    }

    return n;
}

static MD_INLINE md_uint32 md_be2host_32(md_uint32 n)
{
    if (md_is_little_endian()) {
        return md_swap_endian_uint32(n);
    }

    return n;
}

static MD_INLINE md_uint16 md_le2host_16(md_uint16 n)
{
    if (!md_is_little_endian()) {
        return md_swap_endian_uint16(n);
    }

    return n;
}

static MD_INLINE md_uint32 md_le2host_32(md_uint32 n)
{
    if (!md_is_little_endian()) {
        return md_swap_endian_uint32(n);
    }

    return n;
}


static MD_INLINE md_uint16 md_host2be_16(md_uint16 n)
{
    if (md_is_little_endian()) {
        return md_swap_endian_uint16(n);
    }

    return n;
}

static MD_INLINE md_uint32 md_host2be_32(md_uint32 n)
{
    if (md_is_little_endian()) {
        return md_swap_endian_uint32(n);
    }

    return n;
}

static MD_INLINE md_uint16 md_host2le_16(md_uint16 n)
{
    if (!md_is_little_endian()) {
        return md_swap_endian_uint16(n);
    }

    return n;
}

static MD_INLINE md_uint32 md_host2le_32(md_uint32 n)
{
    if (!md_is_little_endian()) {
        return md_swap_endian_uint32(n);
    }

    return n;
}

md_result md_result_from_errno(int e)
{
    switch (e) {
        case EACCES: return MD_ACCESS_DENIED;
        case EEXIST: return MD_ALREADY_EXISTS;
        case EINVAL: return MD_INVALID_ARGS;
        case EMFILE: return MD_TOO_MANY_OPEN_FILES;
        case ENOENT: return MD_DOES_NOT_EXIST;
        case ENOMEM: return MD_OUT_OF_MEMORY;
        case EBADF:  return MD_INVALID_ARGS;   /* Can be returned by ftell() and family. */
        default: break;
    }

    return MD_ERROR;
}


md_handle md_dlopen(const char* filename)
{
    md_handle handle;

#ifdef _WIN32
    handle = (md_handle)LoadLibraryA(filename);
#else
    handle = (md_handle)dlopen(filename, RTLD_NOW);
#endif

    return handle;
}

void md_dlclose(md_handle handle)
{
#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose((void*)handle);
#endif
}

md_proc md_dlsym(md_handle handle, const char* symbol)
{
    md_proc proc;

#ifdef _WIN32
    proc = (md_proc)GetProcAddress((HMODULE)handle, symbol);
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif
    proc = (md_proc)dlsym((void*)handle, symbol);
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #pragma GCC diagnostic pop
#endif
#endif

    return proc;
}



MD_INLINE int md_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#ifdef _MSC_VER
    return strncpy_s(dst, dstSizeInBytes, src, count);
#else
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return 22;  /* EINVAL */
    }
    if (dstSizeInBytes == 0) {
        return 34;  /* ERANGE */
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;  /* EINVAL */
    }

    maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        /* -1 = _TRUNCATE */
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
    return 34;  /* ERANGE */
#endif
}



md_bool32 md_is_newline_utf8(const md_utf8* pTextUTF8, size_t textLength)
{
    if (pTextUTF8 == NULL || textLength == 0) {
        return MD_FALSE;
    }

    if (textLength == (size_t)-1) {
        /* Null terminated. */
        if (pTextUTF8[0] == '\n' && pTextUTF8[1] == '\0') {
            return MD_TRUE;
        }
        
        if (pTextUTF8[0] == '\r' && pTextUTF8[1] == '\0') {
            return MD_TRUE;
        }

        if (pTextUTF8[0] == '\r' && pTextUTF8[1] == '\n' && pTextUTF8[2] == '\0') {
            return MD_TRUE;
        }
    } else {
        /* Not null terminated. */
        if (textLength == 1) {
            return pTextUTF8[0] == '\r' || pTextUTF8[0] == '\n';
        }

        if (textLength == 2) {
            return pTextUTF8[0] == '\r' && pTextUTF8[1] == '\n';
        }
    }

    return MD_FALSE;
}

md_bool32 md_is_tab_utf8(const md_utf8* pTextUTF8, size_t textLength)
{
    if (pTextUTF8 == NULL || textLength == 0) {
        return MD_FALSE;
    }

    return pTextUTF8[0] == '\t';
}

/*
Retrieves the beginning of the next line in the string.

Remarks
-------
When [textLength] is (size_t)-1 it is assumed that pTextUTF8 is null terminated. When [textLength] is not (size_t)-1 it's assumed that
[pTextUTF8] is of the length [textLength]. In this case null terminators will _not_ terminate the line.

The output value of [ppLineEndUTF8] is always 1 character past the end of the line. Therefore, [ppLineEndUTF8] minus [pTextUTF8] will
equal the length of the line.
*/
const md_utf8* md_next_line_utf8(const md_utf8* pTextUTF8, size_t textLength, const md_utf8** ppLineEndUTF8)
{
    const md_utf8* pLineEndUTF8 = pTextUTF8;
    const md_utf8* pNextLineBegUTF8 = NULL;

    if (ppLineEndUTF8 != NULL) {
        *ppLineEndUTF8 = pTextUTF8;
    }

    if (pTextUTF8 == NULL) {
        return NULL;
    }

    if (textLength == 0) {
        return NULL;
    }

    if (textLength == (size_t)-1) {
        size_t iByte = 0;
        for (;;) {
            if (pTextUTF8[iByte] == '\0') {
                break;  /* Reached the end of the string. */
            }

            if (pTextUTF8[iByte] == '\n') {
                pLineEndUTF8 = pTextUTF8 + iByte;
                pNextLineBegUTF8 = pLineEndUTF8 + 1;
                break;
            }

            if (pTextUTF8[iByte] == '\r') {
                pLineEndUTF8 = pTextUTF8 + iByte;
                pNextLineBegUTF8 = pLineEndUTF8 + 1;

                /* Check for \r\n. */
                if (iByte+1 < textLength) {
                    if (pTextUTF8[iByte+1] == '\n') {
                        pNextLineBegUTF8 += 1;
                    }
                }
                
                break;
            }

            iByte += 1;
        }

        if (pNextLineBegUTF8 == NULL) {
            /* A new line character was not found. There is no next line. */
            pLineEndUTF8 = pTextUTF8 + iByte;
        }
    } else {
        size_t iByte;
        for (iByte = 0; iByte < textLength; /* Do nothing. */) {
            if (pTextUTF8[iByte] == '\n') {
                pLineEndUTF8 = pTextUTF8 + iByte;
                pNextLineBegUTF8 = pLineEndUTF8 + 1;
                break;
            }

            if (pTextUTF8[iByte] == '\r') {
                pLineEndUTF8 = pTextUTF8 + iByte;
                pNextLineBegUTF8 = pLineEndUTF8 + 1;

                /* Check for \r\n. */
                if (iByte+1 < textLength) {
                    if (pTextUTF8[iByte+1] == '\n') {
                        pNextLineBegUTF8 += 1;
                    }
                }
                
                break;
            }

            iByte += 1;
        }

        if (pNextLineBegUTF8 == NULL) {
            /* A new line character was not found. There is no next line. */
            pLineEndUTF8 = pTextUTF8 + textLength;
        }
    }

    if (ppLineEndUTF8 != NULL) {
        *ppLineEndUTF8 = pLineEndUTF8;
    }

    return pNextLineBegUTF8;
}

const md_utf8* md_next_tabbed_segment(const md_utf8* pTextUTF8, size_t textLength, const md_utf8** ppSegmentEndUTF8)
{
    const md_utf8* pSegmentEndUTF8 = pTextUTF8;
    const md_utf8* pNextSegmentBegUTF8 = NULL;

    if (ppSegmentEndUTF8 != NULL) {
        *ppSegmentEndUTF8 = pTextUTF8;
    }

    if (pTextUTF8 == NULL) {
        return NULL;
    }

    if (textLength == 0) {
        return NULL;
    }

    if (textLength == (size_t)-1) {
        size_t iByte = 0;
        for (;;) {
            if (pTextUTF8[iByte] == '\0') {
                break;  /* Reached the end of the string. */
            }

            if (pTextUTF8[iByte] == '\t') {
                pSegmentEndUTF8 = pTextUTF8 + iByte;
                pNextSegmentBegUTF8 = pSegmentEndUTF8 + 1;

                /* Get past the entire tab group. */
                iByte += 1;
                while (pTextUTF8[iByte] == '\t') {
                    pNextSegmentBegUTF8 += 1;
                    iByte += 1;
                }

                break;
            }

            iByte += 1;
        }

        if (pNextSegmentBegUTF8 == NULL) {
            /* A tab character was not found. There is no next line. */
            pSegmentEndUTF8 = pTextUTF8 + iByte;
        }
    } else {
        size_t iByte;
        for (iByte = 0; iByte < textLength; /* Do nothing. */) {
            if (pTextUTF8[iByte] == '\t') {
                pSegmentEndUTF8 = pTextUTF8 + iByte;
                pNextSegmentBegUTF8 = pSegmentEndUTF8 + 1;

                /* Get past the entire tab group. */
                iByte += 1;
                while (iByte < textLength && pTextUTF8[iByte] == '\t') {
                    pNextSegmentBegUTF8 += 1;
                    iByte += 1;
                }

                break;
            }

            iByte += 1;
        }

        if (pNextSegmentBegUTF8 == NULL) {
            /* A tab character was not found. There is no next line. */
            pSegmentEndUTF8 = pTextUTF8 + textLength;
        }
    }

    if (ppSegmentEndUTF8 != NULL) {
        *ppSegmentEndUTF8 = pSegmentEndUTF8;
    }

    return pNextSegmentBegUTF8;
}


/**************************************************************************************************************************************************************

Unicode

**************************************************************************************************************************************************************/
MD_INLINE md_bool32 md_is_invalid_utf8_octet(md_utf8 utf8)
{
    /* RFC 3629 - Section 1: The octet values C0, C1, F5 to FF never appear. */
    return (md_uint8)utf8 == 0xC0 || (md_uint8)utf8 == 0xC1 || (md_uint8)utf8 >= 0xF5;
}

MD_INLINE void md_utf32_cp_to_utf16_pair(md_utf32 utf32cp, md_utf16* pUTF16)
{
    /* RFC 2781 - Section 2.1 */
    md_utf32 u;

    MD_ASSERT(utf32cp >= 0x10000);

    u = utf32cp - 0x10000;
    pUTF16[0] = (md_utf16)(0xD800 | ((u & 0xFFC00) >> 10));
    pUTF16[1] = (md_utf16)(0xDC00 | ((u & 0x003FF) >>  0));
}

MD_INLINE md_utf32 md_utf16_pair_to_utf32_cp(const md_utf16* pUTF16)
{
    /* RFC 2781 - Section 2.1 */
    MD_ASSERT(pUTF16 != NULL);

    return (((md_utf32)(pUTF16[0] & 0x003FF) << 10) | ((md_utf32)(pUTF16[1] & 0x003FF) << 0)) + 0x10000;
}

MD_INLINE md_bool32 md_is_cp_in_surrogate_pair_range(md_utf32 utf32)
{
    return utf32 >= 0xD800 && utf32 <= 0xDFFF;
}

MD_INLINE md_bool32 md_is_valid_code_point(md_utf32 utf32)
{
    return utf32 <= MD_UNICODE_MAX_CODE_POINT && !md_is_cp_in_surrogate_pair_range(utf32);
}

MD_INLINE size_t md_utf32_cp_to_utf8_length(md_utf32 utf32)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MD_ASSERT(utf32 <= MD_UNICODE_MAX_CODE_POINT);
    MD_ASSERT(md_is_cp_in_surrogate_pair_range(utf32) == MD_FALSE);

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
    MD_ASSERT(MD_FALSE);
    return 0; /* Invalid. */
}

MD_INLINE size_t md_utf32_cp_to_utf8(md_utf32 utf32, md_utf8* pUTF8, size_t utf8Cap)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MD_ASSERT(utf32 <= MD_UNICODE_MAX_CODE_POINT);
    MD_ASSERT(md_is_cp_in_surrogate_pair_range(utf32) == MD_FALSE);
    MD_ASSERT(utf8Cap > 1);

    if (utf32 <= 0x7F) {
        if (utf8Cap >= 1) {
            pUTF8[0] = (utf32 & 0x7F);
            return 1;
        }
    }
    if (utf32 <= 0x7FF) {
        if (utf8Cap >= 2) {
            pUTF8[0] = 0xC0 | (md_utf8)((utf32 & 0x07C0) >> 6);
            pUTF8[1] = 0x80 | (md_utf8) (utf32 & 0x003F);
            return 2;
        }
    }
    if (utf32 <= 0xFFFF) {
        if (utf8Cap >= 3) {
            pUTF8[0] = 0xE0 | (md_utf8)((utf32 & 0xF000) >> 12);
            pUTF8[1] = 0x80 | (md_utf8)((utf32 & 0x0FC0) >>  6);
            pUTF8[2] = 0x80 | (md_utf8) (utf32 & 0x003F);
            return 3;
        }
    }
    if (utf32 <= 0x10FFFF) {
        if (utf8Cap >= 4) {
            pUTF8[0] = 0xF0 | (md_utf8)((utf32 & 0x1C0000) >> 18);
            pUTF8[1] = 0x80 | (md_utf8)((utf32 & 0x03F000) >> 12);
            pUTF8[2] = 0x80 | (md_utf8)((utf32 & 0x000FC0) >>  6);
            pUTF8[3] = 0x80 | (md_utf8) (utf32 & 0x00003F);
            return 4;
        }
    }

    /* Getting here means there was not enough room in the output buffer. */
    return 0;
}

MD_INLINE size_t md_utf32_cp_to_utf16_length(md_utf32 utf32)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MD_ASSERT(utf32 <= MD_UNICODE_MAX_CODE_POINT);
    MD_ASSERT(md_is_cp_in_surrogate_pair_range(utf32) == MD_FALSE);

    if (utf32 <= 0xFFFF) {
        return 1;
    } else {
        return 2;
    }

    /* Unreachable. */
#if 0
    /* Invalid. This function assume's the UTF-32 code point is valid so do an assert. */
    MD_ASSERT(MD_FALSE);
    return 0; /* Invalid. */
#endif
}

MD_INLINE size_t md_utf32_cp_to_utf16(md_utf32 utf32, md_utf16* pUTF16, size_t utf16Cap)
{
    /* This API assumes the the UTF-32 code point is valid. */
    MD_ASSERT(utf32 <= MD_UNICODE_MAX_CODE_POINT);
    MD_ASSERT(md_is_cp_in_surrogate_pair_range(utf32) == MD_FALSE);
    MD_ASSERT(utf16Cap > 1);

    if (utf32 <= 0xFFFF) {
        if (utf16Cap >= 1) {
            pUTF16[0] = (md_utf16)utf32;
            return 1;
        }
    } else {
        if (utf16Cap >= 2) {
            md_utf32_cp_to_utf16_pair(utf32, pUTF16);
            return 2;
        }
    }

    /* Getting here means there was not enough room in the output buffer. */
    return 0;
}


void md_swap_endian_utf16(md_utf16* pUTF16, size_t count)
{
    if (count == (size_t)-1) {
        size_t i;
        for (i = 0; i < count; ++i) {
            pUTF16[i] = md_swap_endian_uint16(pUTF16[i]);
        }
    } else {
        while (pUTF16[0] != 0) {
            pUTF16[0] = md_swap_endian_uint16(pUTF16[0]);
            pUTF16 += 1;
        }
    }
}

void md_swap_endian_utf32(md_utf32* pUTF32, size_t count)
{
    if (count == (size_t)-1) {
        size_t i;
        for (i = 0; i < count; ++i) {
            pUTF32[i] = md_swap_endian_uint32(pUTF32[i]);
        }
    } else {
        while (pUTF32[0] != 0) {
            pUTF32[0] = md_swap_endian_uint32(pUTF32[0]);
            pUTF32 += 1;
        }
    }
}


md_result md_utf8_to_utf16_length(size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags)
{
    md_result result = MD_SUCCESS;
    size_t utf16Len = 0;

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;   /* Invalid input. */
    }

    if (utf8Len == 0 || pUTF8[0] == 0) {
        return MD_SUCCESS;   /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf8_bom((const md_uint8*)pUTF8, utf8Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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

            if ((md_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                pUTF8    += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16. */
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        pUTF8    += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        md_uint32 cp;
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        cp = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!md_is_valid_code_point(cp)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                pUTF8    += 4;
                            }
                        } else {
                            utf16Len += 2;  /* Must be at least 2 UTF-16s */
                            pUTF8    += 4;
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
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
            if ((md_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                utf16Len += 1;
                iUTF8    += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 > utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 > utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        utf16Len += 1;  /* Can be at most 1 UTF-16.*/
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        md_uint32 cp;
                        if (iUTF8+3 > utf8Len) {
                            return MD_INVALID_ARGS;
                        }

                        cp = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!md_is_valid_code_point(cp)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                iUTF8    += 4;
                            }
                        } else {
                            utf16Len += 2;  /* Must be at least 2 UTF-16s */
                            iUTF8    += 4;
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            utf16Len += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
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

md_result md_utf8_to_utf16ne(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result = MD_SUCCESS;
    size_t utf16CapOriginal = utf16Cap;

    if (pUTF16 == NULL) {
        return md_utf8_to_utf16_length(pUTF16Len, pUTF8, utf8Len, flags);
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf8_bom((const md_uint8*)pUTF8, utf8Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf8* pUTF8Original = pUTF8;
        while (pUTF8[0] != 0) {
            if (utf16Cap == 1) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if ((md_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[0];
                pUTF16   += 1;
                utf16Cap -= 1;
                pUTF8    += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        pUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF16[0] = ((md_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        pUTF8    += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF16[0] = ((md_utf16)(pUTF8[0] & 0x0F) << 12) | ((md_utf16)(pUTF8[1] & 0x3F) << 6) | (pUTF8[2] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        pUTF8    += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (utf16Cap < 2) {
                            break;  /* No enough room. */
                        } else {
                            md_uint32 cp;
                            if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                                result = MD_INVALID_ARGS; /* Input string is too short. */
                                break;
                            }

                            cp = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                            if (!md_is_valid_code_point(cp)) {
                                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    result = MD_INVALID_CODE_POINT;
                                    break;
                                } else {
                                    /* Replacement. */
                                    pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                                    pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    pUTF8    += 4;
                                }
                            } else {
                                md_utf32_cp_to_utf16_pair(cp, pUTF16);
                                pUTF16   += 2;
                                utf16Cap -= 2;
                                pUTF8    += 4;
                            }
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
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
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if ((md_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF16[0] = pUTF8[iUTF8+0];
                pUTF16   += 1;
                utf16Cap -= 1;
                iUTF8    += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                        iUTF8    += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 > utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        pUTF16[0] = ((md_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 > utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        pUTF16[0] = ((md_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((md_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        pUTF16   += 1;
                        utf16Cap -= 1;
                        iUTF8    += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (utf16Cap < 2) {
                            break;  /* No enough room. */
                        } else {
                            md_uint32 cp;
                            if (iUTF8+3 > utf8Len) {
                                result = MD_INVALID_ARGS;
                                break;
                            }

                            cp = ((md_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((md_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((md_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                            if (!md_is_valid_code_point(cp)) {
                                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                    result = MD_INVALID_CODE_POINT;
                                    break;
                                } else {
                                    /* Replacement. */
                                    pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                                    pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                                    iUTF8    += 4;
                                }
                            } else {
                                md_utf32_cp_to_utf16_pair(cp, pUTF16);
                                pUTF16   += 2;
                                utf16Cap -= 2;
                                iUTF8    += 4;
                            }
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            pUTF16[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF16   += MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
                            utf16Cap -= MD_UNICODE_REPLACEMENT_CHARACTER_LENGTH_UTF16;
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
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF16[0] = 0;
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = (utf16CapOriginal - utf16Cap);
    }

    return result;
}

md_result md_utf8_to_utf16le(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = md_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    if (result != MD_SUCCESS) {
        return result;
    }

    if (pUTF16 != NULL && !md_is_little_endian()) {
        md_swap_endian_utf16(pUTF16, utf16Len);
    }

    return MD_SUCCESS;
}

md_result md_utf8_to_utf16be(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result;
    size_t utf16Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = md_utf8_to_utf16ne(pUTF16, utf16Cap, &utf16Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    if (result != MD_SUCCESS) {
        return result;
    }

    if (pUTF16 != NULL && !md_is_big_endian()) {
        md_swap_endian_utf16(pUTF16, utf16Len);
    }

    return MD_SUCCESS;
}



md_result md_utf8_to_utf32_length(size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, md_uint32 flags)
{
    md_result result = MD_SUCCESS;
    size_t utf32Len = 0;

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;   /* Invalid input. */
    }

    if (utf8Len == 0 || pUTF8[0] == 0) {
        return MD_SUCCESS;   /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf8_bom((const md_uint8*)pUTF8, utf8Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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
            if ((md_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF8 += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        md_uint32 cp;
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }
                        cp = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!md_is_valid_code_point(cp)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                            }
                        }
                        pUTF8 += 4;
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
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
            if ((md_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                iUTF8 += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        md_uint32 cp;
                        if (iUTF8+3 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }
                        cp = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        if (!md_is_valid_code_point(cp)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                            }
                        }

                        iUTF8 += 4;
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
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

md_result md_utf8_to_utf32ne(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result = MD_SUCCESS;
    size_t utf32CapOriginal = utf32Cap;

    if (pUTF32 == NULL) {
        return md_utf8_to_utf32_length(pUTF32Len, pUTF8, utf8Len, flags);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf8_bom((const md_uint8*)pUTF8, utf8Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF8 += 3; /* Skip past the BOM. */
        if (utf8Len != (size_t)-1) {
            utf8Len -= 3;
        }
    }

    if (utf8Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf8* pUTF8Original = pUTF8;
        while (pUTF8[0] != 0) {
            if (utf32Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if ((md_uint8)pUTF8[0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[0];
                pUTF8 += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                        pUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[0] & 0xE0) == 0xC0) {
                        if (pUTF8[1] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((md_utf16)(pUTF8[0] & 0x1F) <<  6) | (pUTF8[1] & 0x3F);
                        pUTF8 += 2;
                    } else if ((pUTF8[0] & 0xF0) == 0xE0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((md_utf16)(pUTF8[0] & 0x0F) << 12) | ((md_utf16)(pUTF8[1] & 0x3F) << 6) | (pUTF8[2] & 0x3F);
                        pUTF8 += 3;
                    } else if ((pUTF8[0] & 0xF8) == 0xF0) {
                        if (pUTF8[1] == 0 || pUTF8[2] == 0 || pUTF8[3] == 0) {
                            result = MD_INVALID_ARGS; /* Input string is too short. */
                            break;
                        }

                        pUTF32[0] = ((md_utf32)(pUTF8[0] & 0x07) << 18) | ((md_utf32)(pUTF8[1] & 0x3F) << 12) | ((md_utf32)(pUTF8[2] & 0x3F) << 6) | (pUTF8[3] & 0x3F);
                        pUTF8 += 4;

                        if (!md_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;   /* No characters should be in the UTF-16 surrogate pair range. */
                                break;
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            pUTF8 += 1;
                        }
                    }
                }
            }

            pUTF32   += 1;
            utf32Cap -= 1;
        }

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = (pUTF8 - pUTF8Original);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF8;
        for (iUTF8 = 0; iUTF8 < utf8Len; /* Do nothing */) {
            if (utf32Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if ((md_uint8)pUTF8[iUTF8+0] < 128) {   /* ASCII character. */
                pUTF32[0] = pUTF8[iUTF8+0];
                iUTF8 += 1;
            } else {
                if (md_is_invalid_utf8_octet(pUTF8[iUTF8+0])) {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                        iUTF8 += 1;
                    }
                } else {
                    if ((pUTF8[iUTF8+0] & 0xE0) == 0xC0) {
                        if (iUTF8+1 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((md_utf16)(pUTF8[iUTF8+0] & 0x1F) <<  6) | (pUTF8[iUTF8+1] & 0x3F);
                        iUTF8 += 2;
                    } else if ((pUTF8[iUTF8+0] & 0xF0) == 0xE0) {
                        if (iUTF8+2 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((md_utf16)(pUTF8[iUTF8+0] & 0x0F) << 12) | ((md_utf16)(pUTF8[iUTF8+1] & 0x3F) << 6) | (pUTF8[iUTF8+2] & 0x3F);
                        iUTF8 += 3;
                    } else if ((pUTF8[iUTF8+0] & 0xF8) == 0xF0) {
                        if (iUTF8+3 >= utf8Len) {
                            result = MD_INVALID_ARGS;
                            break;
                        }

                        pUTF32[0] = ((md_utf32)(pUTF8[iUTF8+0] & 0x07) << 18) | ((md_utf32)(pUTF8[iUTF8+1] & 0x3F) << 12) | ((md_utf32)(pUTF8[iUTF8+2] & 0x3F) << 6) | (pUTF8[iUTF8+3] & 0x3F);
                        iUTF8 += 4;

                        if (!md_is_valid_code_point(pUTF32[0])) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }
                    } else {
                        if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                            result = MD_INVALID_CODE_POINT;
                            break;
                        } else {
                            /* Replacement. */
                            pUTF32[0] = MD_UNICODE_REPLACEMENT_CHARACTER;
                            iUTF8 += 1;
                        }
                    }
                }
            }

            pUTF32   += 1;
            utf32Cap -= 1;
        }

        if (pUTF8LenProcessed != NULL) {
            *pUTF8LenProcessed = iUTF8;
        }
    }

    /* Null terminate. */
    if (utf32Cap == 0) {
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF32[0] = 0;
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = (utf32CapOriginal - utf32Cap);
    }

    return result;
}

md_result md_utf8_to_utf32le(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = md_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    if (result != MD_SUCCESS) {
        return result;
    }

    if (pUTF32 != NULL && !md_is_little_endian()) {
        md_swap_endian_utf32(pUTF32, utf32Len);
    }

    return MD_SUCCESS;
}

md_result md_utf8_to_utf32be(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf8* pUTF8, size_t utf8Len, size_t* pUTF8LenProcessed, md_uint32 flags)
{
    md_result result;
    size_t utf32Len;

    /* Always do a native endian conversion first, then byte swap if necessary. */
    result = md_utf8_to_utf32ne(pUTF32, utf32Cap, &utf32Len, pUTF8, utf8Len, pUTF8LenProcessed, flags);

    if (pUTF32Len != NULL) {
        *pUTF32Len = utf32Len;
    }

    if (result != MD_SUCCESS) {
        return result;
    }

    if (pUTF32 != NULL && !md_is_big_endian()) {
        md_swap_endian_utf32(pUTF32, utf32Len);
    }

    return MD_SUCCESS;
}



md_result md_utf16_to_utf8_length_internal(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf8Len = 0;
    md_utf16 w1;
    md_utf16 w2;
    md_utf32 utf32;

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF16 == NULL) {
        return MD_INVALID_ARGS; /* Invalid input. */
    }

    if (utf16Len == 0 || pUTF16[0] == 0) {
        return MD_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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
                w1 = md_le2host_16(pUTF16[0]);
            } else {
                w1 = md_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            utf8Len += md_utf32_cp_to_utf8_length(utf32);
        }
    } else {
        /* Fixed length string. */
        size_t iUTF16;
        for (iUTF16 = 0; iUTF16 < utf16Len; /* Do nothing */) {
            if (isLE) {
                w1 = md_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = md_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 > utf16Len) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            utf8Len += md_utf32_cp_to_utf8_length(utf32);
        }
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = utf8Len;
    }

    return result;
}

md_result md_utf16ne_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf16le_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    } else {
        return md_utf16be_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    }
}

md_result md_utf16le_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    return md_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, MD_TRUE);
}

md_result md_utf16be_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    return md_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, MD_FALSE);
}

md_result md_utf16_to_utf8_length(size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf16_bom_le((const md_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            return md_utf16le_to_utf8_length(pUTF8Len, pUTF16+1, utf16Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return md_utf16be_to_utf8_length(pUTF8Len, pUTF16+1, utf16Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf16ne_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
}


md_result md_utf16_to_utf8_internal(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf8CapOriginal = utf8Cap;
    md_utf16 w1;
    md_utf16 w2;
    md_utf32 utf32;
    size_t utf8cpLen;   /* Code point length in UTF-8 code units. */

    if (pUTF8 == NULL) {
        return md_utf16_to_utf8_length_internal(pUTF8Len, pUTF16, utf16Len, flags, isLE);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }
    }

    if (utf16Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf16* pUTF16Original = pUTF16;
        while (pUTF16[0] != 0) {
            if (utf8Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                w1 = md_le2host_16(pUTF16[0]);
            } else {
                w1 = md_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            utf8cpLen = md_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
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
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                w1 = md_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = md_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 > utf16Len) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            utf8cpLen = md_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
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
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF8[0] = 0;
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = (utf8CapOriginal - utf8Cap);
    }

    return result;
}

md_result md_utf16ne_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    } else {
        return md_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    }
}

md_result md_utf16le_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    return md_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MD_TRUE);
}

md_result md_utf16be_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    return md_utf16_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MD_FALSE);
}

md_result md_utf16_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    if (pUTF8 == NULL) {
        return md_utf16_to_utf8_length(pUTF8Len, pUTF16, utf16Len, flags);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        md_result result;
        size_t utf16LenProcessed;

        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf16_bom_le((const md_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            result = md_utf16le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = md_utf16be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = utf16LenProcessed + 1;    /* +1 for BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf16ne_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
}


md_result md_utf16_to_utf32_length_internal(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf32Len = 0;
    md_utf16 w1;
    md_utf16 w2;

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF16 == NULL) {
        return MD_INVALID_ARGS; /* Invalid input. */
    }

    if (utf16Len == 0 || pUTF16[0] == 0) {
        return MD_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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
                w1 = md_le2host_16(pUTF16[0]);
            } else {
                w1 = md_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[1]);
                        }
                    
                        /* Check for invalid code point. */
                        if (!(w2 >= 0xDC00 && w2 <= 0xDFFF)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
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
                w1 = md_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = md_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 > utf16Len) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        /* Check for invalid code point. */
                        if (!(w2 >= 0xDC00 && w2 <= 0xDFFF)) {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
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

md_result md_utf16ne_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf16le_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    } else {
        return md_utf16be_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    }
}

md_result md_utf16le_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    return md_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, MD_TRUE);
}

md_result md_utf16be_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    return md_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, MD_FALSE);
}

md_result md_utf16_to_utf32_length(size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, md_uint32 flags)
{
    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf16_bom_le((const md_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            return md_utf16le_to_utf32_length(pUTF32Len, pUTF16+1, utf16Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return md_utf16be_to_utf32_length(pUTF32Len, pUTF16+1, utf16Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf16ne_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
}


md_result md_utf16_to_utf32_internal(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf32CapOriginal = utf32Cap;
    md_utf16 w1;
    md_utf16 w2;
    md_utf32 utf32;

    if (pUTF32 == NULL) {
        return md_utf16_to_utf32_length_internal(pUTF32Len, pUTF16, utf16Len, flags, isLE);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF32 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }
    }

    if (utf16Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf16* pUTF16Original = pUTF16;
        while (pUTF16[0] != 0) {
            if (utf32Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                w1 = md_le2host_16(pUTF16[0]);
            } else {
                w1 = md_be2host_16(pUTF16[0]);
            }
            
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                pUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (pUTF16[1] == 0) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        pUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    pUTF16 += 1;
                }
            }

            if (isLE) {
                pUTF32[0] = md_host2le_32(utf32);
            } else {
                pUTF32[0] = md_host2be_32(utf32);
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
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                w1 = md_le2host_16(pUTF16[iUTF16+0]);
            } else {
                w1 = md_be2host_16(pUTF16[iUTF16+0]);
            }

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                /* 1 UTF-16 code unit. */
                utf32 = w1;
                iUTF16 += 1;
            } else {
                /* 2 UTF-16 code units, or an error. */
                if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                    if (iUTF16+1 > utf16Len) {
                        result = MD_INVALID_ARGS; /* Ran out of input data. */
                        break;
                    } else {
                        if (isLE) {
                            w2 = md_le2host_16(pUTF16[iUTF16+1]);
                        } else {
                            w2 = md_be2host_16(pUTF16[iUTF16+1]);
                        }
                    
                        if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                        } else {
                            if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                                result = MD_INVALID_CODE_POINT;
                                break;
                            } else {
                                /* Replacement. */
                                utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                            }
                        }

                        iUTF16 += 2;
                    }
                } else {
                    if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                        result = MD_INVALID_CODE_POINT;
                        break;
                    } else {
                        /* Replacement. */
                        utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                    }

                    iUTF16 += 1;
                }
            }

            if (isLE) {
                pUTF32[0] = md_host2le_32(utf32);
            } else {
                pUTF32[0] = md_host2be_32(utf32);
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
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF32[0] = 0;
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = (utf32CapOriginal - utf32Cap);
    }

    return result;
}

md_result md_utf16ne_to_utf32ne(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf16le_to_utf32le(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    } else {
        return md_utf16be_to_utf32be(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
    }
}

md_result md_utf16le_to_utf32le(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    return md_utf16_to_utf32_internal(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MD_TRUE);
}

md_result md_utf16be_to_utf32be(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    return md_utf16_to_utf32_internal(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags, MD_FALSE);
}

md_result md_utf16_to_utf32(md_utf32* pUTF32, size_t utf32Cap, size_t* pUTF32Len, const md_utf16* pUTF16, size_t utf16Len, size_t* pUTF16LenProcessed, md_uint32 flags)
{
    if (pUTF32 == NULL) {
        return md_utf16_to_utf32_length(pUTF32Len, pUTF16, utf16Len, flags);
    }

    if (pUTF32Len != NULL) {
        *pUTF32Len = 0;
    }

    if (pUTF32 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf16_bom((const md_uint8*)pUTF16, utf16Len)) {
        md_result result;
        size_t utf16LenProcessed;

        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf16_bom_le((const md_uint8*)pUTF16);
        
        pUTF16 += 1;    /* Skip past the BOM. */
        if (utf16Len != (size_t)-1) {
            utf16Len -= 1;
        }

        if (isLE) {
            result = md_utf16le_to_utf32le(pUTF32, utf32Cap, pUTF32Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = md_utf16be_to_utf32be(pUTF32, utf32Cap, pUTF32Len, pUTF16+1, utf16Len-1, &utf16LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF16LenProcessed != NULL) {
            *pUTF16LenProcessed = utf16LenProcessed + 1;    /* +1 for BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf16ne_to_utf32ne(pUTF32, utf32Cap, pUTF32Len, pUTF16, utf16Len, pUTF16LenProcessed, flags);
}


md_result md_utf32_to_utf8_length_internal(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf8Len = 0;
    md_utf32 utf32;

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF32 == NULL) {
        return MD_INVALID_ARGS; /* Invalid input. */
    }

    if (utf32Len == 0 || pUTF32[0] == 0) {
        return MD_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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
                utf32 = md_le2host_32(pUTF32[0]);
            } else {
                utf32 = md_be2host_32(pUTF32[0]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8Len += md_utf32_cp_to_utf8_length(utf32);
            pUTF32  += 1;
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (isLE) {
                utf32 = md_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = md_be2host_32(pUTF32[iUTF32]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8Len += md_utf32_cp_to_utf8_length(utf32);
        }
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = utf8Len;
    }

    return result;
}

md_result md_utf32ne_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf32le_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
    } else {
        return md_utf32be_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
    }
}

md_result md_utf32le_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    return md_utf32_to_utf8_length_internal(pUTF8Len, pUTF32, utf32Len, flags, MD_TRUE);
}

md_result md_utf32be_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    return md_utf32_to_utf8_length_internal(pUTF8Len, pUTF32, utf32Len, flags, MD_FALSE);
}

md_result md_utf32_to_utf8_length(size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf32_bom_le((const md_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            return md_utf32le_to_utf8_length(pUTF8Len, pUTF32+1, utf32Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return md_utf32be_to_utf8_length(pUTF8Len, pUTF32+1, utf32Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf32ne_to_utf8_length(pUTF8Len, pUTF32, utf32Len, flags);
}


md_result md_utf32_to_utf8_internal(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf8CapOriginal = utf8Cap;
    size_t utf8cpLen;   /* Code point length in UTF-8 code units. */
    md_utf32 utf32;

    if (pUTF8 == NULL) {
        return md_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, isLE);
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    if (pUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf32* pUTF32Original = pUTF32;
        while (pUTF32[0] != 0) {
            if (utf8Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = md_le2host_32(pUTF32[0]);
            } else {
                utf32 = md_be2host_32(pUTF32[0]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8cpLen = md_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
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
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = md_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = md_be2host_32(pUTF32[iUTF32]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf8cpLen = md_utf32_cp_to_utf8(utf32, pUTF8, utf8Cap);
            if (utf8cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
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
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF8[0] = 0;
    }

    if (pUTF8Len != NULL) {
        *pUTF8Len = (utf8CapOriginal - utf8Cap);
    }

    return result;
}

md_result md_utf32ne_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf32le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    } else {
        return md_utf32be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    }
}

md_result md_utf32le_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    return md_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MD_TRUE);
}

md_result md_utf32be_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    return md_utf32_to_utf8_internal(pUTF8, utf8Cap, pUTF8Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MD_FALSE);
}

md_result md_utf32_to_utf8(md_utf8* pUTF8, size_t utf8Cap, size_t* pUTF8Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    if (pUTF8Len != NULL) {
        *pUTF8Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        md_result result;
        size_t utf32LenProcessed;

        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf32_bom_le((const md_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            result = md_utf32le_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = md_utf32be_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF32LenProcessed) {
            *pUTF32LenProcessed = utf32LenProcessed + 1;    /* +1 for the BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf32ne_to_utf8(pUTF8, utf8Cap, pUTF8Len, pUTF32+1, utf32Len-1, pUTF32LenProcessed, flags);
}



md_result md_utf32_to_utf16_length_internal(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf16Len = 0;
    md_utf32 utf32;

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF32 == NULL) {
        return MD_INVALID_ARGS; /* Invalid input. */
    }

    if (utf32Len == 0 || pUTF32[0] == 0) {
        return MD_SUCCESS;  /* Empty input string. Length is always 0. */
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
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
                utf32 = md_le2host_32(pUTF32[0]);
            } else {
                utf32 = md_be2host_32(pUTF32[0]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16Len += md_utf32_cp_to_utf16_length(utf32);
            pUTF32   += 1;
        }
    } else {
        /* Fixed length string. */
        size_t iUTF32;
        for (iUTF32 = 0; iUTF32 < utf32Len; iUTF32 += 1) {
            if (isLE) {
                utf32 = md_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = md_be2host_32(pUTF32[iUTF32]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16Len += md_utf32_cp_to_utf16_length(utf32);
        }
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = utf16Len;
    }

    return result;
}

md_result md_utf32ne_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf32le_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
    } else {
        return md_utf32be_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
    }
}

md_result md_utf32le_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    return md_utf32_to_utf16_length_internal(pUTF16Len, pUTF32, utf32Len, flags, MD_TRUE);
}

md_result md_utf32be_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    return md_utf32_to_utf16_length_internal(pUTF16Len, pUTF32, utf32Len, flags, MD_FALSE);
}

md_result md_utf32_to_utf16_length(size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, md_uint32 flags)
{
    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf32_bom_le((const md_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            return md_utf32le_to_utf16_length(pUTF16Len, pUTF32+1, utf32Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            return md_utf32be_to_utf16_length(pUTF16Len, pUTF32+1, utf32Len-1, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf32ne_to_utf16_length(pUTF16Len, pUTF32, utf32Len, flags);
}


md_result md_utf32_to_utf16_internal(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags, md_bool32 isLE)
{
    md_result result = MD_SUCCESS;
    size_t utf16CapOriginal = utf16Cap;
    size_t utf16cpLen;  /* Code point length in UTF-8 code units. */
    md_utf32 utf32;

    if (pUTF16 == NULL) {
        return md_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, isLE);
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    if (pUTF16 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }
    }

    if (utf32Len == (size_t)-1) {
        /* Null terminated string. */
        const md_utf32* pUTF32Original = pUTF32;
        while (pUTF32[0] != 0) {
            if (utf16Cap == 0) {
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = md_le2host_32(pUTF32[0]);
            } else {
                utf32 = md_be2host_32(pUTF32[0]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16cpLen = md_utf32_cp_to_utf16(utf32, pUTF16, utf16Cap);
            if (utf16cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            if (isLE) {
                if (utf16cpLen == 1) {
                    pUTF16[0] = md_host2le_16(pUTF16[0]);
                } else {
                    pUTF16[0] = md_host2le_16(pUTF16[0]);
                    pUTF16[1] = md_host2le_16(pUTF16[1]);
                }
            } else {
                if (utf16cpLen == 1) {
                    pUTF16[0] = md_host2be_16(pUTF16[0]);
                } else {
                    pUTF16[0] = md_host2be_16(pUTF16[0]);
                    pUTF16[1] = md_host2be_16(pUTF16[1]);
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
                result = MD_OUT_OF_MEMORY;
                break;
            }

            if (isLE) {
                utf32 = md_le2host_32(pUTF32[iUTF32]);
            } else {
                utf32 = md_be2host_32(pUTF32[iUTF32]);
            }

            if (!md_is_valid_code_point(utf32)) {
                if ((flags & MD_ERROR_ON_INVALID_CODE_POINT) != 0) {
                    result = MD_INVALID_CODE_POINT;
                    break;
                } else {
                    utf32 = MD_UNICODE_REPLACEMENT_CHARACTER;
                }
            }

            utf16cpLen = md_utf32_cp_to_utf16(utf32, pUTF16, utf16Cap);
            if (utf16cpLen == 0) {
                result = MD_OUT_OF_MEMORY;    /* A return value of 0 at this point means there was not enough room in the output buffer. */
                break;
            }

            if (isLE) {
                if (utf16cpLen == 1) {
                    pUTF16[0] = md_host2le_16(pUTF16[0]);
                } else {
                    pUTF16[0] = md_host2le_16(pUTF16[0]);
                    pUTF16[1] = md_host2le_16(pUTF16[1]);
                }
            } else {
                if (utf16cpLen == 1) {
                    pUTF16[0] = md_host2be_16(pUTF16[0]);
                } else {
                    pUTF16[0] = md_host2be_16(pUTF16[0]);
                    pUTF16[1] = md_host2be_16(pUTF16[1]);
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
        result = MD_OUT_OF_MEMORY;    /* Not enough room in the output buffer. */
    } else {
        pUTF16[0] = 0;
    }

    if (pUTF16Len != NULL) {
        *pUTF16Len = (utf16CapOriginal - utf16Cap);
    }

    return result;
}

md_result md_utf32ne_to_utf16ne(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    if (md_is_little_endian()) {
        return md_utf32le_to_utf16le(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    } else {
        return md_utf32be_to_utf16be(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags);
    }
}

md_result md_utf32le_to_utf16le(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    return md_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MD_TRUE);
}

md_result md_utf32be_to_utf16be(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    return md_utf32_to_utf16_internal(pUTF16, utf16Cap, pUTF16Len, pUTF32, utf32Len, pUTF32LenProcessed, flags, MD_FALSE);
}

md_result md_utf32_to_utf16(md_utf16* pUTF16, size_t utf16Cap, size_t* pUTF16Len, const md_utf32* pUTF32, size_t utf32Len, size_t* pUTF32LenProcessed, md_uint32 flags)
{
    if (pUTF16Len != NULL) {
        *pUTF16Len = 0;
    }

    /* Check for BOM. */
    if (md_has_utf32_bom((const md_uint8*)pUTF32, utf32Len)) {
        md_result result;
        size_t utf32LenProcessed;

        md_bool32 isLE;
        if ((flags & MD_FORBID_BOM) != 0) {
            return MD_INVALID_BOM;  /* Found a BOM, but it's forbidden. */
        }

        /* With this function, we need to use the endian defined by the BOM. */
        isLE = md_is_utf32_bom_le((const md_uint8*)pUTF32);
        
        pUTF32 += 1;    /* Skip past the BOM. */
        if (utf32Len != (size_t)-1) {
            utf32Len -= 1;
        }

        if (isLE) {
            result = md_utf32le_to_utf16le(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        } else {
            result = md_utf32be_to_utf16be(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, &utf32LenProcessed, flags | MD_FORBID_BOM); /* <-- We already found a BOM, so we don't want to allow another occurance. */
        }

        if (pUTF32LenProcessed) {
            *pUTF32LenProcessed = utf32LenProcessed + 1;    /* +1 for the BOM. */
        }

        return result;
    }

    /* Getting here means there was no BOM, so assume native endian. */
    return md_utf32ne_to_utf16ne(pUTF16, utf16Cap, pUTF16Len, pUTF32+1, utf32Len-1, pUTF32LenProcessed, flags);
}



/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************


 Platform Specific Section


 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/


/**************************************************************************************************************************************************************

 GDI + Uniscribe

 **************************************************************************************************************************************************************/
#if defined(MD_HAS_GDI)
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4214)   /* nonstandard extension used: bit field types other than int */
#endif
#include <usp10.h>  /* TODO: Get rid of this dependency. */
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#include <math.h>   /* For cosf() and sinf(). May want to move this out of here and into the general implementation section. */

/* usp10.dll */
typedef HRESULT (WINAPI * MD_PFN_ScriptItemize)  (const WCHAR *pwcInChars, int cInChars, int cMaxItems, const SCRIPT_CONTROL* psControl, const SCRIPT_STATE* psState, SCRIPT_ITEM* pItems, int* pcItems);
typedef HRESULT (WINAPI * MD_PFN_ScriptShape)    (HDC hdc, SCRIPT_CACHE* psc, const WCHAR* pwcChars, int cChars, int cMaxGlyphs, SCRIPT_ANALYSIS* psa, WORD* pwOutGlyphs, WORD* pwLogClust, SCRIPT_VISATTR* psva, int* pcGlyphs);
typedef HRESULT (WINAPI * MD_PFN_ScriptPlace)    (HDC hdc, SCRIPT_CACHE* psc, const WORD* pwGlyphs, int cGlyphs, const SCRIPT_VISATTR* psva, SCRIPT_ANALYSIS* psa, int* piAdvance, GOFFSET* pGoffset, ABC* pABC);
typedef HRESULT (WINAPI * MD_PFN_ScriptTextOut)  (const HDC hdc, SCRIPT_CACHE* psc, int x, int y, UINT fuOptions, const RECT* lprc, const SCRIPT_ANALYSIS* psa, const WCHAR* pwcReserved, int iReserved, const WORD* pwGlyphs, int cGlyphs, const int* piAdvance, const int* piJustify, const GOFFSET* pGoffset);
typedef HRESULT (WINAPI * MD_PFN_ScriptFreeCache)(SCRIPT_CACHE* psc);

/* msimg32.dll */
typedef BOOL (WINAPI * MD_PFN_AlphaBlend)(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);


/* Font */
md_result md_font_init__gdi(md_api* pAPI, const md_font_config* pConfig, md_font* pFont)
{
    BYTE slantGDI;
    LONG weightGDI;
    BYTE qualityGDI;
    LOGFONTA logfont;
    HFONT hFont;
    LONG fontSize;

    MD_ASSERT(pAPI != NULL);
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pFont != NULL);

    switch (pConfig->weight)
    {
    case md_font_weight_medium:      weightGDI = FW_MEDIUM;     break;
    case md_font_weight_thin:        weightGDI = FW_THIN;       break;
    case md_font_weight_extra_light: weightGDI = FW_EXTRALIGHT; break;
    case md_font_weight_light:       weightGDI = FW_LIGHT;      break;
    case md_font_weight_semi_bold:   weightGDI = FW_SEMIBOLD;   break;
    case md_font_weight_bold:        weightGDI = FW_BOLD;       break;
    case md_font_weight_extra_bold:  weightGDI = FW_EXTRABOLD;  break;
    case md_font_weight_heavy:       weightGDI = FW_HEAVY;      break;
    default:                         weightGDI = FW_REGULAR;    break;
    }

    slantGDI = FALSE;
    if (pConfig->slant == md_font_slant_italic || pConfig->slant == md_font_slant_oblique) {
        slantGDI = TRUE;
    }

    if (pConfig->sizeInPixels > 0) {
        fontSize = -(LONG)pConfig->sizeInPixels;
    } else {
        fontSize = -MulDiv(pConfig->sizeInPoints, GetDeviceCaps((HDC)pAPI->gdi.hGlobalDC, LOGPIXELSY), 72);
    }

    qualityGDI = CLEARTYPE_QUALITY;
    switch (pConfig->antialiasMode) {
        case md_antialias_mode_none:     qualityGDI = NONANTIALIASED_QUALITY; break;
        case md_antialias_mode_gray:     qualityGDI = ANTIALIASED_QUALITY;    break;
        case md_antialias_mode_subpixel: qualityGDI = CLEARTYPE_QUALITY;      break;
        case md_antialias_mode_default:  qualityGDI = CLEARTYPE_QUALITY;      break;
        default:                         qualityGDI = CLEARTYPE_QUALITY;      break;
    }

    MD_ZERO_OBJECT(&logfont);
    logfont.lfHeight      = fontSize;
    logfont.lfWeight      = weightGDI;
    logfont.lfItalic      = slantGDI;
    logfont.lfCharSet     = DEFAULT_CHARSET;
    logfont.lfQuality     = qualityGDI;
    logfont.lfEscapement  = 0;
    logfont.lfOrientation = 0;
    md_strncpy_s(logfont.lfFaceName, sizeof(logfont.lfFaceName), pConfig->family, _TRUNCATE);

    hFont = CreateFontIndirectA(&logfont);
    if (hFont == NULL) {
        return MD_ERROR;    /* Failed to create font. */
    }

    /* Grab the font metrics. */
    SaveDC((HDC)pAPI->gdi.hGlobalDC);
    {
        TEXTMETRICW metrics;

        SelectObject((HDC)pAPI->gdi.hGlobalDC, hFont);
        GetTextMetricsW((HDC)pAPI->gdi.hGlobalDC, &metrics);
        pFont->metrics.ascent  =  metrics.tmAscent;
        pFont->metrics.descent = -metrics.tmDescent;
    }
    RestoreDC((HDC)pAPI->gdi.hGlobalDC, -1);

    pFont->gdi.hFont = (md_handle)hFont;

    return MD_SUCCESS;
}

void md_font_uninit__gdi(md_font* pFont)
{
    MD_ASSERT(pFont != NULL);

    /* Free the script cache. */
    ((MD_PFN_ScriptFreeCache)pFont->pAPI->gdi.ScriptFreeCache)((SCRIPT_CACHE*)&pFont->gdi.sc);
    pFont->gdi.sc = NULL;   /* Safety. */

    DeleteObject((HGDIOBJ)pFont->gdi.hFont);
}

md_result md_font_get_glyph_metrics__gdi(md_font* pFont, const md_glyph* pGlyphs, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    md_result result = MD_ERROR;
    HGDIOBJ hPrevFont = SelectObject((HDC)pFont->pAPI->gdi.hGlobalDC, (HFONT)pFont->gdi.hFont);
    {
        const MAT2 transform = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};    /* <-- Identity matrix */
        size_t iGlyph;

        for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
            GLYPHMETRICS metrics;
            DWORD bitmapBufferSize = GetGlyphOutlineW((HDC)pFont->pAPI->gdi.hGlobalDC, pGlyphs[iGlyph].index, GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, 0, NULL, &transform);
            if (bitmapBufferSize != GDI_ERROR) {
                pGlyphMetrics[iGlyph].sizeX    = metrics.gmBlackBoxX;
                pGlyphMetrics[iGlyph].sizeY    = metrics.gmBlackBoxY;
                pGlyphMetrics[iGlyph].bearingX = metrics.gmptGlyphOrigin.x;
                pGlyphMetrics[iGlyph].bearingY = metrics.gmptGlyphOrigin.y;
                pGlyphMetrics[iGlyph].advanceX = metrics.gmCellIncX;
                pGlyphMetrics[iGlyph].advanceY = metrics.gmCellIncY;
                result = MD_SUCCESS;
            }
        }
    }
    SelectObject((HDC)pFont->pAPI->gdi.hGlobalDC, hPrevFont);

    return result;
}

md_result md_font_get_glyph_metrics_by_index__gdi(md_font* pFont, const md_uint32* pGlyphIndices, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    md_result result = MD_ERROR;
    HGDIOBJ hPrevFont = SelectObject((HDC)pFont->pAPI->gdi.hGlobalDC, (HFONT)pFont->gdi.hFont);
    {
        const MAT2 transform = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};    /* <-- Identity matrix */
        size_t iGlyph;

        for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
            GLYPHMETRICS metrics;
            DWORD bitmapBufferSize = GetGlyphOutlineW((HDC)pFont->pAPI->gdi.hGlobalDC, pGlyphIndices[iGlyph], GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, 0, NULL, &transform);
            if (bitmapBufferSize != GDI_ERROR) {
                pGlyphMetrics[iGlyph].sizeX    = metrics.gmBlackBoxX;
                pGlyphMetrics[iGlyph].sizeY    = metrics.gmBlackBoxY;
                pGlyphMetrics[iGlyph].bearingX = metrics.gmptGlyphOrigin.x;
                pGlyphMetrics[iGlyph].bearingY = metrics.gmptGlyphOrigin.y;
                pGlyphMetrics[iGlyph].advanceX = metrics.gmCellIncX;
                pGlyphMetrics[iGlyph].advanceY = metrics.gmCellIncY;
                result = MD_SUCCESS;
            }
        }
    }
    SelectObject((HDC)pFont->pAPI->gdi.hGlobalDC, hPrevFont);

    return result;
}


/* Brush */
md_result md_brush_init__gdi(md_api* pAPI, const md_brush_config* pConfig, md_brush* pBrush)
{
    HBRUSH    hBrush     = NULL;
    HBITMAP   hBitmap    = NULL;
    md_bool32 ownsBitmap = MD_FALSE;

    MD_ASSERT(pAPI != NULL);
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pBrush != NULL);

    switch (pConfig->type)
    {
        case md_brush_type_solid:
        {
            hBrush = (HBRUSH)pAPI->gdi.hStockSolidFillBrush;
        } break;

        case md_brush_type_linear:
        case md_brush_type_radial:
        {
            /* Nothing to do. Gradients will be handled at fill/stroke time. */
        } break;

        case md_brush_type_gc:
        {
            /* If the HDC is writing to a HBITMAP we can retrieve that. Otherwise we need to create a new HBITMAP, BitBlt the source HDC over to it and use that. */
            if (pConfig->gc.pGC == NULL) {
                return MD_INVALID_ARGS; /* GC not set for md_brush_type_gc. */
            }

            if (pConfig->gc.pGC->gdi.hBitmap != NULL) {
                ownsBitmap = MD_FALSE;
                hBitmap    = (HBITMAP)pConfig->gc.pGC->gdi.hBitmap;
                hBrush     = CreatePatternBrush(hBitmap);
            } else {
                HDC hTempDC;
                md_uint32 sizeX;
                md_uint32 sizeY;
                md_gc_get_size(pConfig->gc.pGC, &sizeX, &sizeY);

                ownsBitmap = MD_TRUE;

                hBitmap = CreateCompatibleBitmap((HDC)pConfig->gc.pGC->gdi.hDC, (int)sizeX, (int)sizeY);
                if (hBitmap == NULL) {
                    return MD_ERROR;    /* Failed to create bitmap. */
                }

                hTempDC = CreateCompatibleDC((HDC)pConfig->gc.pGC->gdi.hDC);
                if (hTempDC == NULL) {
                    DeleteObject(hBitmap);
                    return MD_ERROR;    /* Failed to create temp DC. */
                }

                SelectObject(hTempDC, hBitmap);
                BitBlt(hTempDC, 0, 0, (int)sizeX, (int)sizeY, (HDC)pConfig->gc.pGC->gdi.hDC, 0, 0, SRCCOPY);

                DeleteDC(hTempDC);
                
                hBrush = CreatePatternBrush(hBitmap);
            }

            if (hBrush == NULL) {
                if (hBitmap != NULL && ownsBitmap) {
                    DeleteObject(hBitmap);
                }
                return MD_ERROR;
            }
        } break;

        default: return MD_INVALID_ARGS;
    }

    pBrush->gdi.hBrush     = hBrush;
    pBrush->gdi.hBitmap    = hBitmap;
    pBrush->gdi.ownsBitmap = ownsBitmap;

    return MD_SUCCESS;
}

void md_brush_uninit__gdi(md_brush* pBrush)
{
    MD_ASSERT(pBrush != NULL);

    if (pBrush->gdi.hBrush != pBrush->pAPI->gdi.hStockSolidFillBrush) {
        DeleteObject((HGDIOBJ)pBrush->gdi.hBrush);

        /* Destroy the bitmap after the brush to ensure it's not referenced anymore. */
        if (pBrush->gdi.hBitmap != NULL && pBrush->gdi.ownsBitmap) {
            DeleteObject((HGDIOBJ)pBrush->gdi.hBitmap);
        }
    }
}

void md_brush_set_origin__gdi(md_brush* pBrush, md_int32 x, md_int32 y)
{
    MD_ASSERT(pBrush != NULL);

    pBrush->gdi.originX = x;
    pBrush->gdi.originY = y;
}


/* Graphics */
md_result md_gc_init__gdi(md_api* pAPI, const md_gc_config* pConfig, md_gc* pGC)
{
    MD_ASSERT(pAPI != NULL);

    (void)pAPI;

    if (pConfig->gdi.hDC != NULL) {
        pGC->isTransient = MD_TRUE;
        pGC->format = md_format_unknown;
        pGC->gdi.hDC = pConfig->gdi.hDC;
    } else {
        pGC->isTransient = MD_FALSE;
        pGC->format = pConfig->format;
        pGC->gdi.hDC = (md_handle)CreateCompatibleDC(NULL);
        
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

            pGC->gdi.hBitmap = (md_handle)CreateDIBSection((HDC)pGC->gdi.hDC, &bmi, DIB_RGB_COLORS, (void**)&pGC->gdi.pBitmapData, NULL, 0);
            if (pGC->gdi.hBitmap == NULL) {
                DeleteDC((HDC)pGC->gdi.hDC);
                pGC->gdi.hDC = NULL;
                return MD_ERROR;
            }

            if (pConfig->pInitialImageData != NULL) {
                md_copy_and_flip_image_data_y(pGC->gdi.pBitmapData, pConfig->pInitialImageData, pConfig->sizeX, pConfig->sizeY, 0, md_format_bgra, pConfig->stride, pConfig->format); /* GDI uses BGRA and a tightly packed stride internally. */
                GdiFlush();
            }

            pGC->gdi.bitmapSizeX = pConfig->sizeX;
            pGC->gdi.bitmapSizeY = pConfig->sizeY;

            SelectObject((HDC)pGC->gdi.hDC, (HGDIOBJ)pGC->gdi.hBitmap);
        }
    }

    /* Defaults. */
    SetGraphicsMode((HDC)pGC->gdi.hDC, GM_ADVANCED);    /* <-- Needed for world transforms (rotate and scale). */
    SetStretchBltMode((HDC)pGC->gdi.hDC, COLORONCOLOR); /* <-- COLORONCOLOR is better than the default when the image is scaled down. Not setting this results in black pixelation. */
    SetBkMode((HDC)pGC->gdi.hDC, TRANSPARENT);          /* <-- Transparent text background by default. */
    SetPolyFillMode((HDC)pGC->gdi.hDC, WINDING);

    /* We need at least one item in the state stack. Unfortunate malloc() here - may want to think about optimizing this. Perhaps some optimized per-API scheme? */
    pGC->gdi.stateCount = 1;
    pGC->gdi.stateCap   = 1;
    pGC->gdi.pState = (md_gc_state_gdi*)MD_CALLOC(pGC->gdi.stateCap, sizeof(*pGC->gdi.pState));
    if (pGC->gdi.pState == NULL) {
        if (!pGC->isTransient) {
            DeleteDC((HDC)pGC->gdi.hDC);
            DeleteObject((HGDIOBJ)pGC->gdi.hBitmap);
        }
        return MD_OUT_OF_MEMORY;
    }

    return MD_SUCCESS;
}

void md_gc_uninit__gdi(md_gc* pGC)
{
    md_uint32 i;

    MD_ASSERT(pGC != NULL);

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

    MD_FREE(pGC->gdi.pState);
}

md_result md_gc_get_size__gdi(md_gc* pGC, md_uint32* pSizeX, md_uint32* pSizeY)
{
    MD_ASSERT(pSizeX != NULL);
    MD_ASSERT(pSizeY != NULL);

    /* If it's a bitmap we use the size of that. Otherwise, if it's a window we try that. If it's none of these, we use HORZRES/VERTRES. */
    if (pGC->gdi.hBitmap != NULL) {
        *pSizeX = pGC->gdi.bitmapSizeX;
        *pSizeY = pGC->gdi.bitmapSizeY;
    } else {
        HWND hWnd = WindowFromDC((HDC)pGC->gdi.hDC);
        if (hWnd != NULL) {
            RECT rect;
            GetClientRect(hWnd, &rect);
            *pSizeX = (md_uint32)(rect.right - rect.left);
            *pSizeY = (md_uint32)(rect.bottom - rect.top);
        } else {
            /* Fallback to HORZRES/VERTRES. */
            *pSizeX = (md_uint32)GetDeviceCaps((HDC)pGC->gdi.hDC, HORZRES);
            *pSizeY = (md_uint32)GetDeviceCaps((HDC)pGC->gdi.hDC, VERTRES);
        }
    }

    return MD_SUCCESS;
}

md_result md_gc_get_image_data__gdi(md_gc* pGC, md_format outputFormat, void* pImageData)
{
    md_result result;
    HDC hTempDC;
    HBITMAP hTempBitmap;
    BITMAPINFO bmi;
    void* pTempBitmapData;
    md_uint32 sizeX;
    md_uint32 sizeY;
    md_uint32 x;
    md_uint32 y;

    MD_ASSERT(pGC != NULL);

    result = md_gc_get_size__gdi(pGC, &sizeX, &sizeY);
    if (result != MD_SUCCESS) {
        return result;
    }

    /* We need to BitBlt into a DIB, flush, and then move that data into pImageData with the necessary data conversion. */
    hTempDC = CreateCompatibleDC(NULL);
    if (hTempDC == NULL) {
        return MD_ERROR;    /* Failed to create the temporary DC. */
    }

    MD_ZERO_OBJECT(&bmi);
    bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth       = (LONG)sizeX;
    bmi.bmiHeader.biHeight      = (LONG)sizeY;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;   /* Only supporting 32-bit formats. */
    bmi.bmiHeader.biCompression = BI_RGB;

    hTempBitmap = CreateDIBSection(hTempDC, &bmi, DIB_RGB_COLORS, &pTempBitmapData, NULL, 0);
    if (hTempBitmap == NULL) {
        DeleteDC(hTempDC);
        return MD_ERROR;    /* Failed to create the temporary bitmap. */
    }

    SelectObject(hTempDC, hTempBitmap);
    BitBlt(hTempDC, 0, 0, (int)sizeX, (int)sizeY, (HDC)pGC->gdi.hDC, 0, 0, SRCCOPY);
    GdiFlush();

    /*
    GDI does not support alpha on the bitmap. Or at least, I haven't figured out how to use it properly. Therefore, I need to set  the alpha channel
    to 0xFF explicitly because GDI seems to ignore the alpha channel when it draws.
    */
    for (y = 0; y < sizeY; ++y) {
        md_uint8* pDstRow = (md_uint8*)MD_OFFSET_PTR(pTempBitmapData, y*sizeX*4);
        for (x = 0; x < sizeX; ++x) {
            pDstRow[x*4 + 3] = 0xFF;
        }
    }

    md_copy_and_flip_image_data_y(pImageData, pTempBitmapData, sizeX, sizeY, 0, outputFormat, 0, md_format_bgra);

    DeleteDC(hTempDC);
    DeleteObject(hTempBitmap);

    return MD_SUCCESS;
}

md_result md_gc_save__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pGC->gdi.stateCount > 0);

    if (SaveDC((HDC)pGC->gdi.hDC) == 0) {
        return MD_ERROR;    /* Failed to save the HDC state. */
    }

    if (pGC->gdi.stateCount == pGC->gdi.stateCap) {
        md_uint32 newCap = MD_MAX(1, pGC->gdi.stateCap * 2);
        md_gc_state_gdi* pNewState = (md_gc_state_gdi*)MD_REALLOC(pGC->gdi.pState, newCap * sizeof(*pNewState));
        if (pNewState == NULL) {
            return MD_OUT_OF_MEMORY;
        }

        pGC->gdi.pState   = pNewState;
        pGC->gdi.stateCap = newCap;
    }

    MD_ASSERT(pGC->gdi.stateCount < pGC->gdi.stateCap);

    pGC->gdi.pState[pGC->gdi.stateCount] = pGC->gdi.pState[pGC->gdi.stateCount-1];
    pGC->gdi.pState[pGC->gdi.stateCount].hPen = NULL;   /* The pen needs to be cleared to NULL and a new copy of the pen created when necessary. The pen will be deleted when the state is restored. */
    pGC->gdi.stateCount += 1;

    return MD_SUCCESS;
}

md_result md_gc_restore__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pGC->gdi.stateCount > 0);

    if (pGC->gdi.stateCount == 1) {
        return MD_INVALID_OPERATION;    /* Nothing to restore. */
    }

    pGC->gdi.stateCount -= 1;

    /* If we have a pen that's not the stock pen we need to delete it. */
    if (pGC->gdi.pState[pGC->gdi.stateCount].hPen != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_PEN));
        if (!DeleteObject((HGDIOBJ)pGC->gdi.pState[pGC->gdi.stateCount].hPen)) {
            MD_ASSERT(MD_FALSE);    /* For debugging. Should never happen. */
        }
        pGC->gdi.pState[pGC->gdi.stateCount].hPen = NULL;
    }

    if (RestoreDC((HDC)pGC->gdi.hDC, -1) == 0) {
        return MD_ERROR;    /* Failed to restore the HDC state. */
    }

    return MD_SUCCESS;
}

void md_gc_set_matrix__gdi(md_gc* pGC, const md_matrix* pMatrix)
{
    XFORM transform;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    transform.eM11 = pMatrix->m00;
    transform.eM12 = pMatrix->m01;
    transform.eM21 = pMatrix->m10;
    transform.eM22 = pMatrix->m11;
    transform.eDx  = pMatrix->dx;
    transform.eDy  = pMatrix->dy;
    SetWorldTransform((HDC)pGC->gdi.hDC, &transform);
}

void md_gc_get_matrix__gdi(md_gc* pGC, md_matrix* pMatrix)
{
    XFORM transform;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    GetWorldTransform((HDC)pGC->gdi.hDC, &transform);

    pMatrix->m00 = transform.eM11;
    pMatrix->m01 = transform.eM12;
    pMatrix->m10 = transform.eM21;
    pMatrix->m11 = transform.eM22;
    pMatrix->dx  = transform.eDx;
    pMatrix->dy  = transform.eDy;
}

void md_gc_set_matrix_identity__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    ModifyWorldTransform((HDC)pGC->gdi.hDC, NULL, MWT_IDENTITY);
}

void md_gc_transform__gdi(md_gc* pGC, const md_matrix* pMatrix)
{
    XFORM transform;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    transform.eM11 = pMatrix->m00;
    transform.eM12 = pMatrix->m01;
    transform.eM21 = pMatrix->m10;
    transform.eM22 = pMatrix->m11;
    transform.eDx  = pMatrix->dx;
    transform.eDy  = pMatrix->dy;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void md_gc_translate__gdi(md_gc* pGC, md_int32 offsetX, md_int32 offsetY)
{
    XFORM transform = {0};

    MD_ASSERT(pGC != NULL);

    transform.eM11 = 1;
    transform.eM22 = 1;
    transform.eDx  = (float)offsetX;
    transform.eDy  = (float)offsetY;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void md_gc_rotate__gdi(md_gc* pGC, float rotationInRadians)
{
    XFORM transform = {0};
    float c = (float)cos(rotationInRadians);
    float s = (float)sin(rotationInRadians);

    MD_ASSERT(pGC != NULL);

    transform.eM11 =  c;
    transform.eM12 =  s;
    transform.eM21 = -s;
    transform.eM22 =  c;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void md_gc_scale__gdi(md_gc* pGC, float scaleX, float scaleY)
{
    XFORM transform = {0};

    MD_ASSERT(pGC != NULL);

    transform.eM11 = scaleX;
    transform.eM22 = scaleY;
    ModifyWorldTransform((HDC)pGC->gdi.hDC, &transform, MWT_LEFTMULTIPLY);
}

void md_gc_set_miter_limit__gdi(md_gc* pGC, float limit)
{
    MD_ASSERT(pGC != NULL);
    SetMiterLimit((HDC)pGC->gdi.hDC, limit, NULL);
}

MD_PRIVATE void md_gc_delete_current_pen__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hPen != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_PEN));
        DeleteObject((HGDIOBJ)pGC->gdi.pState[pGC->gdi.stateCount-1].hPen);
        pGC->gdi.pState[pGC->gdi.stateCount-1].hPen = NULL;
    }
}

MD_PRIVATE void md_gc_select_current_pen__gdi(md_gc* pGC)
{
    md_uint32 iState = pGC->gdi.stateCount-1;

    MD_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[iState].hPen == NULL) {
        HPEN hPen;
        DWORD iPenStyle;
        DWORD cWidth;
        LOGBRUSH lbrush;
        DWORD cStyle;
        DWORD pStyle[16];
        md_uint32 i;

        iPenStyle = PS_GEOMETRIC;
        if (pGC->gdi.pState[iState].dashCount == 0) {
            iPenStyle |= PS_SOLID;
        } else {
            iPenStyle |= PS_USERSTYLE;
        }

        switch (pGC->gdi.pState[iState].lineCap) {
            case md_line_cap_square: iPenStyle |= PS_ENDCAP_SQUARE; break;
            case md_line_cap_round:  iPenStyle |= PS_ENDCAP_ROUND;  break;
            case md_line_cap_flat:
            default: iPenStyle |= PS_ENDCAP_FLAT; break;
        }

        switch (pGC->gdi.pState[iState].lineJoin) {
            case md_line_join_bevel: iPenStyle |= PS_JOIN_BEVEL; break;
            case md_line_join_round: iPenStyle |= PS_JOIN_ROUND; break;
            case md_line_join_miter:
            default: iPenStyle |= PS_JOIN_MITER; break;
        }

        cWidth = pGC->gdi.pState[iState].lineWidth;
        cStyle = pGC->gdi.pState[iState].dashCount;

        for (i = 0; i < pGC->gdi.pState[iState].dashCount; ++i) {
            pStyle[i] = (DWORD)pGC->gdi.pState[iState].dashes[i];
        }

        MD_ZERO_OBJECT(&lbrush);

        if (pGC->gdi.pState[iState].pUserLineBrush == NULL) {
            md_brush_config* pBrush = &pGC->gdi.pState[iState].lineStockBrush;
            if (pBrush->type == md_brush_type_solid) {
                if (pBrush->solid.color.a > 0) {
                    lbrush.lbStyle = BS_SOLID;
                    lbrush.lbColor = RGB(pBrush->solid.color.r, pBrush->solid.color.g, pBrush->solid.color.b);
                } else {
                    lbrush.lbStyle = BS_NULL;
                }
            } else if (pBrush->type == md_brush_type_gc) {
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
            md_brush* pBrush = pGC->gdi.pState[iState].pUserLineBrush;
            if (pBrush->config.type == md_brush_type_solid) {
                lbrush.lbStyle = BS_SOLID;
                lbrush.lbColor = RGB(pBrush->config.solid.color.r, pBrush->config.solid.color.g, pBrush->config.solid.color.b);
            } else if (/*pBrush->config.type == md_brush_type_linear ||*/ pBrush->config.type == md_brush_type_gc) {
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

        pGC->gdi.pState[iState].hPen = (md_handle)hPen;
    }
}


void md_gc_set_line_width__gdi(md_gc* pGC, md_int32 width)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].lineWidth = width;
    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_cap__gdi(md_gc* pGC, md_line_cap cap)
{
    MD_ASSERT(pGC != NULL);
    
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineCap = cap;
    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_join__gdi(md_gc* pGC, md_line_join join)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].lineJoin = join;
    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_dash__gdi(md_gc* pGC, const float* dashes, md_uint32 count)
{
    MD_ASSERT(pGC != NULL);

    if (count == 0 || dashes == NULL) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].dashCount = 0;
    } else {
        pGC->gdi.pState[pGC->gdi.stateCount-1].dashCount = count;
        MD_COPY_MEMORY(pGC->gdi.pState[pGC->gdi.stateCount-1].dashes, dashes, count * sizeof(float));
    }

    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_brush__gdi(md_gc* pGC, md_brush* pBrush)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = pBrush;
    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_brush_solid__gdi(md_gc* pGC, md_color color)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = NULL;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.type = md_brush_type_solid;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.solid.color = color;

    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_line_brush_gc__gdi(md_gc* pGC, md_gc* pSrcGC)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].pUserLineBrush = NULL;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.type = md_brush_type_gc;
    pGC->gdi.pState[pGC->gdi.stateCount-1].lineStockBrush.gc.pGC = pSrcGC;

    md_gc_delete_current_pen__gdi(pGC);
}

void md_gc_set_fill_brush__gdi(md_gc* pGC, md_brush* pBrush)
{
    md_uint32 iState = pGC->gdi.stateCount-1;

    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[iState].pUserFillBrush = pBrush;

    if (pBrush != NULL) {
        SelectObject((HDC)pGC->gdi.hDC, pBrush->gdi.hBrush);
    } else {
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_BRUSH));
    }

    if (pGC->gdi.pState[iState].hasTransientFillBrush) {
        md_brush_uninit(&pGC->gdi.pState[iState].transientFillBrush);
        pGC->gdi.pState[iState].hasTransientFillBrush = MD_FALSE;
    }
}

MD_PRIVATE void md_gc_set_fill_brush_transient__gdi(md_gc* pGC, const md_brush_config* pConfig)
{
    md_uint32 iState;
    md_brush prevTransientBrush;
    md_bool32 uninitPrevTransientBrush = MD_FALSE;

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;

    /* Need to make a copy of the previous transient fill brush so we can uninitialize later. */
    if (pGC->gdi.pState[iState].hasTransientFillBrush) {
        prevTransientBrush = pGC->gdi.pState[iState].transientFillBrush;
        uninitPrevTransientBrush = MD_TRUE;
    }

    if (md_brush_init(pGC->pAPI, pConfig, &pGC->gdi.pState[iState].transientFillBrush) != MD_SUCCESS) {
        return; /* Failed to initialize transient fill brush. */
    }

    pGC->gdi.pState[iState].hasTransientFillBrush = MD_TRUE;

    SelectObject((HDC)pGC->gdi.hDC, pGC->gdi.pState[iState].transientFillBrush.gdi.hBrush);

    if (pGC->gdi.pState[iState].transientFillBrush.config.type == md_brush_type_solid) {
        SetDCBrushColor((HDC)pGC->gdi.hDC, RGB(pConfig->solid.color.r, pConfig->solid.color.g, pConfig->solid.color.b));
    } else if (pGC->gdi.pState[iState].transientFillBrush.config.type == md_brush_type_gc) {
        /* TODO: Do something. */
    }

    pGC->gdi.pState[iState].pUserFillBrush = NULL;

    /* We can now uninitialize the previous brush since it's no longer selected on the GDI side. */
    if (uninitPrevTransientBrush) {
        md_brush_uninit(&prevTransientBrush);
    }
}

void md_gc_set_fill_brush_solid__gdi(md_gc* pGC, md_color color)
{
    md_brush_config config;
    config.type = md_brush_type_solid;
    config.solid.color = color;
    md_gc_set_fill_brush_transient__gdi(pGC, &config);
}

void md_gc_set_fill_brush_gc__gdi(md_gc* pGC, md_gc* pSrcGC)
{
    md_brush_config config;
    config.type = md_brush_type_gc;
    config.gc.pGC = pSrcGC;
    md_gc_set_fill_brush_transient__gdi(pGC, &config);
}

void md_gc_set_text_fg_color__gdi(md_gc* pGC, md_color fgColor)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    pGC->gdi.pState[iState].textFGColor = fgColor;

    SetTextColor((HDC)pGC->gdi.hDC, RGB(fgColor.r, fgColor.g, fgColor.b));
}

void md_gc_set_text_bg_color__gdi(md_gc* pGC, md_color bgColor)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    pGC->gdi.pState[iState].textBGColor = bgColor;

    if (bgColor.a == 0) {
        SetBkMode((HDC)pGC->gdi.hDC, TRANSPARENT);
    } else {
        SetBkMode((HDC)pGC->gdi.hDC, OPAQUE);
        SetBkColor((HDC)pGC->gdi.hDC, RGB(bgColor.r, bgColor.g, bgColor.b));
    }
}

md_color md_gc_get_text_fg_color__gdi(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    return pGC->gdi.pState[iState].textFGColor;
}

md_color md_gc_get_text_bg_color__gdi(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    return pGC->gdi.pState[iState].textBGColor;
}

void md_gc_set_blend_op__gdi(md_gc* pGC, md_blend_op op)
{
    MD_ASSERT(pGC != NULL);

    pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp = op;
}

void md_gc_set_antialias_mode__gdi(md_gc* pGC, md_antialias_mode mode)
{
    MD_ASSERT(pGC != NULL);

    /* The anti-aliasing mode is always md_antialias_mode_none with GDI since it doesn't support anti-aliasing. */
    (void)pGC;
    (void)mode;
}

void md_gc_set_fill_mode__gdi(md_gc* pGC, md_fill_mode mode)
{
    int modeGDI;

    MD_ASSERT(pGC != NULL);

    if (mode == md_fill_mode_evenodd) {
        modeGDI = ALTERNATE;
    } else {
        modeGDI = WINDING;
    }

    SetPolyFillMode((HDC)pGC->gdi.hDC, modeGDI);
}

void md_gc_set_stretch_filter__gdi(md_gc* pGC, md_stretch_filter filter)
{
    MD_ASSERT(pGC != NULL);

    if (filter == md_stretch_filter_linear) {
        SetStretchBltMode((HDC)pGC->gdi.hDC, HALFTONE);
        SetBrushOrgEx((HDC)pGC->gdi.hDC, 0, 0, NULL);       /* <-- Calling this because the documentation tells me to after SetStretchBltMode() with HALFTONE. I have no idea why on Earth this is necessary... */
    } else {
        SetStretchBltMode((HDC)pGC->gdi.hDC, COLORONCOLOR);
    }
}


MD_PRIVATE void md_gc_begin_path_if_required__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun == MD_FALSE) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun = MD_TRUE;
        BeginPath((HDC)pGC->gdi.hDC);
    }
}

MD_PRIVATE void md_gc_end_path__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun == MD_TRUE) {
        pGC->gdi.pState[pGC->gdi.stateCount-1].hasPathBegun = MD_FALSE;
        EndPath((HDC)pGC->gdi.hDC);
    }
}

void md_gc_move_to__gdi(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    md_gc_begin_path_if_required__gdi(pGC);
    MoveToEx((HDC)pGC->gdi.hDC, x, y, NULL);
}

void md_gc_line_to__gdi(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    md_gc_begin_path_if_required__gdi(pGC);
    LineTo((HDC)pGC->gdi.hDC, x, y);
}

void md_gc_rectangle__gdi(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom)
{
    MD_ASSERT(pGC != NULL);

    md_gc_begin_path_if_required__gdi(pGC);
    Rectangle((HDC)pGC->gdi.hDC, left, top, right, bottom);
}

void md_gc_arc__gdi(md_gc* pGC, md_int32 x, md_int32 y, md_int32 radius, float angle1InRadians, float angle2InRadians)
{
    MD_ASSERT(pGC != NULL);

    md_gc_begin_path_if_required__gdi(pGC);
    AngleArc((HDC)pGC->gdi.hDC, x, y, (DWORD)radius, MD_DEGREESF(angle1InRadians), MD_DEGREESF(angle2InRadians));
}

void md_gc_curve_to__gdi(md_gc* pGC, md_int32 x1, md_int32 y1, md_int32 x2, md_int32 y2, md_int32 x3, md_int32 y3)
{
    POINT pt[3];

    MD_ASSERT(pGC != NULL);

    pt[0].x = x1;
    pt[0].y = y1;
    pt[1].x = x2;
    pt[1].y = y2;
    pt[2].x = x3;
    pt[2].y = y3;

    md_gc_begin_path_if_required__gdi(pGC);
    PolyBezierTo((HDC)pGC->gdi.hDC, pt, 3);
}

void md_gc_close_path__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    CloseFigure((HDC)pGC->gdi.hDC);
}

void md_gc_clip__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    md_gc_end_path__gdi(pGC);
    SelectClipPath((HDC)pGC->gdi.hDC, RGN_AND);
}

void md_gc_reset_clip__gdi(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    SelectClipRgn((HDC)pGC->gdi.hDC, NULL);
}

md_bool32 md_gc_is_point_inside_clip__gdi(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    return PtVisible((HDC)pGC->gdi.hDC, x, y);
}

void md_gc_fill__gdi(md_gc* pGC)
{
    md_uint32 iState;
    md_bool32 adjustBrushOrigin = MD_FALSE;
    md_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin = {0, 0};

    MD_ASSERT(pGC != NULL);

    md_gc_end_path__gdi(pGC);

    iState = pGC->gdi.stateCount-1;

    pBrushToUseForOrigin = pGC->gdi.pState[iState].pUserFillBrush;
    if (pBrushToUseForOrigin != NULL) {
        if (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0) {
            adjustBrushOrigin = MD_TRUE;
            GetBrushOrgEx((HDC)pGC->gdi.hDC, &prevBrushOrigin);
            SetBrushOrgEx((HDC)pGC->gdi.hDC, pBrushToUseForOrigin->gdi.originX, pBrushToUseForOrigin->gdi.originY, &prevBrushOrigin);
        }
    }

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp == md_blend_op_src) {
        FillPath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        FillPath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void md_gc_stroke__gdi(md_gc* pGC)
{
    md_uint32 iState;
    md_bool32 adjustBrushOrigin = MD_FALSE;
    md_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin = {0, 0};

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;
    
    md_gc_end_path__gdi(pGC);
    md_gc_select_current_pen__gdi(pGC);

    pBrushToUseForOrigin = pGC->gdi.pState[iState].pUserLineBrush;
    if (pBrushToUseForOrigin != NULL) {
        if (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0) {
            adjustBrushOrigin = MD_TRUE;
            GetBrushOrgEx((HDC)pGC->gdi.hDC, &prevBrushOrigin);
            SetBrushOrgEx((HDC)pGC->gdi.hDC, pBrushToUseForOrigin->gdi.originX, pBrushToUseForOrigin->gdi.originY, &prevBrushOrigin);
        }
    }

    if (pGC->gdi.pState[iState].blendOp == md_blend_op_src) {
        StrokePath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        StrokePath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void md_gc_fill_and_stroke__gdi(md_gc* pGC)
{
    md_uint32 iState;
    md_bool32 adjustBrushOrigin = MD_FALSE;
    md_brush* pUserLineBrush;
    md_brush* pUserFillBrush;
    md_brush* pBrushToUseForOrigin;
    POINT prevBrushOrigin = {0, 0};

    MD_ASSERT(pGC != NULL);

    iState = pGC->gdi.stateCount-1;

    md_gc_end_path__gdi(pGC);
    md_gc_select_current_pen__gdi(pGC);

    pUserLineBrush = pGC->gdi.pState[iState].pUserLineBrush;
    pUserFillBrush = pGC->gdi.pState[iState].pUserFillBrush;

    pBrushToUseForOrigin = pUserFillBrush;
    if (pBrushToUseForOrigin == NULL) {
        pBrushToUseForOrigin = pUserLineBrush;
    } else {
        if (pBrushToUseForOrigin->config.type == md_brush_type_solid) {
            pBrushToUseForOrigin = pUserLineBrush;
        }
    }

    if (pBrushToUseForOrigin != NULL && (pBrushToUseForOrigin->gdi.originX != 0 || pBrushToUseForOrigin->gdi.originY != 0)) {
        adjustBrushOrigin = MD_TRUE;
        GetBrushOrgEx((HDC)pGC->gdi.hDC, &prevBrushOrigin);
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

    if (pGC->gdi.pState[pGC->gdi.stateCount-1].blendOp == md_blend_op_src) {
        StrokeAndFillPath((HDC)pGC->gdi.hDC);
    } else {
        /* TODO: Implement blending here. Might need to use and intermediary bitmap... */
        StrokeAndFillPath((HDC)pGC->gdi.hDC);
    }

    if (adjustBrushOrigin) {
        SetBrushOrgEx((HDC)pGC->gdi.hDC, prevBrushOrigin.x, prevBrushOrigin.y, NULL);
    }
}

void md_gc_draw_gc__gdi(md_gc* pGC, md_gc* pSrcGC, md_int32 srcX, md_int32 srcY)
{
    HDC hDstDC;
    HDC hSrcDC;
    md_uint32 srcSizeX;
    md_uint32 srcSizeY;

    hDstDC = (HDC)pGC->gdi.hDC;
    hSrcDC = (HDC)pSrcGC->gdi.hDC;

    md_gc_get_size__gdi(pSrcGC, &srcSizeX, &srcSizeY);

    /* If the image has an alpha channel we need to use AlphaBlend(). */
    if (pSrcGC->gdi.hBitmap != NULL && md_format_has_alpha(pSrcGC->format)) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ((MD_PFN_AlphaBlend)pGC->pAPI->gdi.AlphaBlend)(hDstDC, 0, 0, (int)srcSizeX, (int)srcSizeY, hSrcDC, srcX, srcY, (int)srcSizeX, (int)srcSizeY, blend);
    } else {
        StretchBlt(hDstDC, 0, 0, (int)srcSizeX, (int)srcSizeY, hSrcDC, srcX, srcX, (int)srcSizeX, (int)srcSizeY, SRCCOPY);
    }
}

/* API */
md_result md_itemize_utf16__gdi(md_font* pFont, const md_utf16* pTextUTF16, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState)
{
    HRESULT hResult;
    md_uint32 itemCapacity;
    int scriptItemCount = 0;
    int iItem;
    MD_SCRIPT_ITEM  pScriptItemsStack[1024];
    MD_SCRIPT_ITEM* pScriptItemsHeap = NULL;
    MD_SCRIPT_ITEM* pScriptItems = NULL;   /* Set to either pScriptItemsStack or pScriptItemsHeap. */

    MD_ASSERT(pFont != NULL);
    MD_ASSERT(pTextUTF16 != NULL);
    MD_ASSERT(pTextUTF16[0] != '\0');
    MD_ASSERT(textLength > 0);
    MD_ASSERT(pItemCount != NULL);
    
    /* Uniscribe doesn't need the itemization state so we just clear it to zero. */
    MD_ZERO_OBJECT(pItemizeState);

    /* ScriptItemize() uses an integer for the text length, so keep it simple and return an error if we're asking for more. */
    if (textLength > 0x7FFFFFFF) {
        return MD_INVALID_ARGS;
    }

    itemCapacity = *pItemCount;

    /*
    Unfortunately ScriptItemize() is kind of bad when it comes to counting the number of items in the string. From the documentation:
    
        If the function returns E_OUTOFMEMORY, the application can call it again with a larger pItems buffer.

    This means that the way to do this is to repeatedly call ScriptItemize() with ever increasing buffer sizes. We first try using a
    stack-allocated buffer. If this is too small we will use the heap.
    */

    /* Try itemizing into the stack first. If this fails we fall back to the heap. */
    hResult = ((MD_PFN_ScriptItemize)pFont->pAPI->gdi.ScriptItemize)((const wchar_t*)pTextUTF16, (int)textLength, MD_COUNTOF(pScriptItemsStack)-1, NULL, NULL, (SCRIPT_ITEM*)pScriptItemsStack, &scriptItemCount);   /* Subtract 1 from item capacity because the last item is always used as a null terminator. */
    if (hResult == S_OK) {
        pScriptItems = &pScriptItemsStack[0];
    } else {
        if (hResult == E_OUTOFMEMORY) {
            /* Not enough room in the stack so try the heap. We just continuously increase the size of the buffer until it's big enough. */
            size_t heapCap = MD_COUNTOF(pScriptItemsStack);
            while (hResult == E_OUTOFMEMORY) {
                MD_SCRIPT_ITEM* pNewScriptItemsHeap;

                heapCap *= 2;
                if (heapCap > 0x7FFFFFFF) {
                    MD_FREE(pScriptItemsHeap);
                    return MD_INVALID_ARGS; /* String is too long for Uniscribe. */
                }

                pNewScriptItemsHeap = (MD_SCRIPT_ITEM*)MD_REALLOC(pScriptItemsHeap, sizeof(*pNewScriptItemsHeap) * heapCap);
                if (pNewScriptItemsHeap == NULL) {
                    MD_FREE(pScriptItemsHeap);
                    return MD_OUT_OF_MEMORY;
                }

                pScriptItemsHeap = pNewScriptItemsHeap;

                hResult = ((MD_PFN_ScriptItemize)pFont->pAPI->gdi.ScriptItemize)((const wchar_t*)pTextUTF16, (int)textLength, (int)(heapCap-1), NULL, NULL, (SCRIPT_ITEM*)pScriptItemsHeap, &scriptItemCount);   /* Subtract 1 from item capacity because the last item is always used as a null terminator. */
            }

            pScriptItems = pScriptItemsHeap;
        }
    }

    if (hResult != S_OK) {
        MD_FREE(pScriptItemsHeap);
        return md_result_from_HRESULT(hResult); /* Something bad happened. */
    }

    /* Unfortunately Uniscribe splits "\r\n" into two separate "\r" and "\n" items. This is different to our specification so we need to combine them. */
    for (iItem = 0; iItem < scriptItemCount; /* Do nothing. */) {
        if (pTextUTF16[pScriptItems[iItem].iCharPos] == '\r' && ((size_t)pScriptItems[iItem+1].iCharPos < textLength && pTextUTF16[pScriptItems[iItem+1].iCharPos] == '\n')) {
            /* Combine. Assume the SCRIPT_ANALYSIS structure is the same. */
            int jItem;
            for (jItem = iItem+1; jItem+1 < scriptItemCount; jItem += 1) {
                pScriptItems[jItem] = pScriptItems[jItem+1];
            }

            scriptItemCount -= 1;
        } else {
            iItem += 1;
        }
    }

    /*
    Make sure the item count is always set, even in the event of future errors. This ensures the count is available in the event that the input capacity is too small in which
    case the caller can resize their input buffer accordingly.
    */
    *pItemCount = (md_uint32)scriptItemCount;


    /* At this point we have our SCRIPT_ITEM objects, so now we need to convert the SCRIPT_ITEM objects to md_item objects. */
    if (pItems != NULL) {
        if (itemCapacity >= (md_uint32)scriptItemCount) {
            for (iItem = 0; iItem < scriptItemCount; ++iItem) {
                pItems[iItem].offset            = (size_t)pScriptItems[iItem].iCharPos;
                pItems[iItem].length            = (size_t)(pScriptItems[iItem+1].iCharPos - pScriptItems[iItem].iCharPos);
                pItems[iItem].backend.gdi.sa    = pScriptItems[iItem].a;
                pItems[iItem].backend.gdi.hFont = pFont->gdi.hFont; /* For now always use the input font for all items, but we will likely want to implement some kind of font fallback system. Font fallback should be implemented in md_shape_utf16__gdi() for Unicode. */
                pItems[iItem].backend.gdi.sc    = pFont->gdi.sc;    /* ^^^ */
            }
        } else {
            MD_FREE(pScriptItemsHeap);
            return MD_NO_SPACE; /* Not enough room*/
        }
    }

    MD_FREE(pScriptItemsHeap);
    return MD_SUCCESS;
}

void md_free_itemize_state__gdi(md_itemize_state* pItemizeState)
{
    MD_ASSERT(pItemizeState != NULL);

    /* Nothing to do here. */
    (void)pItemizeState;
}

md_result md_shape_utf16__gdi(md_font* pFont, md_item* pItem, const md_utf16* pTextUTF16, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics)
{
    HRESULT hResult;
    HDC hDC = NULL;
    ABC abc;
    int glyphCount;
    size_t outputGlyphCap;
    WORD  pUniscribeClustersStack[4096];
    WORD* pUniscribeClustersHeap = NULL;
    WORD* pUniscribeClusters = NULL;
    void* pGlyphDataHeap = NULL;                    /* Single heap allocation for all per-glyph data. */
    WORD  pUniscribeGlyphsStack[4096];
    WORD* pUniscribeGlyphsHeap = NULL;              /* Offset of pGlyphDataHeap. */
    WORD* pUniscribeGlyphs;
    MD_SCRIPT_VISATTR  pUniscribeSVAStack[4096];
    MD_SCRIPT_VISATTR* pUniscribeSVAHeap = NULL;    /* Offset of pGlyphDataHeap. */
    MD_SCRIPT_VISATTR* pUniscribeSVA = NULL;
    int  pUniscribeAdvancesStack[4096];
    int* pUniscribeAdvancesHeap = NULL;             /* Offset of pGlyphDataHeap. */
    int* pUniscribeAdvances = NULL;
    GOFFSET  pUniscribeOffsetsStack[4096];
    GOFFSET* pUniscribeOffsetsHeap = NULL;          /* Offset of pGlyphDataHeap. */
    GOFFSET* pUniscribeOffsets = NULL;

    MD_ASSERT(pItem != NULL);
    MD_ASSERT(pFont != NULL);
    MD_ASSERT(pTextUTF16 != NULL);

    /* ScriptShape() uses an integer for the text length, so keep it simple and return an error if we're asking for more. */
    if (textLength > 0x7FFFFFFF) {
        return MD_INVALID_ARGS;
    }

    if (pGlyphCount != NULL) {
        if (*pGlyphCount > 0x7FFFFFFF) {
            return MD_INVALID_ARGS;
        }

        outputGlyphCap = *pGlyphCount;
    } else {
        outputGlyphCap = 0;
    }

    /* We need the same number of clusters as textLength. */
    if (textLength <= MD_COUNTOF(pUniscribeClustersStack)) {
        pUniscribeClusters = &pUniscribeClustersStack[0];
    } else {
        pUniscribeClustersHeap = (WORD*)MD_MALLOC(sizeof(*pUniscribeClustersHeap) * textLength);
        if (pUniscribeClustersHeap == NULL) {
            return MD_OUT_OF_MEMORY;
        }

        pUniscribeClusters = pUniscribeClustersHeap;
    }

    /* TODO: Font fallback. */

    /* We first try with a NULL DC. If this fails we need to select the font into the global DC and try again. */
    hResult = ((MD_PFN_ScriptShape)pFont->pAPI->gdi.ScriptShape)(NULL, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, MD_COUNTOF(pUniscribeGlyphsStack), (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsStack, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAStack, &glyphCount);
    if (hResult == E_PENDING) {
        /* Select the font into the global DC and try again, this time with the DC set to the global DC. */
        hDC = (HDC)pFont->pAPI->gdi.hGlobalDC;
        SelectObject(hDC, (HGDIOBJ)pFont->gdi.hFont);
        hResult = ((MD_PFN_ScriptShape)pFont->pAPI->gdi.ScriptShape)(hDC, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, MD_COUNTOF(pUniscribeGlyphsStack), (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsStack, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAStack, &glyphCount);
    }

    if (hResult == S_OK) {
        pUniscribeGlyphs   = &pUniscribeGlyphsStack[0];
        pUniscribeSVA      = &pUniscribeSVAStack[0];
        pUniscribeAdvances = &pUniscribeAdvancesStack[0];
        pUniscribeOffsets  = &pUniscribeOffsetsStack[0];
    } else {
        if (hResult == E_OUTOFMEMORY) {
            /* Not enough room in the stack, so try the heap. We need to continuously call ScriptShape() with ever increasing buffer sizes until it works. */
            size_t heapCap = MD_COUNTOF(pUniscribeGlyphsStack);
            while (hResult == E_OUTOFMEMORY) {
                void* pNewGlyphDataHeap;

                heapCap *= 2;
                if (heapCap > 0x7FFFFFFF) {
                    MD_FREE(pUniscribeClustersHeap);
                    MD_FREE(pGlyphDataHeap);
                    return MD_INVALID_ARGS; /* String is too long for Uniscribe. */
                }

                pNewGlyphDataHeap = MD_REALLOC(pGlyphDataHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap)+sizeof(*pUniscribeAdvancesHeap)+sizeof(*pUniscribeOffsetsHeap)) * heapCap);
                if (pNewGlyphDataHeap == NULL) {
                    MD_FREE(pUniscribeClustersHeap);
                    MD_FREE(pGlyphDataHeap);
                    return MD_OUT_OF_MEMORY;
                }

                pGlyphDataHeap = pNewGlyphDataHeap;

                pUniscribeGlyphsHeap   = (WORD*)pGlyphDataHeap;
                pUniscribeSVAHeap      = (MD_SCRIPT_VISATTR*)MD_OFFSET_PTR(pGlyphDataHeap, (sizeof(*pUniscribeGlyphsHeap))                                                            * heapCap);
                pUniscribeAdvancesHeap =               (int*)MD_OFFSET_PTR(pGlyphDataHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap))                                 * heapCap);
                pUniscribeOffsetsHeap  =           (GOFFSET*)MD_OFFSET_PTR(pGlyphDataHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeSVAHeap)+sizeof(*pUniscribeAdvancesHeap)) * heapCap);

                hResult = ((MD_PFN_ScriptShape)pFont->pAPI->gdi.ScriptShape)(hDC, (SCRIPT_CACHE*)&pFont->gdi.sc, (const WCHAR*)pTextUTF16, (int)textLength, (int)heapCap, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeGlyphsHeap, pUniscribeClusters, (SCRIPT_VISATTR*)pUniscribeSVAHeap, &glyphCount);
            }

            pUniscribeGlyphs   = pUniscribeGlyphsHeap;
            pUniscribeSVA      = pUniscribeSVAHeap;
            pUniscribeAdvances = pUniscribeAdvancesHeap;
            pUniscribeOffsets  = pUniscribeOffsetsHeap;
        } else {
            MD_FREE(pUniscribeClustersHeap);
            return md_result_from_HRESULT(hResult);
        }
    }

    /* Placement. */
    hResult = ((MD_PFN_ScriptPlace)pFont->pAPI->gdi.ScriptPlace)(hDC, (SCRIPT_CACHE*)&pFont->gdi.sc, pUniscribeGlyphs, (int)glyphCount, (const SCRIPT_VISATTR*)pUniscribeSVA, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, pUniscribeAdvances, pUniscribeOffsets, &abc);
    if (hResult != S_OK) {
        MD_FREE(pUniscribeClustersHeap);
        MD_FREE(pGlyphDataHeap);
        return md_result_from_HRESULT(hResult);
    }

    
    /* At this point we have the glyphs in Uniscribe format, so now we need to convert that to minidraw format. */
    if (pGlyphCount != NULL) {
        *pGlyphCount = glyphCount;  /* <-- Always make sure this is set to that the client can know how many glyphs are required in the event that MD_NO_SPACE is returned. */
    }

    /* The output array can be null in which case we just ignore it. */
    if (pGlyphs != NULL && (size_t)glyphCount <= outputGlyphCap) {
        size_t iGlyph;
        for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
            pGlyphs[iGlyph].backend.gdi.sv      = pUniscribeSVA[iGlyph];
            pGlyphs[iGlyph].index               = pUniscribeGlyphs[iGlyph];
            pGlyphs[iGlyph].advance             = pUniscribeAdvances[iGlyph];
            pGlyphs[iGlyph].backend.gdi.offsetX = pUniscribeOffsets[iGlyph].du;
            pGlyphs[iGlyph].backend.gdi.offsetY = pUniscribeOffsets[iGlyph].dv;
        }
    }

    if (pClusters != NULL) {
        size_t iCluster;
        for (iCluster = 0; iCluster < textLength; ++iCluster) {
            pClusters[iCluster] = pUniscribeClusters[iCluster];
        }
    }

    if (pRunMetrics != NULL) {
        pRunMetrics->lPadding = abc.abcA;
        pRunMetrics->rPadding = abc.abcC;
        pRunMetrics->sizeX    = abc.abcA + abc.abcB + abc.abcC;
        pRunMetrics->sizeY    = (pFont->metrics.ascent - pFont->metrics.descent);
    }


    MD_FREE(pUniscribeClustersHeap);
    MD_FREE(pGlyphDataHeap);

    /* Make sure MD_NO_SPACE is returned in the event that the output glyph count exceeds the capacity of the output buffer. */
    if (pGlyphCount != NULL && (size_t)glyphCount > outputGlyphCap) {
        return MD_NO_SPACE;
    } else {
        return MD_SUCCESS;
    }
}

void md_gc_draw_glyphs__gdi(md_gc* pGC, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32 x, md_int32 y)
{
    HRESULT hResult;
    size_t iGlyph;
    void* pHeap = NULL;
    WORD  pUniscribeGlyphsStack[4096];
    WORD* pUniscribeGlyphsHeap = NULL;              /* Offset of pHeap. */
    WORD* pUniscribeGlyphs;
    int  pUniscribeAdvancesStack[4096];
    int* pUniscribeAdvancesHeap = NULL;             /* Offset of pHeap. */
    int* pUniscribeAdvances = NULL;
    GOFFSET  pUniscribeOffsetsStack[4096];
    GOFFSET* pUniscribeOffsetsHeap = NULL;          /* Offset of pHeap. */
    GOFFSET* pUniscribeOffsets = NULL;

    /* ScriptTextOut() uses an integer for the glyph count, so keep it simple and return an error if we're asking for more. */
    if (glyphCount > 0x7FFFFFFF) {
        return; /* MD_INVALID_ARGS */
    }

    if (glyphCount <= MD_COUNTOF(pUniscribeAdvancesStack)) {
        /* Stack allocation. */
        pUniscribeGlyphs   = &pUniscribeGlyphsStack[0];
        pUniscribeAdvances = &pUniscribeAdvancesStack[0];
        pUniscribeOffsets  = &pUniscribeOffsetsStack[0];
    } else {
        /* Heap allocation. */
        pHeap = MD_MALLOC((sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeAdvancesHeap)+sizeof(*pUniscribeOffsetsHeap)) * glyphCount);
        if (pHeap == NULL) {
            return; /* MD_OUT_OF_MEMORY */
        }

        pUniscribeGlyphs   =    (WORD*)MD_OFFSET_PTR(pHeap, 0);
        pUniscribeAdvances =     (int*)MD_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap))                                 * glyphCount);
        pUniscribeOffsets  = (GOFFSET*)MD_OFFSET_PTR(pHeap, (sizeof(*pUniscribeGlyphsHeap)+sizeof(*pUniscribeAdvancesHeap)) * glyphCount);
    }

    for (iGlyph = 0; iGlyph < (size_t)glyphCount; ++iGlyph) {
        pUniscribeGlyphs[iGlyph]     = (WORD)pGlyphs[iGlyph].index;
        pUniscribeAdvances[iGlyph]   = pGlyphs[iGlyph].advance;
        pUniscribeOffsets[iGlyph].du = pGlyphs[iGlyph].backend.gdi.offsetX;
        pUniscribeOffsets[iGlyph].dv = pGlyphs[iGlyph].backend.gdi.offsetY;
    }

    SelectObject((HDC)pGC->gdi.hDC, (HFONT)pItem->backend.gdi.hFont);

    hResult = ((MD_PFN_ScriptTextOut)pGC->pAPI->gdi.ScriptTextOut)((HDC)pGC->gdi.hDC, &((md_item*)pItem)->backend.gdi.sc, x, y, /* fuOptions */ETO_OPAQUE, NULL, (SCRIPT_ANALYSIS*)&pItem->backend.gdi.sa, NULL, 0, pUniscribeGlyphs, (int)glyphCount, pUniscribeAdvances, NULL, pUniscribeOffsets);
    if (hResult != S_OK) {
        return; /* Error occurred. */
    }
}

void md_gc_clear__gdi(md_gc* pGC, md_color color)
{
    SaveDC((HDC)pGC->gdi.hDC);
    {
        md_uint32 sizeX;
        md_uint32 sizeY;
        md_gc_get_size__gdi(pGC, &sizeX, &sizeY);

        ModifyWorldTransform((HDC)pGC->gdi.hDC, NULL, MWT_IDENTITY);
        SetDCBrushColor((HDC)pGC->gdi.hDC, RGB(color.r, color.g, color.b));
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(DC_BRUSH));
        SelectObject((HDC)pGC->gdi.hDC, GetStockObject(NULL_PEN));
        Rectangle((HDC)pGC->gdi.hDC, 0, 0, (int)sizeX, (int)sizeY);
    }
    RestoreDC((HDC)pGC->gdi.hDC, -1);
}


void md_uninit__gdi(md_api* pAPI)
{
    MD_ASSERT(pAPI != NULL);

    if (pAPI->gdi.ownsGlobalDC) {
        DeleteDC((HDC)pAPI->gdi.hGlobalDC);
    }

    md_dlclose(pAPI->gdi.hMsimg32DLL);
    md_dlclose(pAPI->gdi.hGdi32DLL);
    md_dlclose(pAPI->gdi.hUsp10DLL);
}

md_result md_init__gdi(const md_api_config* pConfig, md_api* pAPI)
{
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pAPI != NULL);

    /* usp10.dll. Always do this before gdi32.dll. */
    pAPI->gdi.hUsp10DLL = md_dlopen("usp10.dll");
    if (pAPI->gdi.hUsp10DLL == NULL) {
        return MD_ERROR;    /* Failed to load usp10.dll. */
    }

    pAPI->gdi.ScriptItemize   = md_dlsym(pAPI->gdi.hUsp10DLL, "ScriptItemize");
    pAPI->gdi.ScriptShape     = md_dlsym(pAPI->gdi.hUsp10DLL, "ScriptShape");
    pAPI->gdi.ScriptPlace     = md_dlsym(pAPI->gdi.hUsp10DLL, "ScriptPlace");
    pAPI->gdi.ScriptTextOut   = md_dlsym(pAPI->gdi.hUsp10DLL, "ScriptTextOut");
    pAPI->gdi.ScriptFreeCache = md_dlsym(pAPI->gdi.hUsp10DLL, "ScriptFreeCache");


    /* gdi32.dll */
    pAPI->gdi.hGdi32DLL = md_dlopen("gdi32.dll");
    if (pAPI->gdi.hGdi32DLL == NULL) {
        md_dlclose(pAPI->gdi.hUsp10DLL);
        return MD_ERROR;    /* Failed to load gdi32.dll. */
    }


    /* msimg32.dll */
    pAPI->gdi.hMsimg32DLL = md_dlopen("msimg32.dll");
    if (pAPI->gdi.hMsimg32DLL == NULL) {
        md_dlclose(pAPI->gdi.hGdi32DLL);
        md_dlclose(pAPI->gdi.hUsp10DLL);
        return MD_ERROR;
    }

    pAPI->gdi.AlphaBlend = md_dlsym(pAPI->gdi.hMsimg32DLL, "AlphaBlend");


    /* We need a DC to use for creating a font and performing glyph shaping and placement. */
    if (pConfig->gdi.hDC != NULL) {
        pAPI->gdi.ownsGlobalDC = MD_FALSE;
        pAPI->gdi.hGlobalDC = pConfig->gdi.hDC;
    } else {
        pAPI->gdi.ownsGlobalDC = MD_TRUE;
        pAPI->gdi.hGlobalDC = CreateCompatibleDC(NULL);
        if (pAPI->gdi.hGlobalDC == NULL) {
            md_dlclose(pAPI->gdi.hMsimg32DLL);
            md_dlclose(pAPI->gdi.hGdi32DLL);
            md_dlclose(pAPI->gdi.hUsp10DLL);
            return MD_ERROR;    /* Failed to create global DC. */
        }
    }


    /* Stock objects. */
    pAPI->gdi.hStockSolidFillBrush = GetStockObject(DC_BRUSH);    /* The brush to use for solid brushes. */

    pAPI->procs.uninit                     = md_uninit__gdi;
    pAPI->procs.itemizeUTF8                = NULL;   /* No native support for UTF-8 with Uniscribe. */
    pAPI->procs.itemizeUTF16               = md_itemize_utf16__gdi;
    pAPI->procs.itemizeUTF32               = NULL;   /* No native support for UTF-32 with Uniscribe. */
    pAPI->procs.freeItemizeState           = md_free_itemize_state__gdi;
    pAPI->procs.shapeUTF8                  = NULL;   /* No native support for UTF-8 with Uniscribe. */
    pAPI->procs.shapeUTF16                 = md_shape_utf16__gdi;
    pAPI->procs.shapeUTF32                 = NULL;   /* No native support for UTF-32 with Uniscribe. */
    pAPI->procs.fontInit                   = md_font_init__gdi;
    pAPI->procs.fontUninit                 = md_font_uninit__gdi;
    pAPI->procs.fontGetGlyphMetrics        = md_font_get_glyph_metrics__gdi;
    pAPI->procs.fontGetGlyphMetricsByIndex = md_font_get_glyph_metrics_by_index__gdi;
    pAPI->procs.brushInit                  = md_brush_init__gdi;
    pAPI->procs.brushUninit                = md_brush_uninit__gdi;
    pAPI->procs.brushSetOrigin             = md_brush_set_origin__gdi;
    pAPI->procs.gcInit                     = md_gc_init__gdi;
    pAPI->procs.gcUninit                   = md_gc_uninit__gdi;
    pAPI->procs.gcGetImageData             = md_gc_get_image_data__gdi;
    pAPI->procs.gcGetSize                  = md_gc_get_size__gdi;
    pAPI->procs.gcSave                     = md_gc_save__gdi;
    pAPI->procs.gcRestore                  = md_gc_restore__gdi;
    pAPI->procs.gcSetMatrix                = md_gc_set_matrix__gdi;
    pAPI->procs.gcGetMatrix                = md_gc_get_matrix__gdi;
    pAPI->procs.gcSetMatrixIdentity        = md_gc_set_matrix_identity__gdi;
    pAPI->procs.gcTransform                = md_gc_transform__gdi;
    pAPI->procs.gcTranslate                = md_gc_translate__gdi;
    pAPI->procs.gcRotate                   = md_gc_rotate__gdi;
    pAPI->procs.gcScale                    = md_gc_scale__gdi;
    pAPI->procs.gcSetLineWidth             = md_gc_set_line_width__gdi;
    pAPI->procs.gcSetLineCap               = md_gc_set_line_cap__gdi;
    pAPI->procs.gcSetLineJoin              = md_gc_set_line_join__gdi;
    pAPI->procs.gcSetMiterLimit            = md_gc_set_miter_limit__gdi;
    pAPI->procs.gcSetLineDash              = md_gc_set_line_dash__gdi;
    pAPI->procs.gcSetLineBrush             = md_gc_set_line_brush__gdi;
    pAPI->procs.gcSetLineBrushSolid        = md_gc_set_line_brush_solid__gdi;
    pAPI->procs.gcSetLineBrushGC           = md_gc_set_line_brush_gc__gdi;
    pAPI->procs.gcSetFillBrush             = md_gc_set_fill_brush__gdi;
    pAPI->procs.gcSetFillBrushSolid        = md_gc_set_fill_brush_solid__gdi;
    pAPI->procs.gcSetFillBrushGC           = md_gc_set_fill_brush_gc__gdi;
    pAPI->procs.gcSetTextFGColor           = md_gc_set_text_fg_color__gdi;
    pAPI->procs.gcSetTextBGColor           = md_gc_set_text_bg_color__gdi;
    pAPI->procs.gcGetTextFGColor           = md_gc_get_text_fg_color__gdi;
    pAPI->procs.gcGetTextBGColor           = md_gc_get_text_bg_color__gdi;
    pAPI->procs.gcSetBlendOp               = md_gc_set_blend_op__gdi;
    pAPI->procs.gcSetAntialiasMode         = md_gc_set_antialias_mode__gdi;
    pAPI->procs.gcSetFillMode              = md_gc_set_fill_mode__gdi;
    pAPI->procs.gcSetStretchFilter         = md_gc_set_stretch_filter__gdi;
    pAPI->procs.gcMoveTo                   = md_gc_move_to__gdi;
    pAPI->procs.gcLineTo                   = md_gc_line_to__gdi;
    pAPI->procs.gcRectangle                = md_gc_rectangle__gdi;
    pAPI->procs.gcArc                      = md_gc_arc__gdi;
    pAPI->procs.gcCurveTo                  = md_gc_curve_to__gdi;
    pAPI->procs.gcClosePath                = md_gc_close_path__gdi;
    pAPI->procs.gcClip                     = md_gc_clip__gdi;
    pAPI->procs.gcResetClip                = md_gc_reset_clip__gdi;
    pAPI->procs.gcIsPointInsideClip        = md_gc_is_point_inside_clip__gdi;
    pAPI->procs.gcFill                     = md_gc_fill__gdi;
    pAPI->procs.gcStroke                   = md_gc_stroke__gdi;
    pAPI->procs.gcFillAndStroke            = md_gc_fill_and_stroke__gdi;
    pAPI->procs.gcDrawGC                   = md_gc_draw_gc__gdi;
    pAPI->procs.gcDrawGlyphs               = md_gc_draw_glyphs__gdi;
    pAPI->procs.gcClear                    = md_gc_clear__gdi;

    (void)pConfig;
    return MD_SUCCESS;
}
#endif


/**************************************************************************************************************************************************************

 Direct2D + DirectWrite

 **************************************************************************************************************************************************************/
#if defined(MD_HAS_DIRECT2D)
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
#if defined(MD_HAS_COREGRAPHICS)

/* Font */

/* Graphics */

/* API */

#endif


/**************************************************************************************************************************************************************

 Cairo + Pango

 **************************************************************************************************************************************************************/
#if defined(MD_HAS_CAIRO)
#include <pango/pangocairo.h>

PangoStyle md_PangoStyle_from_slant__pango(md_font_slant slant)
{
    switch (slant)
    {
        case md_font_slant_none:    return PANGO_STYLE_NORMAL;
        case md_font_slant_oblique: return PANGO_STYLE_OBLIQUE;
        case md_font_slant_italic:  return PANGO_STYLE_ITALIC;
        default:                    return PANGO_STYLE_NORMAL;
    }
}

PangoWeight md_PangoWeight_from_weight__pango(md_font_weight weight)
{
    switch (weight)
    {
        case md_font_weight_medium:      return PANGO_WEIGHT_NORMAL;    /* There is actually a PANGO_WEIGHT_MEDIUM, which is different to PANGO_WEIGHT_NORMAL, however in minidraw, normal and medium are the same. */
        case md_font_weight_thin:        return PANGO_WEIGHT_THIN;
        case md_font_weight_extra_light: return PANGO_WEIGHT_ULTRALIGHT;
        case md_font_weight_light:       return PANGO_WEIGHT_LIGHT;
        case md_font_weight_semi_light:  return PANGO_WEIGHT_SEMILIGHT;
        case md_font_weight_book:        return PANGO_WEIGHT_BOOK;
        case md_font_weight_semi_bold:   return PANGO_WEIGHT_SEMIBOLD;
        case md_font_weight_bold:        return PANGO_WEIGHT_BOLD;
        case md_font_weight_extra_bold:  return PANGO_WEIGHT_ULTRABOLD;
        case md_font_weight_heavy:       return PANGO_WEIGHT_HEAVY;
        case md_font_weight_extra_heavy: return PANGO_WEIGHT_ULTRAHEAVY;
        default:                         return PANGO_WEIGHT_NORMAL;
    }
}

/* Font */
md_result md_font_init__cairo(md_api* pAPI, const md_font_config* pConfig, md_font* pFont)
{
    PangoFontMap* pPangoFontMap;
    PangoContext* pPangoContext;
    PangoFontDescription* pPangoFontDesc;
    PangoFont* pPangoFont;
    PangoFontMetrics* pPangoFontMetrics;
    PangoAttrList* pPangoAttrList;

    MD_ASSERT(pAPI != NULL);
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pFont != NULL);

    pPangoFontMap = pConfig->cairo.pPangoFontMap;
    if (pPangoFontMap == NULL) {
        pPangoFontMap = pango_cairo_font_map_get_default();
    }

    pPangoContext = pConfig->cairo.pPangoContext;
    if (pPangoContext == NULL) {
        pPangoContext = (PangoContext*)pAPI->cairo.pPangoContext;
    }

    pPangoFontDesc = pango_font_description_new();
    if (pPangoFontDesc == NULL) {
        return MD_OUT_OF_MEMORY;    /* Cannot think of a reason why this would fail other than out-of-memory. */
    }

    pango_font_description_set_family_static(pPangoFontDesc, pConfig->family);
    pango_font_description_set_style(pPangoFontDesc, md_PangoStyle_from_slant__pango(pConfig->slant));
    pango_font_description_set_weight(pPangoFontDesc, md_PangoWeight_from_weight__pango(pConfig->weight));
    if (pConfig->sizeInPixels > 0) {
        pango_font_description_set_absolute_size(pPangoFontDesc, pConfig->sizeInPixels*PANGO_SCALE);
    } else {
        pango_font_description_set_size(pPangoFontDesc, pConfig->sizeInPoints*PANGO_SCALE);
    }

    pPangoFont = pango_font_map_load_font(pPangoFontMap, pPangoContext, pPangoFontDesc);
    if (pPangoFont == NULL) {
        pango_font_description_free(pPangoFontDesc);
        return MD_ERROR;    /* Failed to load the requested font. */
    }


    /* Now that we have the font description we need to build a PangoAttrList with that description. The PangoAttrList object is passed to pango_itemize(). */
    pPangoAttrList = pango_attr_list_new();
    if (pPangoAttrList == NULL) {
        g_object_unref((PangoFont*)pFont->cairo.pPangoFont);
        pango_font_description_free(pPangoFontDesc);
        return MD_OUT_OF_MEMORY;
    }

    pango_attr_list_insert(pPangoAttrList, pango_attr_font_desc_new(pPangoFontDesc));


    /* We should have the font at this point, so now we need to retrieve the metrics. */
    pPangoFontMetrics = pango_font_get_metrics(pPangoFont, NULL);
    pFont->metrics.ascent  =  pango_font_metrics_get_ascent(pPangoFontMetrics)  / PANGO_SCALE;
    pFont->metrics.descent = -pango_font_metrics_get_descent(pPangoFontMetrics) / PANGO_SCALE;
    pango_font_metrics_unref(pPangoFontMetrics);

    pFont->cairo.pPangoFont = pPangoFont;
    pFont->cairo.pPangoFontDesc = pPangoFontDesc;
    pFont->cairo.pPangoAttrList = pPangoAttrList;

    return MD_SUCCESS;
}

void md_font_uninit__cairo(md_font* pFont)
{
    MD_ASSERT(pFont != NULL);

    pango_attr_list_unref((PangoAttrList*)pFont->cairo.pPangoAttrList);
    pFont->cairo.pPangoAttrList = NULL;

    g_object_unref((PangoFont*)pFont->cairo.pPangoFont);
    pFont->cairo.pPangoFont = NULL;

    pango_font_description_free((PangoFontDescription*)pFont->cairo.pPangoFontDesc);
    pFont->cairo.pPangoFontDesc = NULL;
}

MD_PRIVATE void md_font_get_glyph_metrics_single__cairo(cairo_scaled_font_t* pCairoFont, md_uint32 glyphIndex, md_glyph_metrics* pGlyphMetrics)
{
    cairo_text_extents_t cairoMetrics;
    cairo_glyph_t cairoGlyph;

    cairoGlyph.index = glyphIndex;
    cairoGlyph.x = 0;
    cairoGlyph.y = 0;

    cairo_scaled_font_glyph_extents(pCairoFont, &cairoGlyph, 1, &cairoMetrics);

    pGlyphMetrics->sizeX    = cairoMetrics.width     / PANGO_SCALE;
    pGlyphMetrics->sizeY    = cairoMetrics.height    / PANGO_SCALE;
    pGlyphMetrics->bearingX = cairoMetrics.x_bearing / PANGO_SCALE;
    pGlyphMetrics->bearingY = cairoMetrics.y_bearing / PANGO_SCALE;
    pGlyphMetrics->advanceX = cairoMetrics.x_advance / PANGO_SCALE;
    pGlyphMetrics->advanceY = cairoMetrics.y_advance / PANGO_SCALE;
}

md_result md_font_get_glyph_metrics__cairo(md_font* pFont, const md_glyph* pGlyphs, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    size_t iGlyph;
    cairo_scaled_font_t* pCairoFont;

    MD_ASSERT(pFont         != NULL);
    MD_ASSERT(pGlyphs       != NULL);
    MD_ASSERT(pGlyphMetrics != NULL);

    pCairoFont = pango_cairo_font_get_scaled_font((PangoCairoFont*)pFont->cairo.pPangoFont);
    if (pCairoFont == NULL) {
        return MD_ERROR;    /* Failed to retrieve the cairo_scaled_font_t* object from the PangoFont. */
    }

    /* TODO: I think this can be optimized by doing it in batches, since cairo_scaled_font_glyph_extents() takes an array of glyphs as input. */
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        md_font_get_glyph_metrics_single__cairo(pCairoFont, pGlyphs[iGlyph].index, &pGlyphMetrics[iGlyph]);
    }

    return MD_SUCCESS;
}

md_result md_font_get_glyph_metrics_by_index__cairo(md_font* pFont, const md_uint32* pGlyphIndices, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    size_t iGlyph;
    cairo_scaled_font_t* pCairoFont;

    MD_ASSERT(pFont         != NULL);
    MD_ASSERT(pGlyphIndices != NULL);
    MD_ASSERT(pGlyphMetrics != NULL);

    pCairoFont = pango_cairo_font_get_scaled_font((PangoCairoFont*)pFont->cairo.pPangoFont);
    if (pCairoFont == NULL) {
        return MD_ERROR;    /* Failed to retrieve the cairo_scaled_font_t* object from the PangoFont. */
    }

    /* TODO: I think this can be optimized by doing it in batches, since cairo_scaled_font_glyph_extents() takes an array of glyphs as input. */
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        md_font_get_glyph_metrics_single__cairo(pCairoFont, pGlyphIndices[iGlyph], &pGlyphMetrics[iGlyph]);
    }

    return MD_SUCCESS;
}


/* Brush */
md_result md_brush_init__cairo(md_api* pAPI, const md_brush_config* pConfig, md_brush* pBrush)
{
    cairo_pattern_t* pCairoPattern = NULL;

    MD_ASSERT(pAPI    != NULL);
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pBrush  != NULL);

    (void)pAPI;

    switch (pConfig->type)
    {
        case md_brush_type_solid:
        {
            pCairoPattern = cairo_pattern_create_rgba(pConfig->solid.color.r/255.0, pConfig->solid.color.g/255.0, pConfig->solid.color.b/255.0, pConfig->solid.color.a/255.0);
            if (pCairoPattern == NULL) {
                return MD_ERROR;    /* Failed to create the pattern. */
            }
        } break;

        case md_brush_type_gc:
        {
            if (pConfig->gc.pGC == NULL) {
                return MD_INVALID_ARGS;
            }

            if (pConfig->gc.pGC->isTransient) {
                return MD_INVALID_ARGS; /* Creating a GC brush from a transient Cairo context is not supported. */
            } else {
                if (pConfig->gc.pGC->cairo.pCairoSurface == NULL) {
                    return MD_INVALID_ARGS; /* Source GC is not in a valid state. */
                }

                pCairoPattern = cairo_pattern_create_for_surface(pConfig->gc.pGC->cairo.pCairoSurface);
            }
        } break;
    }

    if (pCairoPattern == NULL) {
        return MD_ERROR;
    }

    /* We need to check the status of the surface. */
    if (cairo_pattern_status(pCairoPattern) != CAIRO_STATUS_SUCCESS) {
        cairo_pattern_destroy(pCairoPattern);
        return MD_ERROR;    /* An error occurred when creating the pattern. */
    }

    pBrush->cairo.pCairoPattern = pCairoPattern;

    return MD_SUCCESS;
}

void md_brush_uninit__cairo(md_brush* pBrush)
{
    MD_ASSERT(pBrush != NULL);

    cairo_pattern_destroy((cairo_pattern_t*)pBrush->cairo.pCairoPattern);

    if (pBrush->cairo.pCairoSurface != NULL) {
        cairo_surface_destroy(pBrush->cairo.pCairoSurface);
    }

    MD_ZERO_OBJECT(pBrush); /* Safety. */
}

void md_brush_set_origin__cairo(md_brush* pBrush, md_int32 x, md_int32 y)
{
    cairo_matrix_t m;

    MD_ASSERT(pBrush != NULL);

    cairo_matrix_init_translate(&m, (double)x, (double)y);
    cairo_pattern_set_matrix((cairo_pattern_t*)pBrush->cairo.pCairoPattern, &m);
}


/* Graphics */
md_result md_gc_init__cairo(md_api* pAPI, const md_gc_config* pConfig, md_gc* pGC)
{
    MD_ASSERT(pAPI    != NULL);
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pGC     != NULL);

    (void)pAPI;

    /* For accurate size retrieval, the size needs to be set in the config, even for transient GC's. */
    pGC->cairo.cairoSurfaceSizeX = pConfig->sizeX;
    pGC->cairo.cairoSurfaceSizeY = pConfig->sizeY;

    if (pConfig->cairo.pCairoContext != NULL) {
        pGC->isTransient = MD_TRUE;
        pGC->format = md_format_unknown;
        pGC->cairo.pCairoContext = pConfig->cairo.pCairoContext;
    } else {
        cairo_surface_t* pCairoSurface;

        pGC->isTransient = MD_FALSE;
        pGC->format = pConfig->format;

        /* We need a surface before we can create the Cairo context. */
        pGC->cairo.pCairoSurfaceData = MD_MALLOC(pConfig->sizeX * pConfig->sizeY * md_get_bytes_per_pixel(md_format_argb));
        if (pGC->cairo.pCairoSurfaceData == NULL) {
            return MD_OUT_OF_MEMORY;
        }

        if (pConfig->pInitialImageData != NULL) {
            /* Image data needs to be converted. */
            md_copy_image_data(pGC->cairo.pCairoSurfaceData, pConfig->pInitialImageData, pConfig->sizeX, pConfig->sizeY, 0, md_format_argb, pConfig->stride, pConfig->format);
        } else {
            /* Clear to zero initially. */
            MD_ZERO_MEMORY(pGC->cairo.pCairoSurfaceData, pConfig->sizeX * pConfig->sizeY * md_get_bytes_per_pixel(md_format_argb));
        }

        pCairoSurface = cairo_image_surface_create_for_data((unsigned char*)pGC->cairo.pCairoSurfaceData, CAIRO_FORMAT_ARGB32, (int)pConfig->sizeX, (int)pConfig->sizeY, (int)pConfig->sizeX*4);
        if (pCairoSurface == NULL) {
            MD_FREE(pGC->cairo.pCairoSurfaceData);
            pGC->cairo.pCairoSurfaceData = NULL;
            return MD_ERROR;    /* Failed to create cairo_surface_t object. */
        }

        pGC->cairo.pCairoContext = (md_ptr)cairo_create(pCairoSurface);
        if (pGC->cairo.pCairoContext == NULL) {
            cairo_surface_destroy(pCairoSurface);
            MD_FREE(pGC->cairo.pCairoSurfaceData);
            pGC->cairo.pCairoSurfaceData = NULL;
            return MD_ERROR;    /* Failed to create cairo_t object. */
        }

        pGC->cairo.pCairoSurface = pCairoSurface;

        /* Set defaults here. */
        cairo_set_operator((cairo_t*)pGC->cairo.pCairoContext, CAIRO_OPERATOR_SOURCE);
    }

    /* We need at least one item in the state stack. Unfortunate malloc() here - may want to think about optimizing this. Perhaps some optimized per-API scheme? */
    pGC->cairo.stateCount = 1;
    pGC->cairo.stateCap   = 1;
    pGC->cairo.pState = (md_gc_state_cairo*)MD_CALLOC(pGC->cairo.stateCap, sizeof(*pGC->cairo.pState));
    if (pGC->cairo.pState == NULL) {
        if (!pGC->isTransient) {
            cairo_destroy((cairo_t*)pGC->cairo.pCairoContext);
            pGC->cairo.pCairoContext = NULL;

            cairo_surface_destroy((cairo_surface_t*)pGC->cairo.pCairoSurface);
            pGC->cairo.pCairoSurface = NULL;

            MD_FREE(pGC->cairo.pCairoSurfaceData);
            pGC->cairo.pCairoSurfaceData = NULL;
        }
        return MD_OUT_OF_MEMORY;
    }

    /* The default text color needs to be opaque black. */
    pGC->cairo.pState[0].textFGColor = md_rgba(0, 0, 0, 255);

    /* The line width needs to default to 1. */
    cairo_set_line_width((cairo_t*)pGC->cairo.pCairoContext, 1);

    return MD_SUCCESS;
}

void md_gc_uninit__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    if (!pGC->isTransient) {
        cairo_destroy((cairo_t*)pGC->cairo.pCairoContext);
        pGC->cairo.pCairoContext = NULL;
    }

    if (pGC->cairo.pCairoSurface != NULL) {
        cairo_surface_destroy((cairo_surface_t*)pGC->cairo.pCairoSurface);
        pGC->cairo.pCairoSurface = NULL;
    }

    MD_FREE(pGC->cairo.pCairoSurfaceData);
    pGC->cairo.pCairoSurfaceData = NULL;
}

md_result md_gc_get_size__cairo(md_gc* pGC, md_uint32* pSizeX, md_uint32* pSizeY)
{
    MD_ASSERT(pGC    != NULL);
    MD_ASSERT(pSizeX != NULL);
    MD_ASSERT(pSizeY != NULL);

    *pSizeX = pGC->cairo.cairoSurfaceSizeX;
    *pSizeY = pGC->cairo.cairoSurfaceSizeY;

    return MD_SUCCESS;
}

md_result md_gc_get_image_data__cairo(md_gc* pGC, md_format outputFormat, void* pImageData)
{
    MD_ASSERT(pGC != NULL);

    /* Cannot currently do this on transient GC's. */
    if (pGC->isTransient || pGC->cairo.pCairoSurface == NULL) {
        return MD_INVALID_OPERATION;
    }

    cairo_surface_flush((cairo_surface_t*)pGC->cairo.pCairoSurface);
    md_copy_image_data(pImageData, pGC->cairo.pCairoSurfaceData, pGC->cairo.cairoSurfaceSizeX, pGC->cairo.cairoSurfaceSizeY, 0, outputFormat, 0, md_format_bgra);

    return MD_SUCCESS;
}

md_result md_gc_save__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pGC->cairo.stateCount > 0);

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);

    if (pGC->cairo.stateCount == pGC->cairo.stateCap) {
        md_uint32 newCap = MD_MAX(1, pGC->cairo.stateCap * 2);
        md_gc_state_cairo* pNewState = (md_gc_state_cairo*)MD_REALLOC(pGC->cairo.pState, newCap * sizeof(*pNewState));
        if (pNewState == NULL) {
            return MD_OUT_OF_MEMORY;
        }

        pGC->cairo.pState   = pNewState;
        pGC->cairo.stateCap = newCap;
    }

    MD_ASSERT(pGC->cairo.stateCount < pGC->cairo.stateCap);

    pGC->cairo.pState[pGC->cairo.stateCount] = pGC->cairo.pState[pGC->cairo.stateCount-1];
    pGC->cairo.stateCount += 1;

    return MD_SUCCESS;
}

md_result md_gc_restore__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pGC->cairo.stateCount > 0);

    if (pGC->cairo.stateCount == 1) {
        return MD_INVALID_OPERATION;    /* Nothing to restore. */
    }

    pGC->cairo.stateCount -= 1;

    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);

    return MD_SUCCESS;
}

void md_gc_set_matrix__cairo(md_gc* pGC, const md_matrix* pMatrix)
{
    cairo_matrix_t m;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    cairo_matrix_init(
        &m,
        pMatrix->m00, pMatrix->m01,
        pMatrix->m10, pMatrix->m11,
        pMatrix->dx,  pMatrix->dy
    );

    cairo_set_matrix((cairo_t*)pGC->cairo.pCairoContext, &m);
}

void md_gc_get_matrix__cairo(md_gc* pGC, md_matrix* pMatrix)
{
    cairo_matrix_t m;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    cairo_get_matrix((cairo_t*)pGC->cairo.pCairoContext, &m);

    pMatrix->m00 = m.xx;
    pMatrix->m01 = m.xy;
    pMatrix->m10 = m.yx;
    pMatrix->m11 = m.yy;
    pMatrix->dx  = m.x0;
    pMatrix->dy  = m.y0;
}

void md_gc_set_matrix_identity__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_identity_matrix((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_transform__cairo(md_gc* pGC, const md_matrix* pMatrix)
{
    cairo_matrix_t m;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pMatrix != NULL);

    cairo_matrix_init(
        &m,
        pMatrix->m00, pMatrix->m01,
        pMatrix->m10, pMatrix->m11,
        pMatrix->dx,  pMatrix->dy
    );

    cairo_transform((cairo_t*)pGC->cairo.pCairoContext, &m);
}

void md_gc_translate__cairo(md_gc* pGC, md_int32 offsetX, md_int32 offsetY)
{
    MD_ASSERT(pGC != NULL);

    cairo_translate((cairo_t*)pGC->cairo.pCairoContext, (double)offsetX, (double)offsetY);
}

void md_gc_rotate__cairo(md_gc* pGC, float rotationInRadians)
{
    MD_ASSERT(pGC != NULL);

    cairo_rotate((cairo_t*)pGC->cairo.pCairoContext, (double)rotationInRadians);
}

void md_gc_scale__cairo(md_gc* pGC, float scaleX, float scaleY)
{
    MD_ASSERT(pGC != NULL);

    cairo_scale((cairo_t*)pGC->cairo.pCairoContext, (double)scaleX, (double)scaleY);
}

void md_gc_set_miter_limit__cairo(md_gc* pGC, float limit)
{
    MD_ASSERT(pGC != NULL);

    cairo_set_miter_limit((cairo_t*)pGC->cairo.pCairoContext, (double)limit);
}

void md_gc_set_line_width__cairo(md_gc* pGC, md_int32 width)
{
    MD_ASSERT(pGC != NULL);

    cairo_set_line_width((cairo_t*)pGC->cairo.pCairoContext, (double)width);
}

void md_gc_set_line_cap__cairo(md_gc* pGC, md_line_cap cap)
{
    cairo_line_cap_t capCairo;

    MD_ASSERT(pGC != NULL);
    
    switch (cap)
    {
        case md_line_cap_square:
        {
            capCairo = CAIRO_LINE_CAP_SQUARE;
        } break;
        case md_line_cap_round:
        {
            capCairo = CAIRO_LINE_CAP_ROUND;
        } break;
        case md_line_cap_flat:
        default:
        {
            capCairo = CAIRO_LINE_CAP_BUTT;
        } break;
    }

    cairo_set_line_cap((cairo_t*)pGC->cairo.pCairoContext, capCairo);
}

void md_gc_set_line_join__cairo(md_gc* pGC, md_line_join join)
{
    cairo_line_join_t joinCairo;

    MD_ASSERT(pGC != NULL);

    switch (join)
    {
        case md_line_join_bevel:
        {
            joinCairo = CAIRO_LINE_JOIN_BEVEL;
        } break;
        case md_line_join_round:
        {
            joinCairo = CAIRO_LINE_JOIN_ROUND;
        } break;
        case md_line_join_miter:
        default:
        {
            joinCairo = CAIRO_LINE_JOIN_MITER;
        } break;
    }

    cairo_set_line_join((cairo_t*)pGC->cairo.pCairoContext, joinCairo);
}

void md_gc_set_line_dash__cairo(md_gc* pGC, const float* dashes, md_uint32 count)
{
    double dashesD[16]; /* Max of 16. */
    md_uint32 iDash;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(count < 16);

    for (iDash = 0; iDash < count; ++iDash) {
        dashesD[iDash] = (double)dashes[iDash];
    }

    cairo_set_dash((cairo_t*)pGC->cairo.pCairoContext, dashesD, count, 0);
}

void md_gc_set_line_brush__cairo(md_gc* pGC, md_brush* pBrush)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].pLineBrush = pBrush;
    pGC->cairo.pState[iState].hasTransientLineBrush = MD_FALSE;
}

void md_gc_set_line_brush_solid__cairo(md_gc* pGC, md_color color)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].transientLineBrush.type = md_brush_type_solid;
    pGC->cairo.pState[iState].transientLineBrush.solid.color = color;
    pGC->cairo.pState[iState].pLineBrush = NULL;
    pGC->cairo.pState[iState].hasTransientLineBrush = MD_TRUE;
}

void md_gc_set_line_brush_gc__cairo(md_gc* pGC, md_gc* pSrcGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].transientLineBrush.type = md_brush_type_gc;
    pGC->cairo.pState[iState].transientLineBrush.gc.pGC = pSrcGC;
    pGC->cairo.pState[iState].pLineBrush = NULL;
    pGC->cairo.pState[iState].hasTransientLineBrush = MD_TRUE;
}

void md_gc_set_fill_brush__cairo(md_gc* pGC, md_brush* pBrush)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].pFillBrush = pBrush;
    pGC->cairo.pState[iState].hasTransientFillBrush = MD_FALSE;
}

void md_gc_set_fill_brush_solid__cairo(md_gc* pGC, md_color color)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].transientFillBrush.type = md_brush_type_solid;
    pGC->cairo.pState[iState].transientFillBrush.solid.color = color;
    pGC->cairo.pState[iState].pFillBrush = NULL;
    pGC->cairo.pState[iState].hasTransientFillBrush = MD_TRUE;
}

void md_gc_set_fill_brush_gc__cairo(md_gc* pGC, md_gc* pSrcGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].transientFillBrush.type = md_brush_type_gc;
    pGC->cairo.pState[iState].transientFillBrush.gc.pGC = pSrcGC;
    pGC->cairo.pState[iState].pFillBrush = NULL;
    pGC->cairo.pState[iState].hasTransientFillBrush = MD_TRUE;
}

void md_gc_set_text_fg_color__cairo(md_gc* pGC, md_color fgColor)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].textFGColor = fgColor;
}

void md_gc_set_text_bg_color__cairo(md_gc* pGC, md_color bgColor)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].textBGColor = bgColor;
}

md_color md_gc_get_text_fg_color__cairo(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    return pGC->cairo.pState[iState].textFGColor;
}

md_color md_gc_get_text_bg_color__cairo(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    return pGC->cairo.pState[iState].textBGColor;
}

void md_gc_set_blend_op__cairo(md_gc* pGC, md_blend_op op)
{
    cairo_operator_t opCairo;

    MD_ASSERT(pGC != NULL);

    switch (op)
    {
        case md_blend_op_src:
        {
            opCairo = CAIRO_OPERATOR_SOURCE;
        } break;
        case md_blend_op_src_over:
        {
            opCairo = CAIRO_OPERATOR_OVER;
        } break;
        default:
        {
            opCairo = CAIRO_OPERATOR_SOURCE;
        } break;
    }

    cairo_set_operator((cairo_t*)pGC->cairo.pCairoContext, opCairo);
}

void md_gc_set_antialias_mode__cairo(md_gc* pGC, md_antialias_mode mode)
{
    cairo_antialias_t modeCairo;

    MD_ASSERT(pGC != NULL);

    switch (mode)
    {
        case md_antialias_mode_none:
        {
            modeCairo = CAIRO_ANTIALIAS_NONE;
        } break;

        case md_antialias_mode_gray:
        {
            modeCairo = CAIRO_ANTIALIAS_GRAY;
        } break;

        case md_antialias_mode_subpixel:
        {
            modeCairo = CAIRO_ANTIALIAS_SUBPIXEL;
        } break;

        case md_antialias_mode_default:
        default:
        {
            modeCairo = CAIRO_ANTIALIAS_DEFAULT;
        } break;
    }

    cairo_set_antialias((cairo_t*)pGC->cairo.pCairoContext, modeCairo);
}

void md_gc_set_fill_mode__cairo(md_gc* pGC, md_fill_mode mode)
{
    cairo_fill_rule_t modeCairo;

    MD_ASSERT(pGC != NULL);

    if (mode == md_fill_mode_evenodd) {
        modeCairo = CAIRO_FILL_RULE_EVEN_ODD;
    } else {
        modeCairo = CAIRO_FILL_RULE_WINDING;
    }

    cairo_set_fill_rule((cairo_t*)pGC->cairo.pCairoContext, modeCairo);
}

void md_gc_set_stretch_filter__cairo(md_gc* pGC, md_stretch_filter filter)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;
    pGC->cairo.pState[iState].stretchFilter = filter;
}

void md_gc_move_to__cairo(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    cairo_move_to((cairo_t*)pGC->cairo.pCairoContext, (double)x, (double)y);
}

void md_gc_line_to__cairo(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    cairo_line_to((cairo_t*)pGC->cairo.pCairoContext, (double)x, (double)y);
}

void md_gc_rectangle__cairo(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom)
{
    MD_ASSERT(pGC != NULL);

    cairo_rectangle((cairo_t*)pGC->cairo.pCairoContext, (double)left, (double)top, (double)(right - left), (double)(bottom - top));
}

void md_gc_arc__cairo(md_gc* pGC, md_int32 x, md_int32 y, md_int32 radius, float angle1InRadians, float angle2InRadians)
{
    MD_ASSERT(pGC != NULL);

    cairo_arc((cairo_t*)pGC->cairo.pCairoContext, (double)x, (double)y, (double)radius, (double)angle1InRadians, (double)angle2InRadians);
}

void md_gc_curve_to__cairo(md_gc* pGC, md_int32 x1, md_int32 y1, md_int32 x2, md_int32 y2, md_int32 x3, md_int32 y3)
{
    MD_ASSERT(pGC != NULL);

    cairo_curve_to((cairo_t*)pGC->cairo.pCairoContext, (double)x1, (double)x2, (double)y1, (double)y2, (double)x3, (double)y3);
}

void md_gc_close_path__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_close_path((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_clip__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_clip((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_reset_clip__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_reset_clip((cairo_t*)pGC->cairo.pCairoContext);
}

md_bool32 md_gc_is_point_inside_clip__cairo(md_gc* pGC, md_int32 x, md_int32 y)
{
    MD_ASSERT(pGC != NULL);

    return (md_bool32)cairo_in_clip((cairo_t*)pGC->cairo.pCairoContext, (double)x, (double)y);
}


MD_PRIVATE cairo_filter_t md_to_cairo_filter(md_stretch_filter filter)
{
    cairo_filter_t filterCairo;
    if (filter == md_stretch_filter_nearest) {
        filterCairo = CAIRO_FILTER_NEAREST;
    } else {
        filterCairo = CAIRO_FILTER_BILINEAR;
    }

    return filterCairo;
}

MD_PRIVATE void md_gc_set_source_from_brush_config__cairo(md_gc* pGC, const md_brush_config* pConfig)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pConfig != NULL);

    iState = pGC->cairo.stateCount-1;

    switch (pConfig->type)
    {
        case md_brush_type_solid:
        {
            cairo_set_source_rgba((cairo_t*)pGC->cairo.pCairoContext, pConfig->solid.color.r/255.0, pConfig->solid.color.g/255.0, pConfig->solid.color.b/255.0, pConfig->solid.color.a/255.0);
        } break;
        /*case md_brush_type_linear:
        {
        } break;*/
        case md_brush_type_gc:
        {
            cairo_set_source_surface((cairo_t*)pGC->cairo.pCairoContext, (cairo_surface_t*)pConfig->gc.pGC->cairo.pCairoSurface, 0, 0);
            cairo_pattern_set_filter(cairo_get_source((cairo_t*)pGC->cairo.pCairoContext), md_to_cairo_filter(pGC->cairo.pState[iState].stretchFilter));
        } break;
        default:
        {
        } break;
    }
}

MD_PRIVATE void md_gc_set_source_from_brush__cairo(md_gc* pGC, md_brush* pBrush)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pBrush != NULL);

    iState = pGC->cairo.stateCount-1;

    if (pBrush != NULL) {
        cairo_set_source((cairo_t*)pGC->cairo.pCairoContext, (cairo_pattern_t*)pBrush->cairo.pCairoPattern);
        if (pBrush->config.type == md_brush_type_gc) {
            cairo_pattern_set_filter(cairo_get_source((cairo_t*)pGC->cairo.pCairoContext), md_to_cairo_filter(pGC->cairo.pState[iState].stretchFilter));
        }
    } else {
        cairo_set_source((cairo_t*)pGC->cairo.pCairoContext, NULL);
    }
}

MD_PRIVATE void md_gc_set_source_from_fill_brush__cairo(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;

    if (pGC->cairo.pState[iState].hasTransientFillBrush) {
        md_gc_set_source_from_brush_config__cairo(pGC, &pGC->cairo.pState[iState].transientFillBrush);
    } else {
        md_gc_set_source_from_brush__cairo(pGC, pGC->cairo.pState[iState].pFillBrush);
    }
}

MD_PRIVATE void md_gc_set_source_from_line_brush__cairo(md_gc* pGC)
{
    md_uint32 iState;

    MD_ASSERT(pGC != NULL);

    iState = pGC->cairo.stateCount-1;

    if (pGC->cairo.pState[iState].hasTransientLineBrush) {
        md_gc_set_source_from_brush_config__cairo(pGC, &pGC->cairo.pState[iState].transientLineBrush);
    } else {
        md_gc_set_source_from_brush__cairo(pGC, pGC->cairo.pState[iState].pLineBrush);
    }
}

void md_gc_fill__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);
    {
        md_gc_set_source_from_fill_brush__cairo(pGC);
        cairo_fill((cairo_t*)pGC->cairo.pCairoContext);
    }
    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_stroke__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);
    {
        md_gc_set_source_from_line_brush__cairo(pGC);
        cairo_stroke((cairo_t*)pGC->cairo.pCairoContext);
    }
    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_fill_and_stroke__cairo(md_gc* pGC)
{
    MD_ASSERT(pGC != NULL);

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);
    {
        md_gc_set_source_from_fill_brush__cairo(pGC);
        cairo_fill((cairo_t*)pGC->cairo.pCairoContext);

        md_gc_set_source_from_line_brush__cairo(pGC);
        cairo_stroke((cairo_t*)pGC->cairo.pCairoContext);
    }
    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);
}

void md_gc_draw_gc__cairo(md_gc* pGC, md_gc* pSrcGC, md_int32 srcX, md_int32 srcY)
{
    MD_ASSERT(pGC != NULL);
    MD_ASSERT(pSrcGC != NULL);

    /* This cannot be done using a transient GC. */
    if (pSrcGC->isTransient || pSrcGC->cairo.pCairoSurface == NULL) {
        return;
    }

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);
    {
        cairo_set_source_surface((cairo_t*)pGC->cairo.pCairoContext, (cairo_surface_t*)pSrcGC->cairo.pCairoSurface, srcX, srcY);
        cairo_pattern_set_filter(cairo_get_source((cairo_t*)pGC->cairo.pCairoContext), CAIRO_FILTER_NEAREST);
        cairo_paint((cairo_t*)pGC->cairo.pCairoContext);
    }
    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);
}

/* API */
MD_PRIVATE void md_free_pango_items__cairo(GList* pPangoItems)
{
    GList* pListItem;

    for (pListItem = g_list_first(pPangoItems); pListItem != NULL; pListItem = g_list_next(pListItem)) {
        pango_item_free((PangoItem*)pListItem->data);
    }

    g_list_free(pPangoItems);
}

md_result md_itemize_utf8__cairo(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState)
{
    GList* pPangoItems;
    md_uint32 itemCap;
    md_uint32 itemCount = 0;

    MD_ASSERT(pFont != NULL);
    MD_ASSERT(textLength > 0);
    MD_ASSERT(pTextUTF8 != NULL);
    MD_ASSERT(pTextUTF8[0] != '\0');
    MD_ASSERT(pItemCount != NULL);
    MD_ASSERT(pItemizeState != NULL);

    itemCap = *pItemCount;

    pPangoItems = pango_itemize((PangoContext*)pFont->pAPI->cairo.pPangoContext, pTextUTF8, 0, (gint)textLength, (PangoAttrList*)pFont->cairo.pPangoAttrList, NULL);
    if (pPangoItems == NULL) {
        return MD_ERROR;    /* An error occurred when itemizing. */
    }

    /*
    Unfortunately Pango does not split items the way minidraw defines them. We need certain common characters to have their own items to simplify text processing.
        * New lines
        * Tabs
    */
    if (pItems != NULL) {
        md_uint32 iItem = 0;
        GList* pListItem;
        for (pListItem = g_list_first(pPangoItems); pListItem != NULL; pListItem = g_list_next(pListItem)) {
            const char* pLineBegUTF8;
            const char* pLineEndUTF8;
            PangoItem* pPangoItem = (PangoItem*)pListItem->data;

            pLineBegUTF8 = pTextUTF8 + pPangoItem->offset;
            for (;;) {  /* For each line... */
                const char* pNextLineBegUTF8;
                pNextLineBegUTF8 = md_next_line_utf8(pLineBegUTF8, (size_t)((pTextUTF8 + pPangoItem->offset + pPangoItem->length) - pLineBegUTF8), &pLineEndUTF8);

                /* The space between pLineBegUTF8 and pLineEndUTF8 is the line content. */
                if ((pLineEndUTF8 - pLineBegUTF8) > 0) {
                    /* Split tabs. */
                    const char* pTabbedSegmentBegUTF8 = pLineBegUTF8;
                    const char* pTabbedSegmentEndUTF8;
                    for (;;) {
                        const char* pNextTabbedSegmentBegUTF8;
                        pNextTabbedSegmentBegUTF8 = md_next_tabbed_segment(pTabbedSegmentBegUTF8, (size_t)(pLineEndUTF8 - pTabbedSegmentBegUTF8), &pTabbedSegmentEndUTF8);

                        /* The main segment. */
                        if ((pTabbedSegmentEndUTF8 - pTabbedSegmentBegUTF8) > 0) {
                            itemCount += 1;
                            if (itemCount > itemCap) {
                                break;  /* Not enough room in the output buffer. */
                            }

                            pItems[iItem].offset = (size_t)(pTabbedSegmentBegUTF8 - pTextUTF8);
                            pItems[iItem].length = (size_t)(pTabbedSegmentEndUTF8 - pTabbedSegmentBegUTF8);
                            pItems[iItem].backend.cairo.pPangoItem = pPangoItem;
                            iItem += 1;
                        }

                        /* The \t characters. */
                        if ((pNextTabbedSegmentBegUTF8 - pTabbedSegmentEndUTF8) > 0) {
                            itemCount += 1;
                            if (itemCount > itemCap) {
                                break;  /* Not enough room in the output buffer. */
                            }

                            pItems[iItem].offset = (size_t)(pTabbedSegmentEndUTF8 - pTextUTF8);
                            pItems[iItem].length = (size_t)(pNextTabbedSegmentBegUTF8 - pTabbedSegmentEndUTF8);
                            pItems[iItem].backend.cairo.pPangoItem = pPangoItem;
                            iItem += 1;
                        }

                        /* Get out of the loop if we reached the end. */
                        if (pNextTabbedSegmentBegUTF8 == NULL) {
                            break;
                        }

                        pTabbedSegmentBegUTF8 = pNextTabbedSegmentBegUTF8;
                    }

                    if (itemCount > itemCap) {
                        break;  /* Not enough room in the output buffer. */
                    }
                }
                
                /* The space between pLineEndUTF8 and pNextLineBegUTF8 is the new line character. */
                if ((pNextLineBegUTF8 - pLineEndUTF8) > 0) {
                    itemCount += 1;
                    if (itemCount > itemCap) {
                        break;  /* Not enough room in the output buffer. */
                    }

                    pItems[iItem].offset = (size_t)(pLineEndUTF8 - pTextUTF8);
                    pItems[iItem].length = (size_t)(pNextLineBegUTF8 - pLineEndUTF8);
                    pItems[iItem].backend.cairo.pPangoItem = pPangoItem;
                    iItem += 1;
                }

                /* Get out of the loop if we reached the end. */
                if (pNextLineBegUTF8 == NULL) {
                    break;
                }

                pLineBegUTF8 = pNextLineBegUTF8;
            }
        }

        pItemizeState->backend.cairo.pPangoItems = pPangoItems;
    } else {
        /* Just counting. */
        GList* pListItem;
        for (pListItem = g_list_first(pPangoItems); pListItem != NULL; pListItem = g_list_next(pListItem)) {
            const char* pLineBegUTF8;
            const char* pLineEndUTF8;
            PangoItem* pPangoItem = (PangoItem*)pListItem->data;

            pLineBegUTF8 = pTextUTF8 + pPangoItem->offset;
            for (;;) {  /* For each line... */
                const char* pNextLineBegUTF8;
                pNextLineBegUTF8 = md_next_line_utf8(pLineBegUTF8, (size_t)((pTextUTF8 + pPangoItem->offset + pPangoItem->length) - pLineBegUTF8), &pLineEndUTF8);

                /* The space between pLineBegUTF8 and pLineEndUTF8 is the line content. */
                if ((pLineEndUTF8 - pLineBegUTF8) > 0) {
                    /* Split tabs. */
                    const char* pTabbedSegmentBegUTF8 = pLineBegUTF8;
                    const char* pTabbedSegmentEndUTF8;
                    for (;;) {
                        const char* pNextTabbedSegmentBegUTF8;
                        pNextTabbedSegmentBegUTF8 = md_next_tabbed_segment(pTabbedSegmentBegUTF8, (size_t)(pLineEndUTF8 - pTabbedSegmentBegUTF8), &pTabbedSegmentEndUTF8);

                        /* The main segment. */
                        if ((pTabbedSegmentEndUTF8 - pTabbedSegmentBegUTF8) > 0) {
                            itemCount += 1;
                        }

                        /* The \t characters. */
                        if ((pNextTabbedSegmentBegUTF8 - pTabbedSegmentEndUTF8) > 0) {
                            itemCount += 1;
                        }

                        /* Get out of the loop if we reached the end. */
                        if (pNextTabbedSegmentBegUTF8 == NULL) {
                            break;
                        }

                        pTabbedSegmentBegUTF8 = pNextTabbedSegmentBegUTF8;
                    }
                }
                
                /* The space between pLineEndUTF8 and pNextLineBegUTF8 is the new line character. */
                if ((pNextLineBegUTF8 - pLineEndUTF8) > 0) {
                    itemCount += 1;
                }

                /* Get out of the loop if we reached the end. */
                if (pNextLineBegUTF8 == NULL) {
                    break;
                }

                pLineBegUTF8 = pNextLineBegUTF8;
            }
        }

        if (pItemizeState != NULL) {
            pItemizeState->backend.cairo.pPangoItems = pPangoItems;
        } else {
            md_free_pango_items__cairo(pPangoItems);
            pPangoItems = NULL;
        }
    }

    if (itemCount > itemCap) {
        return MD_NO_SPACE;
    }

    *pItemCount = itemCount;

    return MD_SUCCESS;
}

void md_free_itemize_state__cairo(md_itemize_state* pItemizeState)
{
    MD_ASSERT(pItemizeState != NULL);

    md_free_pango_items__cairo((GList*)pItemizeState->backend.cairo.pPangoItems);
    pItemizeState->backend.cairo.pPangoItems = NULL;
}

md_result md_shape_utf8__cairo(md_font* pFont, md_item* pItem, const md_utf8* pTextUTF8, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics)
{
    PangoGlyphString* pPangoGlyphString;
    size_t glyphCap;
    size_t glyphCount = 0;
    md_text_metrics metrics;

    MD_ASSERT(pItem != NULL);
    MD_ASSERT(pFont != NULL);
    MD_ASSERT(pTextUTF8 != NULL);

    if (pGlyphCount != NULL) {
        glyphCap = *pGlyphCount;
    } else {
        glyphCap = 0;
    }
    
    MD_ZERO_OBJECT(&metrics);

    pPangoGlyphString = pango_glyph_string_new();
    if (pPangoGlyphString == NULL) {
        return MD_OUT_OF_MEMORY;
    }

    pango_shape(pTextUTF8, (gint)textLength, &((PangoItem*)pItem->backend.cairo.pPangoItem)->analysis, pPangoGlyphString);

    if (pGlyphs != NULL) {
        size_t iGlyph;

        glyphCount = (size_t)pPangoGlyphString->num_glyphs;
        if (glyphCount > glyphCap) {
            pango_glyph_string_free(pPangoGlyphString);
            return MD_NO_SPACE; /* No room to store each glyph. */
        }

        for (iGlyph = 0; iGlyph < pPangoGlyphString->num_glyphs; ++iGlyph) {
            pGlyphs[iGlyph].index                 = pPangoGlyphString->glyphs[iGlyph].glyph;
            pGlyphs[iGlyph].advance               = pPangoGlyphString->glyphs[iGlyph].geometry.width / PANGO_SCALE;
            pGlyphs[iGlyph].backend.cairo.width   = pPangoGlyphString->glyphs[iGlyph].geometry.width;
            pGlyphs[iGlyph].backend.cairo.offsetX = pPangoGlyphString->glyphs[iGlyph].geometry.x_offset;
            pGlyphs[iGlyph].backend.cairo.offsetY = pPangoGlyphString->glyphs[iGlyph].geometry.y_offset;
            metrics.sizeX += pGlyphs[iGlyph].advance;
        }
        metrics.sizeY = pFont->metrics.ascent - pFont->metrics.descent;
    } else {
        /* The output glyph array is NULL, but we may still want to calculate metrics. */
        if (pRunMetrics != NULL) {
            size_t iGlyph;
            glyphCount = (size_t)pPangoGlyphString->num_glyphs;

            for (iGlyph = 0; iGlyph < pPangoGlyphString->num_glyphs; ++iGlyph) {
                metrics.sizeX += pPangoGlyphString->glyphs[iGlyph].geometry.width / PANGO_SCALE;
            }
            metrics.sizeY = pFont->metrics.ascent - pFont->metrics.descent;
        }
    }

    if (pClusters != NULL) {
        size_t iTextByte;
        for (iTextByte = 0; iTextByte < textLength; ++iTextByte) {
            pClusters[iTextByte] = (size_t)pPangoGlyphString->log_clusters[iTextByte];
        }
    }

    pango_glyph_string_free(pPangoGlyphString);

    if (pGlyphCount != NULL) {
        *pGlyphCount = glyphCount;
    }

    if (pRunMetrics != NULL) {
        *pRunMetrics = metrics;
    }

    return MD_SUCCESS;
}

void md_gc_draw_glyphs__cairo(md_gc* pGC, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32 x, md_int32 y)
{
    md_uint32 iState;
    size_t iGlyph;
    md_color fgColor;
    md_color bgColor;
    md_uint32 textWidth = 0;
    PangoGlyphString glyphString;
    PangoGlyphInfo  pGlyphInfoStack[4096];
    PangoGlyphInfo* pGlyphInfoHeap = NULL;
    PangoGlyphInfo* pGlyphInfo = NULL;
    PangoFont* pPangoFont;
    PangoFontMetrics* pPangoFontMetrics;
    md_int32 ascent;
    md_int32 descent;

    MD_ASSERT(pGC     != NULL);
    MD_ASSERT(pItem   != NULL);
    MD_ASSERT(pGlyphs != NULL);

    pPangoFont = ((PangoItem*)pItem->backend.cairo.pPangoItem)->analysis.font;

    /* May want to optimize this... */
    pPangoFontMetrics = pango_font_get_metrics(pPangoFont, NULL);
    ascent  =  pango_font_metrics_get_ascent(pPangoFontMetrics)  / PANGO_SCALE;
    descent = -pango_font_metrics_get_descent(pPangoFontMetrics) / PANGO_SCALE;
    pango_font_metrics_unref(pPangoFontMetrics);

    iState  = pGC->cairo.stateCount-1;
    fgColor = pGC->cairo.pState[iState].textFGColor;
    bgColor = pGC->cairo.pState[iState].textBGColor;

    /* Unfortunately Pango's API doesn't map too well with minidraw's. We need to build our own glyph string. */
    if (glyphCount <= MD_COUNTOF(pGlyphInfoStack)) {
        /* Enough room on the stack. */
        pGlyphInfo = pGlyphInfoStack;
    } else {
        /* Try a heap allocation. */
        pGlyphInfoHeap = (PangoGlyphInfo*)MD_MALLOC(sizeof(*pGlyphInfoHeap) * glyphCount);
        if (pGlyphInfoHeap == NULL) {
            return; /* Out of memory. */
        }

        pGlyphInfo = pGlyphInfoHeap;
    }

    glyphString.num_glyphs = (gint)glyphCount;
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        pGlyphInfo[iGlyph].glyph                 = pGlyphs[iGlyph].index;
        pGlyphInfo[iGlyph].geometry.width        = pGlyphs[iGlyph].backend.cairo.width;
        pGlyphInfo[iGlyph].geometry.x_offset     = pGlyphs[iGlyph].backend.cairo.offsetX;
        pGlyphInfo[iGlyph].geometry.y_offset     = pGlyphs[iGlyph].backend.cairo.offsetY;
        pGlyphInfo[iGlyph].attr.is_cluster_start = pGlyphs[iGlyph].backend.cairo.isClusterStart;

        textWidth += pGlyphs[iGlyph].advance;
    }
    glyphString.glyphs = pGlyphInfo;
    glyphString.log_clusters = NULL;

    /* Background. */
    cairo_set_source_rgba((cairo_t*)pGC->cairo.pCairoContext, bgColor.r/255.0, bgColor.g/255.0, bgColor.b/255.0, bgColor.a/255.0);
    cairo_rectangle((cairo_t*)pGC->cairo.pCairoContext, x, y, textWidth, (ascent - descent));
    cairo_fill((cairo_t*)pGC->cairo.pCairoContext);

    /* Text. */
    cairo_move_to((cairo_t*)pGC->cairo.pCairoContext, x, y + ascent);
    cairo_set_source_rgba((cairo_t*)pGC->cairo.pCairoContext, fgColor.r/255.0, fgColor.g/255.0, fgColor.b/255.0, fgColor.a/255.0);
    pango_cairo_show_glyph_string((cairo_t*)pGC->cairo.pCairoContext, pPangoFont, &glyphString);

    MD_FREE(pGlyphInfoHeap);
}

void md_gc_clear__cairo(md_gc* pGC, md_color color)
{
    MD_ASSERT(pGC != NULL);

    cairo_save((cairo_t*)pGC->cairo.pCairoContext);
    {
        cairo_set_operator((cairo_t*)pGC->cairo.pCairoContext, CAIRO_OPERATOR_OVER);
        cairo_set_source_rgba((cairo_t*)pGC->cairo.pCairoContext, color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
        cairo_paint((cairo_t*)pGC->cairo.pCairoContext);
    }
    cairo_restore((cairo_t*)pGC->cairo.pCairoContext);
}


void md_uninit__cairo(md_api* pAPI)
{
    MD_ASSERT(pAPI != NULL);

    if (pAPI->cairo.ownsPangoContext) {
        g_object_unref((PangoContext*)pAPI->cairo.pPangoContext);
    }
}

md_result md_init__cairo(const md_api_config* pConfig, md_api* pAPI)
{
    MD_ASSERT(pConfig != NULL);
    MD_ASSERT(pAPI != NULL);

    pAPI->procs.uninit                     = md_uninit__cairo;
    pAPI->procs.itemizeUTF8                = md_itemize_utf8__cairo;
    pAPI->procs.itemizeUTF16               = NULL;    /* No native support for UTF-16 with Pango. */
    pAPI->procs.itemizeUTF32               = NULL;    /* No native support for UTF-32 with Pango. */
    pAPI->procs.freeItemizeState           = md_free_itemize_state__cairo;
    pAPI->procs.shapeUTF8                  = md_shape_utf8__cairo;
    pAPI->procs.shapeUTF16                 = NULL;    /* No native support for UTF-16 with Pango. */
    pAPI->procs.shapeUTF32                 = NULL;    /* No native support for UTF-32 with Pango. */
    pAPI->procs.fontInit                   = md_font_init__cairo;
    pAPI->procs.fontUninit                 = md_font_uninit__cairo;
    pAPI->procs.fontGetGlyphMetrics        = md_font_get_glyph_metrics__cairo;
    pAPI->procs.fontGetGlyphMetricsByIndex = md_font_get_glyph_metrics_by_index__cairo;
    pAPI->procs.brushInit                  = md_brush_init__cairo;
    pAPI->procs.brushUninit                = md_brush_uninit__cairo;
    pAPI->procs.brushSetOrigin             = md_brush_set_origin__cairo;
    pAPI->procs.gcInit                     = md_gc_init__cairo;
    pAPI->procs.gcUninit                   = md_gc_uninit__cairo;
    pAPI->procs.gcGetImageData             = md_gc_get_image_data__cairo;
    pAPI->procs.gcGetSize                  = md_gc_get_size__cairo;
    pAPI->procs.gcSave                     = md_gc_save__cairo;
    pAPI->procs.gcRestore                  = md_gc_restore__cairo;
    pAPI->procs.gcSetMatrix                = md_gc_set_matrix__cairo;
    pAPI->procs.gcGetMatrix                = md_gc_get_matrix__cairo;
    pAPI->procs.gcSetMatrixIdentity        = md_gc_set_matrix_identity__cairo;
    pAPI->procs.gcTransform                = md_gc_transform__cairo;
    pAPI->procs.gcTranslate                = md_gc_translate__cairo;
    pAPI->procs.gcRotate                   = md_gc_rotate__cairo;
    pAPI->procs.gcScale                    = md_gc_scale__cairo;
    pAPI->procs.gcSetLineWidth             = md_gc_set_line_width__cairo;
    pAPI->procs.gcSetLineCap               = md_gc_set_line_cap__cairo;
    pAPI->procs.gcSetLineJoin              = md_gc_set_line_join__cairo;
    pAPI->procs.gcSetMiterLimit            = md_gc_set_miter_limit__cairo;
    pAPI->procs.gcSetLineDash              = md_gc_set_line_dash__cairo;
    pAPI->procs.gcSetLineBrush             = md_gc_set_line_brush__cairo;
    pAPI->procs.gcSetLineBrushSolid        = md_gc_set_line_brush_solid__cairo;
    pAPI->procs.gcSetLineBrushGC           = md_gc_set_line_brush_gc__cairo;
    pAPI->procs.gcSetFillBrush             = md_gc_set_fill_brush__cairo;
    pAPI->procs.gcSetFillBrushSolid        = md_gc_set_fill_brush_solid__cairo;
    pAPI->procs.gcSetFillBrushGC           = md_gc_set_fill_brush_gc__cairo;
    pAPI->procs.gcSetTextFGColor           = md_gc_set_text_fg_color__cairo;
    pAPI->procs.gcSetTextBGColor           = md_gc_set_text_bg_color__cairo;
    pAPI->procs.gcGetTextFGColor           = md_gc_get_text_fg_color__cairo;
    pAPI->procs.gcGetTextBGColor           = md_gc_get_text_bg_color__cairo;
    pAPI->procs.gcSetBlendOp               = md_gc_set_blend_op__cairo;
    pAPI->procs.gcSetAntialiasMode         = md_gc_set_antialias_mode__cairo;
    pAPI->procs.gcSetFillMode              = md_gc_set_fill_mode__cairo;
    pAPI->procs.gcSetStretchFilter         = md_gc_set_stretch_filter__cairo;
    pAPI->procs.gcMoveTo                   = md_gc_move_to__cairo;
    pAPI->procs.gcLineTo                   = md_gc_line_to__cairo;
    pAPI->procs.gcRectangle                = md_gc_rectangle__cairo;
    pAPI->procs.gcArc                      = md_gc_arc__cairo;
    pAPI->procs.gcCurveTo                  = md_gc_curve_to__cairo;
    pAPI->procs.gcClosePath                = md_gc_close_path__cairo;
    pAPI->procs.gcClip                     = md_gc_clip__cairo;
    pAPI->procs.gcResetClip                = md_gc_reset_clip__cairo;
    pAPI->procs.gcIsPointInsideClip        = md_gc_is_point_inside_clip__cairo;
    pAPI->procs.gcFill                     = md_gc_fill__cairo;
    pAPI->procs.gcStroke                   = md_gc_stroke__cairo;
    pAPI->procs.gcFillAndStroke            = md_gc_fill_and_stroke__cairo;
    pAPI->procs.gcDrawGC                   = md_gc_draw_gc__cairo;
    pAPI->procs.gcDrawGlyphs               = md_gc_draw_glyphs__cairo;
    pAPI->procs.gcClear                    = md_gc_clear__cairo;

    /* We need to a PangoContext before we'll be able to create fonts. */
    if (pConfig->cairo.pPangoContext != NULL) {
        pAPI->cairo.ownsPangoContext = MD_FALSE;
        pAPI->cairo.pPangoContext = pConfig->cairo.pPangoContext;
    } else {
        pAPI->cairo.ownsPangoContext = MD_TRUE;
        pAPI->cairo.pPangoContext = pango_font_map_create_context(pango_cairo_font_map_get_default());
        if (pAPI->cairo.pPangoContext == NULL) {
            return MD_ERROR;    /* Should never happen. */
        }
    }

    return MD_SUCCESS;
}

#endif

/**************************************************************************************************************************************************************

 Xft

 **************************************************************************************************************************************************************/
#if defined(MD_HAS_XFT)
#include <X11/Xft/Xft.h>
#include <harfbuzz/hb-ft.h>

/* Font */

/* Graphics */

/* API */

#endif



/**************************************************************************************************************************************************************

 Custom

 **************************************************************************************************************************************************************/
md_result md_init__custom(const md_api_config* pConfig, md_api* pAPI)
{
    pAPI->procs = pConfig->custom.procs;
    return MD_SUCCESS;
}



/**************************************************************************************************************************************************************

API

**************************************************************************************************************************************************************/
md_result md_init(const md_api_config* pConfig, md_api* pAPI)
{
    md_result result = MD_ERROR;

    if (pAPI == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pAPI);

    if (pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    pAPI->backend = pConfig->backend;
    pAPI->pUserData = pConfig->pUserData;

    switch (pConfig->backend)
    {
#if defined(MD_HAS_GDI)
        case md_backend_gdi:
        {
            result = md_init__gdi(pConfig, pAPI);
        } break;
#endif
#if defined(MD_HAS_CAIRO)
        case md_backend_cairo:
        {
            result = md_init__cairo(pConfig, pAPI);
        } break;
#endif

        case md_backend_custom:
        {
            result = md_init__custom(pConfig, pAPI);
        } break;

        default:
        {
            result = MD_INVALID_ARGS;
        };
    }

    if (result != MD_SUCCESS) {
        return result;
    }

    return MD_SUCCESS;
}

void md_uninit(md_api* pAPI)
{
    if (pAPI == NULL) {
        return;
    }

    pAPI->procs.uninit(pAPI);
}

md_result md_itemize_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_item* pItems, md_uint32* pItemCount, md_itemize_state* pItemizeState)
{
    md_result result;
    md_uint32 itemCount;

    if (pItemCount == NULL) {
        return MD_INVALID_ARGS;
    }

    itemCount = *pItemCount;

    if (pFont == NULL || pTextUTF8 == NULL || textLength == 0) {
        return MD_INVALID_ARGS;
    }

    if (textLength == (size_t)-1) {
        textLength = strlen(pTextUTF8);
    }

    /*
    We could do some generic itemization here such as line breaks and tabs, however it makes things awkward with memory management. I'm therefore
    leaving this to each of the backends.
    */

    /* If the backend directly supports UTF-8, just pass it straight through. Otherwise we need to convert. */
    if (pFont->pAPI->procs.itemizeUTF8) {
        /* UTF-8 natively supported. */
        result = pFont->pAPI->procs.itemizeUTF8(pFont, pTextUTF8, textLength, pItems, &itemCount, pItemizeState);
    } else {
        /* UTF-8 not natively supported. Need to convert the string to another format. */
        if (pFont->pAPI->procs.itemizeUTF16) {
            /*
            Convert the input string to UTF-16, itemize, then convert offsets back to UTF-8 equivalents. To make things more efficient for small
            strings we can try converting to a stack-allocated buffer first. If this is too small we will fall back to the heap which will be
            slightly less efficient.
            */
            size_t utf16Len;
            md_utf16  pUTF16Stack[4096];
            md_utf16* pUTF16Heap = NULL;
            md_utf16* pUTF16 = NULL;    /* Will be set to either pUTF16Stack or pUTF16Heap. */

            result = md_utf8_to_utf16ne(pUTF16Stack, MD_COUNTOF(pUTF16Stack), &utf16Len, pTextUTF8, textLength, NULL, MD_ERROR_ON_INVALID_CODE_POINT);
            if (result == MD_SUCCESS) {
                pUTF16 = &pUTF16Stack[0];
            } else {
                /* Fall back to the heap. */
                result = md_utf8_to_utf16ne_length(&utf16Len, pTextUTF8, textLength, 0);
                if (result == MD_SUCCESS) {
                    pUTF16Heap = (md_utf16*)MD_MALLOC(sizeof(*pUTF16Heap) * (utf16Len+1));
                    if (pUTF16Heap != NULL) {
                        result = md_utf8_to_utf16ne(pUTF16Heap, utf16Len+1, &utf16Len, pTextUTF8, textLength, NULL, MD_ERROR_ON_INVALID_CODE_POINT);
                        if (result == MD_SUCCESS) {
                            pUTF16 = pUTF16Heap;
                        }
                    } else {
                        result = MD_OUT_OF_MEMORY;
                    }
                }

                if (result != MD_SUCCESS) {
                    return result;  /* An error occurred when converting to UTF-16. */
                }
            }

            /* We have the UTF-16 string, so now we need to itemize these. */
            result = pFont->pAPI->procs.itemizeUTF16(pFont, pUTF16, utf16Len, pItems, &itemCount, pItemizeState);
            if (result == MD_SUCCESS) {
                /* We have the items, so now we need to convert the offsets and lengths to the UTF-8 equivalents. */
                if (pItems != NULL) {
                    size_t runningOffset = 0;
                    md_uint32 iItem;
                    for (iItem = 0; iItem < itemCount; ++iItem) {
                        size_t itemLengthUTF8;
                        result = md_utf16ne_to_utf8_length(&itemLengthUTF8, pUTF16 + pItems[iItem].offset, pItems[iItem].length, 0);
                        if (result != MD_SUCCESS) {
                            break;  /* Conversion error. */
                        }

                        pItems[iItem].offset = runningOffset;
                        pItems[iItem].length = itemLengthUTF8;
                        runningOffset += itemLengthUTF8;
                    }
                }
            }
            
            /* Done. */
            MD_FREE(pUTF16Heap);
        } else if (pFont->pAPI->procs.itemizeUTF32) {
            /* TODO: Convert the input string to UTF-32, itemize, then convert offsets back to UTF-8 equivalents. */
            return MD_INVALID_OPERATION;
        } else {
            return MD_INVALID_OPERATION;    /* Itemization not supported by the backend. */
        }
    }

    *pItemCount = itemCount;

    /* Make sure the API object is always set in the itemize state. */
    if (pItemizeState) {
        pItemizeState->pAPI = pFont->pAPI;
    }

    return result;
}

void md_free_itemize_state(md_itemize_state* pItemizeState)
{
    if (pItemizeState == NULL) {
        return;
    }

    if (pItemizeState->pAPI->procs.freeItemizeState) {
        pItemizeState->pAPI->procs.freeItemizeState(pItemizeState);
    }
}

md_result md_shape_utf8(md_font* pFont, md_item* pItem, const md_utf8* pTextUTF8, size_t textLength, md_glyph* pGlyphs, size_t* pGlyphCount, size_t* pClusters, md_text_metrics* pRunMetrics)
{
    md_result result;

    if (pFont == NULL || pItem == NULL || pTextUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    /* If we have a glyph array, we must have a glyph count. */
    if (pGlyphs != NULL) {
        if (pGlyphCount == NULL) {
            return MD_INVALID_ARGS;
        }
    }

    /* If the backend directly supports UTF-8, just pass it straight through. Otherwise we need to convert. */
    if (pFont->pAPI->procs.shapeUTF8) {
        result = pFont->pAPI->procs.shapeUTF8(pFont, pItem, pTextUTF8, textLength, pGlyphs, pGlyphCount, pClusters, pRunMetrics);
    } else {
        if (pFont->pAPI->procs.shapeUTF16) {
            /* Convert to UTF-16. We can try using the stack first, and then fall back to a heap-allocation if necessary. */
            size_t    utf16Len;
            void*     pHeap = NULL;
            md_utf16  pUTF16Stack[4096];
            md_utf16* pUTF16Heap = NULL;
            md_utf16* pUTF16 = NULL;    /* Will be set to either pUTF16Stack or pUTF16Heap. */
            size_t    pUTF16ClustersStack[4096];
            size_t*   pUTF16ClustersHeap = NULL;
            size_t*   pUTF16Clusters = NULL;

            result = md_utf8_to_utf16ne(pUTF16Stack, MD_COUNTOF(pUTF16Stack), &utf16Len, pTextUTF8, textLength, NULL, MD_ERROR_ON_INVALID_CODE_POINT);
            if (result == MD_SUCCESS) {
                pUTF16         = &pUTF16Stack[0];
                pUTF16Clusters = &pUTF16ClustersStack[0];
            } else {
                /* Fall back to the heap. */
                result = md_utf8_to_utf16ne_length(&utf16Len, pTextUTF8, textLength, 0);
                if (result == MD_SUCCESS) {
                    pHeap = MD_MALLOC((sizeof(*pUTF16Heap)+sizeof(*pUTF16ClustersHeap)) * (utf16Len+1));
                    if (pHeap != NULL) {
                        pUTF16Heap         = (md_utf16*)pHeap;
                        pUTF16ClustersHeap = (size_t*)MD_OFFSET_PTR(pHeap, sizeof(*pUTF16Heap) * (utf16Len+1));

                        result = md_utf8_to_utf16ne(pUTF16Heap, utf16Len+1, &utf16Len, pTextUTF8, textLength, NULL, MD_ERROR_ON_INVALID_CODE_POINT);
                        if (result == MD_SUCCESS) {
                            pUTF16         = pUTF16Heap;
                            pUTF16Clusters = pUTF16ClustersHeap;
                        } else {
                            MD_FREE(pHeap);
                            pHeap = NULL;   /* Safety. */
                        }
                    } else {
                        result = MD_OUT_OF_MEMORY;
                    }
                }

                if (result != MD_SUCCESS) {
                    return result;  /* An error occurred when converting to UTF-16. */
                }
            }

            MD_ASSERT(utf16Len <= textLength);

            /* We have the UTF-16 string, so now we perform shaping. */
            result = pFont->pAPI->procs.shapeUTF16(pFont, pItem, pUTF16, utf16Len, pGlyphs, pGlyphCount, pUTF16Clusters, pRunMetrics);
            if (result == MD_SUCCESS) {
                /* The values in pClusters need to be expanded so that they map to the original UTF-8 string. */
                if (pClusters != NULL) {
                    md_utf32 utf32;
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
                            utf32 = md_utf16_pair_to_utf32_cp(pUTF16 + iUTF16);
                            utf16Count = 2;
                        }

                        utf8Count = md_utf32_cp_to_utf8_length(utf32);
                        for (iClusterUTF8 = 0; iClusterUTF8 < utf8Count; ++iClusterUTF8) {
                            pClusters[iUTF8] = pUTF16Clusters[iUTF16];
                            iUTF8 += 1;
                        }

                        iUTF16 += utf16Count;
                    }
                }
            }

            MD_FREE(pHeap);
        } else if (pFont->pAPI->procs.shapeUTF32) {
            /* TODO: Implement me. */
            return MD_INVALID_OPERATION;
        } else {
            return MD_INVALID_OPERATION;    /* Shaping not supported by the backend. */
        }
    }

    return result;
}

md_result md_measure_x(md_api* pAPI, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32* pX)
{
    size_t iGlyph;
    md_int32 runningX;

    if (pAPI == NULL || pItem == NULL || pGlyphs == NULL) {
        return MD_SUCCESS;
    }

    runningX = 0;
    for (iGlyph = 0; iGlyph < glyphCount; ++iGlyph) {
        runningX += pGlyphs[iGlyph].advance;
    }

    if (pX != NULL) {
        *pX = runningX;
    }

    return MD_SUCCESS;
}

md_result md_x_to_index(md_api* pAPI, const md_item* pItem, md_int32 x, size_t textLength, const size_t* pClusters, const md_glyph* pGlyphs, size_t glyphCount, size_t* pIndex, md_int32* pOffsetToEdge)
{
    /* Naive implementation for now while I figure things out. */

    size_t iGlyph;
    size_t iCluster;
    md_int32 runningX;

    if (pIndex != NULL) {
        *pIndex = (size_t)0;
    }
    if (pOffsetToEdge != NULL) {
        *pOffsetToEdge = 0;
    }

    /* If x is negative we need to clamp it to edge of the run. */
    if (x < 0) {
        return MD_SUCCESS;
    }

    if (pAPI == NULL || pItem == NULL || pClusters == NULL || pGlyphs == NULL) {
        return MD_INVALID_ARGS;
    }

    runningX = 0;
    for (iGlyph = 0; iGlyph < glyphCount; iGlyph += 1) {
        md_int32 nextRunningX = runningX + pGlyphs[iGlyph].advance;
        if (x >= runningX && x < nextRunningX) {
            /* It's somewhere on this glyph. */
            md_bool32 found = MD_FALSE;
            for (iCluster = 0; iCluster < textLength; iCluster += 1) {
                if (pClusters[iCluster] == iGlyph) {
                    if (pIndex != NULL) {
                        *pIndex = iCluster;
                    }
                        
                    found = MD_TRUE;
                }
            }

            if (!found) {
                return MD_INVALID_ARGS; /* Couldn't find the cluster for this glyph. */
            }

            if (pOffsetToEdge != NULL) {
                *pOffsetToEdge = runningX - x;
            }

            return MD_SUCCESS;
        }

        runningX = nextRunningX;
    }

    /* Geting here means the point is beyond the string. We just place it at the end. */
    if (pIndex != NULL) {
        *pIndex = textLength;
    }
    if (pOffsetToEdge != NULL) {
        *pOffsetToEdge = 0;
    }

    return MD_SUCCESS;
}

md_result md_index_to_x(md_api* pAPI, const md_item* pItem, size_t index, size_t textLength, const size_t* pClusters, const md_glyph* pGlyphs, size_t glyphCount, md_int32* pX)
{
    /* Naive implementation for now while I figure things out. */

    size_t iGlyph;
    md_int32 runningX;

    if (pX != NULL) {
        *pX = 0;
    }

    if (pAPI == NULL || pClusters == NULL || pGlyphs == NULL) {
        return MD_INVALID_ARGS;
    }

    if (glyphCount == 0) {
        return MD_SUCCESS;
    }

    /* Normalize the index if it's out of range. */
    if (index > textLength) {
        index = textLength;
    }

    /* Special case when the code unit is beyond the length of the text. In this case we set pX to end of the run. */
    runningX = 0;
    if (index < textLength) {
        size_t iTargetGlyph = pClusters[index];
        if (iTargetGlyph > 0) {
            for (iGlyph = 0; iGlyph < iTargetGlyph; iGlyph += 1) {
                runningX += pGlyphs[iGlyph].advance;
            }
        }

        if (pX != NULL) {
            *pX = runningX;
        }
    } else {
        /* Go to the end. */
        MD_ASSERT(index == textLength);
        return md_measure_x(pAPI, pItem, pGlyphs, glyphCount, pX);
    }

    return MD_SUCCESS;
}


/**************************************************************************************************************************************************************

Fonts

**************************************************************************************************************************************************************/
md_result md_font_init(md_api* pAPI, const md_font_config* pConfig, md_font* pFont)
{
    md_result result;

    if (pFont == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pFont);

    if (pAPI == NULL || pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    pFont->pAPI = pAPI;
    pFont->pUserData = pConfig->pUserData;

    result = pAPI->procs.fontInit(pAPI, pConfig, pFont);
    if (result != MD_SUCCESS) {
        return result;
    }

    return MD_SUCCESS;
}

void md_font_uninit(md_font* pFont)
{
    if (pFont == NULL) {
        return;
    }

    if (pFont->pAPI->procs.fontUninit) {
        pFont->pAPI->procs.fontUninit(pFont);
    }
    
    MD_ZERO_OBJECT(pFont);  /* Safety. */
}

md_result md_font_get_glyph_metrics(md_font* pFont, const md_glyph* pGlyphs, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    if (pGlyphMetrics == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_MEMORY(pGlyphMetrics, sizeof(*pGlyphMetrics) * glyphCount);

    if (pFont == NULL || pGlyphs == NULL) {
        return MD_INVALID_ARGS;
    }

    if (pFont->pAPI->procs.fontGetGlyphMetrics == NULL) {
        return MD_INVALID_OPERATION;
    }

    return pFont->pAPI->procs.fontGetGlyphMetrics(pFont, pGlyphs, glyphCount, pGlyphMetrics);
}

md_result md_font_get_glyph_metrics_by_index(md_font* pFont, const md_uint32* pGlyphIndices, size_t glyphCount, md_glyph_metrics* pGlyphMetrics)
{
    if (pGlyphMetrics == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_MEMORY(pGlyphMetrics, sizeof(*pGlyphMetrics) * glyphCount);

    if (pFont == NULL || pGlyphIndices == NULL) {
        return MD_INVALID_ARGS;
    }

    if (pFont->pAPI->procs.fontGetGlyphMetricsByIndex == NULL) {
        return MD_INVALID_OPERATION;
    }

    return pFont->pAPI->procs.fontGetGlyphMetricsByIndex(pFont, pGlyphIndices, glyphCount, pGlyphMetrics);
}

md_result md_font_get_text_metrics_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_text_metrics* pTextMetrics)
{
    md_int32 lineHeight;
    md_int32 lineWidth;

    if (pTextMetrics == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pTextMetrics);

    if (pFont == NULL || pTextUTF8 == NULL) {
        return MD_INVALID_ARGS;
    }

    lineHeight = (pFont->metrics.ascent - pFont->metrics.descent);

    if (textLength == 0) {
        pTextMetrics->sizeY = lineHeight;
        return MD_SUCCESS;  /* Nothing to do. */
    }

    /* TODO: Add support for backend-optimized implementations. */
#if 0
    if (pFont->pAPI->procs.fontGetTextMetricsUTF8) {
        return pFont->pAPI->procs.fontGetTextMetricsUTF8(pFont, pTextUTF8, textLength, pTextMetrics);
    } else
#endif
    {
        /* Generic implementation. */
        md_result result;
        md_int32  sizeX = 0;
        md_int32  sizeY = lineHeight;
        md_itemize_state itemizeState;
        md_item   pItemsStack[1024];
        md_item*  pItemsHeap = NULL;
        md_item*  pItems = NULL;
        md_uint32 itemCount;
        md_uint32 iItem;

        /* The first step is to itemize. */
        itemCount = MD_COUNTOF(pItemsStack);
        result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsStack, &itemCount, &itemizeState);
        if (result == MD_SUCCESS) {
            pItems = &pItemsStack[0];
        } else if (result == MD_NO_SPACE) {
            /* Not enough room on the stack. Try again against the heap. */
            pItemsHeap = (md_item*)MD_MALLOC(sizeof(*pItemsHeap) * itemCount);
            if (pItemsHeap == NULL) {
                return MD_OUT_OF_MEMORY;
            }

            result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsHeap, &itemCount, &itemizeState);
            if (result != MD_SUCCESS) {
                return result;
            }

            pItems = pItemsHeap;
        } else {
            return result;
        }

        /* We have the items so now we can measure them. */
        lineWidth = 0;
        for (iItem = 0; iItem < itemCount; ++iItem) {
            /* No need to do shaping for new-lines. */
            if (md_is_newline_utf8(pTextUTF8 + pItems[iItem].offset, pItems[iItem].length)) {
                sizeY += lineHeight;
                if (sizeX < lineWidth) {
                    sizeX = lineWidth;
                }
                lineWidth = 0;
            } else {
                md_text_metrics itemMetrics;
                result = md_shape_utf8(pFont, &pItems[iItem], pTextUTF8 + pItems[iItem].offset, pItems[iItem].length, NULL, NULL, NULL, &itemMetrics);
                if (result != MD_SUCCESS) {
                    MD_FREE(pItemsHeap);
                    pItemsHeap = NULL;
                    md_free_itemize_state(&itemizeState);
                    return result;
                }

                lineWidth += itemMetrics.sizeX;
            }
        }

        /* Don't forget to check the last line. */
        if (sizeX < lineWidth) {
            sizeX = lineWidth;
        }

        pTextMetrics->sizeX = sizeX;
        pTextMetrics->sizeY = sizeY;

        md_free_itemize_state(&itemizeState);
        return MD_SUCCESS;
    }
}


/**************************************************************************************************************************************************************

Graphics

**************************************************************************************************************************************************************/
md_result md_brush_init(md_api* pAPI, const md_brush_config* pConfig, md_brush* pBrush)
{
    md_result result;

    if (pBrush == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pBrush);

    if (pAPI == NULL || pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    pBrush->pAPI = pAPI;
    pBrush->pUserData = pConfig->pUserData;
    pBrush->config = *pConfig;

    if (pAPI->procs.brushInit) {
        result = pAPI->procs.brushInit(pAPI, pConfig, pBrush);
        if (result != MD_SUCCESS) {
            return result;
        }
    }

    return MD_SUCCESS;
}

void md_brush_uninit(md_brush* pBrush)
{
    if (pBrush == NULL) {
        return;
    }

    MD_ASSERT(pBrush->pAPI != NULL);

    if (pBrush->pAPI->procs.brushUninit) {
        pBrush->pAPI->procs.brushUninit(pBrush);
    }

    MD_ZERO_OBJECT(pBrush);  /* Safety. */
}

void md_brush_set_origin(md_brush* pBrush, md_int32 x, md_int32 y)
{
    if (pBrush == NULL) {
        return;
    }

    MD_ASSERT(pBrush->pAPI != NULL);

    if (pBrush->pAPI->procs.brushSetOrigin) {
        pBrush->pAPI->procs.brushSetOrigin(pBrush, x, y);
    }
}

md_result md_gc_init(md_api* pAPI, const md_gc_config* pConfig, md_gc* pGC)
{
    md_result result;

    if (pGC == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pGC);

    if (pAPI == NULL || pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    pGC->pAPI = pAPI;
    pGC->pUserData = pConfig->pUserData;

    if (pAPI->procs.gcInit) {
        result = pAPI->procs.gcInit(pAPI, pConfig, pGC);
        if (result != MD_SUCCESS) {
            return result;
        }
    }

    return MD_SUCCESS;
}

void md_gc_uninit(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcUninit) {
        pGC->pAPI->procs.gcUninit(pGC);
    }
}

md_result md_gc_get_image_data_size_in_bytes(md_gc* pGC, md_format outputFormat, size_t* pSizeInBytes)
{
    md_result result;
    md_uint32 sizeX;
    md_uint32 sizeY;

    if (pSizeInBytes != NULL) {
        *pSizeInBytes = 0;  /* Safety. */
    }

    if (pGC == NULL || pSizeInBytes == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    result = md_gc_get_size(pGC, &sizeX, &sizeY);
    if (result != MD_SUCCESS) {
        return result;
    }

    *pSizeInBytes = sizeX * sizeY * md_get_bytes_per_pixel(outputFormat);

    return MD_SUCCESS;
}

md_result md_gc_get_image_data(md_gc* pGC, md_format outputFormat, void* pImageData)
{
    md_result result;

    if (pGC == NULL || pImageData == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcGetImageData) {
        result = pGC->pAPI->procs.gcGetImageData(pGC, outputFormat, pImageData);
        if (result != MD_SUCCESS) {
            return result;
        }

        return result;
    } else {
        return MD_INVALID_OPERATION;
    }
}

md_result md_gc_get_size(md_gc* pGC, md_uint32* pSizeX, md_uint32* pSizeY)
{
    if (pSizeX != NULL) {
        *pSizeX = 0;
    }
    if (pSizeY != NULL) {
        *pSizeY = 0;
    }

    if (pGC == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcGetSize) {
        md_uint32 sizeX;
        md_uint32 sizeY;
        md_result result = pGC->pAPI->procs.gcGetSize(pGC, &sizeX, &sizeY);
        if (result != MD_SUCCESS) {
            return result;
        }

        if (pSizeX != NULL) {
            *pSizeX = sizeX;
        }
        if (pSizeY != NULL) {
            *pSizeY = sizeY;
        }
    }

    return MD_SUCCESS;
}

md_result md_gc_save(md_gc* pGC)
{
    if (pGC == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSave) {
        return pGC->pAPI->procs.gcSave(pGC);
    }
    
    return MD_SUCCESS;
}

md_result md_gc_restore(md_gc* pGC)
{
    if (pGC == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcRestore) {
        return pGC->pAPI->procs.gcRestore(pGC);
    }
    
    return MD_SUCCESS;
}

void md_gc_set_matrix(md_gc* pGC, const md_matrix* pMatrix)
{
    if (pGC == NULL || pMatrix == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetMatrix) {
        pGC->pAPI->procs.gcSetMatrix(pGC, pMatrix);
    }
}

void md_gc_get_matrix(md_gc* pGC, md_matrix* pMatrix)
{
    if (pGC == NULL || pMatrix == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcGetMatrix) {
        pGC->pAPI->procs.gcGetMatrix(pGC, pMatrix);
    }
}

void md_gc_set_matrix_identity(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetMatrixIdentity) {
        pGC->pAPI->procs.gcSetMatrixIdentity(pGC);
    }
}

void md_gc_transform(md_gc* pGC, const md_matrix* pMatrix)
{
    if (pGC == NULL || pMatrix == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcTransform) {
        pGC->pAPI->procs.gcTransform(pGC, pMatrix);
    }
}

void md_gc_translate(md_gc* pGC, md_int32 offsetX, md_int32 offsetY)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcTranslate) {
        pGC->pAPI->procs.gcTranslate(pGC, offsetX, offsetY);
    }
}

void md_gc_rotate(md_gc* pGC, float rotationInRadians)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcRotate) {
        pGC->pAPI->procs.gcRotate(pGC, rotationInRadians);
    }
}

void md_gc_scale(md_gc* pGC, float scaleX, float scaleY)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcScale) {
        pGC->pAPI->procs.gcScale(pGC, scaleX, scaleY);
    }
}

void md_gc_set_miter_limit(md_gc* pGC, float limit)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetMiterLimit) {
        pGC->pAPI->procs.gcSetMiterLimit(pGC, limit);
    }
}

void md_gc_set_line_width(md_gc* pGC, md_int32 width)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineWidth) {
        pGC->pAPI->procs.gcSetLineWidth(pGC, width);
    }
}

void md_gc_set_line_cap(md_gc* pGC, md_line_cap cap)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineCap) {
        pGC->pAPI->procs.gcSetLineCap(pGC, cap);
    }
}

void md_gc_set_line_join(md_gc* pGC, md_line_join join)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineJoin) {
        pGC->pAPI->procs.gcSetLineJoin(pGC, join);
    }
}

void md_gc_set_line_dash(md_gc* pGC, const float* dashes, md_uint32 count)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (count > 16) {
        return; /* Count cannot be more than 16. */
    }

    if (pGC->pAPI->procs.gcSetLineDash) {
        pGC->pAPI->procs.gcSetLineDash(pGC, dashes, count);
    }
}

void md_gc_set_line_brush(md_gc* pGC, md_brush* pBrush)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineBrush) {
        pGC->pAPI->procs.gcSetLineBrush(pGC, pBrush);
    }
}

void md_gc_set_line_brush_solid(md_gc* pGC, md_color color)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineBrushSolid) {
        pGC->pAPI->procs.gcSetLineBrushSolid(pGC, color);
    }
}

void md_gc_set_line_brush_gc(md_gc* pGC, md_gc* pSrcGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetLineBrushGC) {
        pGC->pAPI->procs.gcSetLineBrushGC(pGC, pSrcGC);
    }
}

void md_gc_set_fill_brush(md_gc* pGC, md_brush* pBrush)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetFillBrush) {
        pGC->pAPI->procs.gcSetFillBrush(pGC, pBrush);
    }
}

void md_gc_set_fill_brush_solid(md_gc* pGC, md_color color)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetFillBrushSolid) {
        pGC->pAPI->procs.gcSetFillBrushSolid(pGC, color);
    }
}

void md_gc_set_fill_brush_gc(md_gc* pGC, md_gc* pSrcGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetFillBrushGC) {
        pGC->pAPI->procs.gcSetFillBrushGC(pGC, pSrcGC);
    }
}

void md_gc_set_text_fg_color(md_gc* pGC, md_color fgColor)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetTextFGColor) {
        pGC->pAPI->procs.gcSetTextFGColor(pGC, fgColor);
    }
}

void md_gc_set_text_bg_color(md_gc* pGC, md_color bgColor)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetTextBGColor) {
        pGC->pAPI->procs.gcSetTextBGColor(pGC, bgColor);
    }
}

md_color md_gc_get_text_fg_color(md_gc* pGC)
{
    if (pGC != NULL) {
        MD_ASSERT(pGC->pAPI != NULL);

        if (pGC->pAPI->procs.gcGetTextFGColor) {
            return pGC->pAPI->procs.gcGetTextFGColor(pGC);
        }
    }

    return md_rgba(0, 0, 0, 255);
}

md_color md_gc_get_text_bg_color(md_gc* pGC)
{
    if (pGC != NULL) {
        MD_ASSERT(pGC->pAPI != NULL);

        if (pGC->pAPI->procs.gcGetTextBGColor) {
            return pGC->pAPI->procs.gcGetTextBGColor(pGC);
        }
    }

    return md_rgba(0, 0, 0, 0);
}

void md_gc_set_blend_op(md_gc* pGC, md_blend_op op)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetBlendOp) {
        pGC->pAPI->procs.gcSetBlendOp(pGC, op);
    }
}

void md_gc_set_antialias_mode(md_gc* pGC, md_antialias_mode mode)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetAntialiasMode) {
        pGC->pAPI->procs.gcSetAntialiasMode(pGC, mode);
    }
}

void md_gc_set_fill_mode(md_gc* pGC, md_fill_mode mode)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetFillMode) {
        pGC->pAPI->procs.gcSetFillMode(pGC, mode);
    }
}

void md_gc_set_stretch_filter(md_gc* pGC, md_stretch_filter filter)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcSetStretchFilter) {
        pGC->pAPI->procs.gcSetStretchFilter(pGC, filter);
    }
}

void md_gc_move_to(md_gc* pGC, md_int32 x, md_int32 y)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcMoveTo) {
        pGC->pAPI->procs.gcMoveTo(pGC, x, y);
    }
}

void md_gc_line_to(md_gc* pGC, md_int32 x, md_int32 y)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcLineTo) {
        pGC->pAPI->procs.gcLineTo(pGC, x, y);
    }
}

void md_gc_rectangle(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcRectangle) {
        pGC->pAPI->procs.gcRectangle(pGC, left, top, right, bottom);
    }
}

void md_gc_arc(md_gc* pGC, md_int32 x, md_int32 y, md_int32 radius, float angle1InRadians, float angle2InRadians)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcArc) {
        pGC->pAPI->procs.gcArc(pGC, x, y, radius, angle1InRadians, angle2InRadians);
    }
}

void md_gc_curve_to(md_gc* pGC, md_int32 x1, md_int32 y1, md_int32 x2, md_int32 y2, md_int32 x3, md_int32 y3)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcCurveTo) {
        pGC->pAPI->procs.gcCurveTo(pGC, x1, y1, x2, y2, x3, y3);
    }
}

void md_gc_close_path(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcClosePath) {
        pGC->pAPI->procs.gcClosePath(pGC);
    }
}

void md_gc_rectangle_border_inner(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom, md_int32 borderWidth)
{
    /*
    There's two ways to do this. We can define the path as 4 rectangles (one for each side of the border) or we can define it as an outer
    path and an inner path. The advantage of the former is that it works will both types of fill modes whereas the later will only work with
    even-odd filling. For this reason I am using the former option as it should Just Work in more configurations.
    */

    /* 4-Rectangles Version. Make sure there is no overlap between the pieces. */
    md_gc_rectangle(pGC, left,                top,                  left  + borderWidth, bottom);               /* Left */
    md_gc_rectangle(pGC, right - borderWidth, top,                  right,               bottom);               /* Right */
    md_gc_rectangle(pGC, left  + borderWidth, top,                  right - borderWidth, top + borderWidth);    /* Top */
    md_gc_rectangle(pGC, left  + borderWidth, bottom - borderWidth, right - borderWidth, bottom);               /* Bottom */

    /* Outer / Inner Version. */
#if 0
    /* Outer */
    md_gc_move_to(pGC, left, top);
    md_gc_line_to(pGC, left, bottom);
    md_gc_line_to(pGC, right, bottom);
    md_gc_line_to(pGC, right, top);
    md_gc_line_to(pGC, left, top);

    /* Inner */
    left   += borderWidth;
    top    += borderWidth;
    right  -= borderWidth;
    bottom -= borderWidth;
    if (left < right && top < bottom) {
        md_gc_move_to(pGC, left, top);
        md_gc_line_to(pGC, left, bottom);
        md_gc_line_to(pGC, right, bottom);
        md_gc_line_to(pGC, right, top);
        md_gc_line_to(pGC, left, top);
    }
#endif
}

void md_gc_rectangle_border_outer(md_gc* pGC, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom, md_int32 borderWidth)
{
    md_gc_rectangle(pGC, left - borderWidth, top - borderWidth, left,                bottom + borderWidth); /* Left */
    md_gc_rectangle(pGC, right,              top - borderWidth, right + borderWidth, bottom + borderWidth); /* Right */
    md_gc_rectangle(pGC, left,               top - borderWidth, right,               top);                  /* Top */
    md_gc_rectangle(pGC, left,               bottom,            right,               bottom + borderWidth); /* Bottom */
}

void md_gc_clip(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcClip) {
        pGC->pAPI->procs.gcClip(pGC);
    }
}

void md_gc_reset_clip(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcResetClip) {
        pGC->pAPI->procs.gcResetClip(pGC);
    }
}

md_bool32 md_gc_is_point_inside_clip(md_gc* pGC, md_int32 x, md_int32 y)
{
    if (pGC == NULL) {
        return MD_FALSE;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcIsPointInsideClip) {
        return pGC->pAPI->procs.gcIsPointInsideClip(pGC, x, y);
    }

    return MD_FALSE;
}

void md_gc_fill(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcFill) {
        pGC->pAPI->procs.gcFill(pGC);
    }
}

void md_gc_stroke(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcStroke) {
        pGC->pAPI->procs.gcStroke(pGC);
    }
}

void md_gc_fill_and_stroke(md_gc* pGC)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pGC->pAPI->procs.gcFillAndStroke) {
        pGC->pAPI->procs.gcFillAndStroke(pGC);
    }
}

void md_gc_draw_gc(md_gc* pGC, md_gc* pSrcGC, md_int32 srcX, md_int32 srcY)
{
    if (pGC == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (pSrcGC == NULL) {
        return; /* Nothing to draw. */
    }

    if (pGC->pAPI->procs.gcDrawGC) {
        pGC->pAPI->procs.gcDrawGC(pGC, pSrcGC, srcX, srcY);
    }
}

void md_gc_draw_glyphs(md_gc* pGC, const md_item* pItem, const md_glyph* pGlyphs, size_t glyphCount, md_int32 x, md_int32 y)
{
    if (pGC == NULL || pItem == NULL || pGlyphs == NULL) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    if (glyphCount == 0) {
        return; /* Nothing to draw. */
    }

    if (pGC->pAPI->procs.gcDrawGlyphs) {
        pGC->pAPI->procs.gcDrawGlyphs(pGC, pItem, pGlyphs, glyphCount, x, y);
    }
}

void md_gc_draw_text_utf8(md_gc* pGC, md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, md_int32 x, md_int32 y, md_alignment originAlignmentX, md_alignment originAlignmentY, md_text_metrics* pMetrics)
{
    md_result result;

    if (pMetrics != NULL) {
        MD_ZERO_OBJECT(pMetrics);
    }

    if (pGC == NULL || pFont == NULL || pTextUTF8 == NULL || textLength == 0) {
        return;
    }

    if (textLength == (size_t)-1) {
        textLength = strlen(pTextUTF8);
    }

    /* TODO: Implement support for vertical text layouts. Not sure yet how to identify a font or script as vertical... */

    /* TODO: Implement support for backend-optimized implementations of this method. */
#if 0
    if (pGC->pAPI->gcDrawTextUTF8) {
        pGC->pAPI->gcDrawTextUTF8(pGC, pFont, pTextUTF8, textLength, x, y, originAlignmentX, originAlignmentY);
    } else
#endif
    {
        /* Generic implementation. This is optimized for simplicity rather than speed. Consider this to be the reference implementation. */
        md_int32 originX = 0;
        md_int32 originY = 0;
        md_int32 penX;
        md_int32 penY;
        md_text_metrics metrics;
        md_int32 lineSizeY = (pFont->metrics.ascent - pFont->metrics.descent);
        

        /* The starting position of the pen depends on the alignment of the origin. */
        if (originAlignmentX == md_alignment_left && originAlignmentY == md_alignment_top) {
            /* Simple case. The pen starts at [x] and [y] exactly. */
            originX = x;
            originY = y;
        } else {
            /* We need to measure the string to know where to start the pen. */
            result = md_font_get_text_metrics_utf8(pFont, pTextUTF8, textLength, &metrics);
            if (result != MD_SUCCESS) {
                return;
            }

            /* X */
            if (originAlignmentX == md_alignment_right) {
                originX = x - metrics.sizeX;
            } else if (originAlignmentX == md_alignment_center) {
                originX = x - (metrics.sizeX / 2);
            }

            /* Y */
            if (originAlignmentY == md_alignment_bottom) {
                originY = y - metrics.sizeY;
            } else if (originAlignmentY == md_alignment_center) {
                originY = y - (metrics.sizeY / 2);
            }
        }

        penX = originX;
        penY = originY;

        metrics.lPadding = 0;
        metrics.rPadding = 0;
        metrics.sizeX    = 0;
        metrics.sizeY    = lineSizeY;

        /* We've figured out the pen position, so now we can draw the text. */
        /* NOTE: Only supporting left-to-right, top-to-bottom for now. Still figuring out vertical text... */
        {
            md_itemize_state itemizeState;
            md_item   pItemsStack[1024];
            md_item*  pItemsHeap = NULL;
            md_item*  pItems = NULL;
            md_uint32 itemCount;
            md_uint32 iItem;

            /* First step is to itemize as usual. */
            itemCount = MD_COUNTOF(pItemsStack);
            result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsStack, &itemCount, &itemizeState);
            if (result == MD_SUCCESS) {
                pItems = &pItemsStack[0];
            } else if (result == MD_NO_SPACE) {
                /* Not enough room on the stack. Try again against the heap. */
                pItemsHeap = (md_item*)MD_MALLOC(sizeof(*pItemsHeap) * itemCount);
                if (pItemsHeap == NULL) {
                    return; /* Out of memory. */
                }

                result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsHeap, &itemCount, &itemizeState);
                if (result != MD_SUCCESS) {
                    return; /* Failed to itemize. */
                }

                pItems = pItemsHeap;
            } else {
                return; /* Failed to itemize. */
            }

            /* Now we just shape and draw. */
            if (originAlignmentX == md_alignment_left) {
                /* Simple case. No need for measuring lines. */
                md_int32 lineSizeX = 0;
                for (iItem = 0; iItem < itemCount; ++iItem) {
                    md_item* pItem = &pItems[iItem];

                    if (md_is_newline_utf8(pTextUTF8 + pItem->offset, pItem->length)) {
                        penX  = originX;
                        penY += lineSizeY;

                        metrics.sizeY += lineSizeY;
                        if (metrics.sizeX < lineSizeX) {
                            metrics.sizeX = lineSizeX;
                        }
                    } else {
                        md_text_metrics itemMetrics;
                        md_glyph  pGlyphsStack[4096];
                        md_glyph* pGlyphsHeap = NULL;
                        md_glyph* pGlyphs = NULL;
                        size_t glyphCount;

                        glyphCount = MD_COUNTOF(pGlyphsStack);
                        result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsStack, &glyphCount, NULL, &itemMetrics);
                        if (result == MD_SUCCESS) {
                            pGlyphs = &pGlyphsStack[0];
                        } else if (result == MD_NO_SPACE) {
                            /* Try the heap. */
                            pGlyphsHeap = (md_glyph*)MD_MALLOC(sizeof(*pGlyphsHeap) * glyphCount);
                            if (pGlyphsHeap == NULL) {
                                break;  /* Out of memory. */
                            }

                            result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsHeap, &glyphCount, NULL, &itemMetrics);
                            if (result != MD_SUCCESS) {
                                break;  /* Failed to shape this item. */
                            }

                            pGlyphs = pGlyphsHeap;
                        } else {
                            break;  /* Failed to shape this item. */
                        }

                        md_gc_draw_glyphs(pGC, &pItems[iItem], pGlyphs, glyphCount, penX, penY);

                        MD_FREE(pGlyphsHeap);
                        pGlyphsHeap = NULL;

                        penX += itemMetrics.sizeX;
                        lineSizeX += itemMetrics.sizeX;
                    }
                }

                if (metrics.sizeX < lineSizeX) {
                    metrics.sizeX = lineSizeX;
                }
            } else {
                /* Complicated case. We need to go line-by-line, measure it, then position it based on the alignment. */
                md_int32 lineSizeX;

                iItem = 0;
                while (iItem < itemCount) {  /* For each line... */
                    md_uint32 iLineBeg = iItem;
                    md_uint32 iLineEnd = iItem;
                    
                    lineSizeX = 0;

                    /* Find the range of items making up this line. */
                    for (; iLineEnd < itemCount; ++iLineEnd) {
                        md_item* pItem = &pItems[iLineEnd];
                        if (md_is_newline_utf8(pTextUTF8 + pItem->offset, pItem->length)) {
                            metrics.sizeY += lineSizeY;
                            if (metrics.sizeX < lineSizeX) {
                                metrics.sizeX = lineSizeX;
                            }

                            break;
                        } else {
                            md_text_metrics itemMetrics;
                            result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, NULL, NULL, NULL, &itemMetrics);
                            if (result != MD_SUCCESS) {
                                break;
                            }

                            lineSizeX += itemMetrics.sizeX;
                        }
                    }

                    if (iLineBeg != iLineEnd) {
                        if (originAlignmentX == md_alignment_right) {
                            penX = x - lineSizeX;
                        } else if (originAlignmentX == md_alignment_center) {
                            penX = x - (lineSizeX / 2);
                        } else {
                            penX = originX;
                        }
                    
                        for (iItem = iLineBeg; iItem < iLineEnd; ++iItem) {
                            md_item* pItem = &pItems[iItem];
                            md_text_metrics itemMetrics;
                            md_glyph  pGlyphsStack[4096];
                            md_glyph* pGlyphsHeap = NULL;
                            md_glyph* pGlyphs = NULL;
                            size_t glyphCount;

                            glyphCount = MD_COUNTOF(pGlyphsStack);
                            result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsStack, &glyphCount, NULL, &itemMetrics);
                            if (result == MD_SUCCESS) {
                                pGlyphs = &pGlyphsStack[0];
                            } else if (result == MD_NO_SPACE) {
                                /* Try the heap. */
                                pGlyphsHeap = (md_glyph*)MD_MALLOC(sizeof(*pGlyphsHeap) * glyphCount);
                                if (pGlyphsHeap == NULL) {
                                    break;  /* Out of memory. */
                                }

                                result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsHeap, &glyphCount, NULL, &itemMetrics);
                                if (result != MD_SUCCESS) {
                                    break;  /* Failed to shape this item. */
                                }

                                pGlyphs = pGlyphsHeap;
                            } else {
                                break;  /* Failed to shape this item. */
                            }

                            /* This hack is specifically for GDI. We need to clip against the item metrics or else we'll overwrite one pixel too far to the left. Very annoying. */
                            if (pGC->pAPI->backend == md_backend_gdi) {
                                md_gc_save(pGC);
                                {
                                    md_gc_rectangle(pGC, penX, penY, penX + itemMetrics.sizeX, penY + itemMetrics.sizeY);
                                    md_gc_clip(pGC);
                                    md_gc_draw_glyphs(pGC, pItem, pGlyphs, glyphCount, penX, penY);
                                }
                                md_gc_restore(pGC);
                            } else {
                                md_gc_draw_glyphs(pGC, pItem, pGlyphs, glyphCount, penX, penY);
                            }

                            MD_FREE(pGlyphsHeap);
                            pGlyphsHeap = NULL;

                            penX += itemMetrics.sizeX;
                        }
                    }

                    penY += lineSizeY;

                    if (metrics.sizeX < lineSizeX) {
                        metrics.sizeX = lineSizeX;
                    }

                    /* Go past the new-line item. */
                    iItem += 1;
                }
            }

            MD_FREE(pItemsHeap);
            md_free_itemize_state(&itemizeState);
        }

        if (pMetrics != NULL) {
            *pMetrics = metrics;
        }
    }
}

md_result md_font_get_text_layout_metrics_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout, md_text_metrics* pTextMetrics)
{
    md_int32 lineSizeX;
    md_int32 lineSizeY;

    if (pTextMetrics == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pTextMetrics);

    if (pFont == NULL || pTextUTF8 == NULL || pLayout == NULL) {
        return MD_INVALID_ARGS;
    }

    lineSizeY = (pFont->metrics.ascent - pFont->metrics.descent);

    if (textLength == 0) {
        pTextMetrics->sizeY = lineSizeY;
        return MD_SUCCESS;  /* Nothing to do. */
    }

#if 0
    if (pFont->pAPI->procs.fontGetTextLayoutMetricsUTF8) {
        return pFont->pAPI->procs.fontGetTextLayoutMetricsUTF8(pFont, pTextUTF8, textLength, pLayout, pMetrics);
    }
#endif
    {
        /* Generic implementation. */
        /*
        TODO:
            - Add support for word wrap.
            - Handle different line heights for different items (may happen with font fallback).
        */
        md_result result;
        md_int32  sizeX = 0;
        md_int32  sizeY = lineSizeY;
        md_itemize_state itemizeState;
        md_item   pItemsStack[1024];
        md_item*  pItemsHeap = NULL;
        md_item*  pItems = NULL;
        md_uint32 itemCount;
        md_uint32 iItem;
        md_int32  tabSizeXInPixels = pLayout->tabWidthInPixels;

        /* The first step is to itemize. */
        itemCount = MD_COUNTOF(pItemsStack);
        result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsStack, &itemCount, &itemizeState);
        if (result == MD_SUCCESS) {
            pItems = &pItemsStack[0];
        } else if (result == MD_NO_SPACE) {
            /* Not enough room on the stack. Try again against the heap. */
            pItemsHeap = (md_item*)MD_MALLOC(sizeof(*pItemsHeap) * itemCount);
            if (pItemsHeap == NULL) {
                return MD_OUT_OF_MEMORY;
            }

            result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsHeap, &itemCount, &itemizeState);
            if (result != MD_SUCCESS) {
                return result;
            }

            pItems = pItemsHeap;
        } else {
            return result;
        }

        /* We have the items so now we can measure them. */
        lineSizeX = 0;
        for (iItem = 0; iItem < itemCount; ++iItem) {
            /* No need to do shaping for new-lines. */
            if (md_is_newline_utf8(pTextUTF8 + pItems[iItem].offset, pItems[iItem].length)) {
                if (pLayout->singleLine == MD_FALSE) {
                    sizeY += lineSizeY;
                    if (sizeX < lineSizeX) {
                        sizeX = lineSizeX;
                    }
                    lineSizeX = 0;
                }
            } else if (md_is_tab_utf8(pTextUTF8 + pItems[iItem].offset, pItems[iItem].length)) {
                /*
                It's a tab item. To measure this we first find the width of the first tab character. And then we just take the remaining tab characters and multiply it by
                the tab width.
                */
                if (tabSizeXInPixels == 0) {
                    char spaceCh[1] = {' '};
                    md_text_metrics spaceMetrics;
                    if (md_font_get_text_metrics_utf8(pFont, spaceCh, 1, &spaceMetrics) != MD_SUCCESS) {
                        MD_ZERO_OBJECT(&spaceMetrics);
                        spaceMetrics.sizeX = 8;
                        spaceMetrics.sizeY = (pFont->metrics.ascent - pFont->metrics.descent);
                    }

                    tabSizeXInPixels = spaceMetrics.sizeX * pLayout->tabWidthInSpaces;

                    lineSizeX = ((lineSizeX / tabSizeXInPixels) * tabSizeXInPixels) + (pItems[iItem].length * tabSizeXInPixels);
                }
            } else {
                md_text_metrics itemMetrics;
                result = md_shape_utf8(pFont, &pItems[iItem], pTextUTF8, pItems[iItem].length, NULL, NULL, NULL, &itemMetrics);
                if (result != MD_SUCCESS) {
                    MD_FREE(pItemsHeap);
                    pItemsHeap = NULL;
                    md_free_itemize_state(&itemizeState);
                    return result;
                }

                lineSizeX += itemMetrics.sizeX;
            }
        }

        /* Don't forget to check the last line. */
        if (sizeX < lineSizeX) {
            sizeX = lineSizeX;
        }

        pTextMetrics->sizeX = sizeX;
        pTextMetrics->sizeY = sizeY;

        md_free_itemize_state(&itemizeState);
        return MD_SUCCESS;
    }
}

md_text_layout md_text_layout_init_default()
{
    md_text_layout layout;

    MD_ZERO_OBJECT(&layout);
    layout.alignmentX       = md_alignment_left;
    layout.alignmentY       = md_alignment_top;
    layout.fillBackground   = MD_TRUE;
    layout.singleLine       = MD_FALSE;
    layout.tabWidthInSpaces = 4;

    return layout;
}


#define MD_TEXT_LAYOUT_WHITESPACE   (1UL << 0)
#define MD_TEXT_LAYOUT_NEWLINE      (1UL << 1)

typedef struct
{
    void* pUserData;
    void      (* onInit)      (void* pUserData, const md_text_layout* pLayout);
    void      (* onUninit)    (void* pUserData, const md_text_layout* pLayout);
    void      (* onBeginItems)(void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY);
    void      (* onEndItems)  (void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY);
    void      (* onBeginLine) (void* pUserData, const md_text_layout* pLayout, const char* pTextUTF8, const md_item* pItems, md_uint32 itemCount, md_int32 penX, md_int32 penY, md_int32 sizeX, md_int32 sizeY);
    void      (* onEndLine)   (void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY);
    md_result (* onItem)      (void* pUserData, const md_text_layout* pLayout, const char* pTextUTF8, const md_item* pItem, const md_glyph* pGlyphs, md_uint32 glyphCount, const size_t* pClusters, md_int32 penX, md_int32 penY, md_int32 sizeX, md_int32 sizeY, md_uint32 flags);
} md_layout_callbacks;

/*
Main function for performing layout operations of text. Customizations of the layout breakdown are done via callbacks.
*/
static md_result md_text_layout_do_layout_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout, const md_layout_callbacks* pCallbacks)
{
    md_result result = MD_SUCCESS;
    md_itemize_state itemizeState;
    md_item   pItemsStack[1024];
    md_item*  pItemsHeap = NULL;
    md_item*  pItems = NULL;
    md_uint32 itemCount;
    md_uint32 iItem;
    size_t    pClustersStack[1024];
    size_t*   pClustersHeap = NULL;
    size_t*   pClusters = NULL;
    size_t    clustersHeapCap = 0;
    md_glyph  pGlyphsStack[1024];
    md_glyph* pGlyphsHeap = NULL;
    md_glyph* pGlyphs = NULL;
    size_t    glyphsHeapCap = 0;

    MD_ASSERT(pFont      != NULL);
    MD_ASSERT(pTextUTF8  != NULL);
    MD_ASSERT(pLayout    != NULL);
    MD_ASSERT(pCallbacks != NULL);

    /* Itemize first. */
    itemCount = MD_COUNTOF(pItemsStack);
    result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsStack, &itemCount, &itemizeState);
    if (result == MD_SUCCESS) {
        pItems = &pItemsStack[0];
    } else if (result == MD_NO_SPACE) {
        /* Not enough room on the stack. Try again against the heap. */
        pItemsHeap = (md_item*)MD_MALLOC(sizeof(*pItemsHeap) * itemCount);
        if (pItemsHeap == NULL) {
            return MD_OUT_OF_MEMORY;
        }

        result = md_itemize_utf8(pFont, pTextUTF8, textLength, pItemsHeap, &itemCount, &itemizeState);
        if (result != MD_SUCCESS) {
            return result;
        }

        pItems = pItemsHeap;
    } else {
        return result;
    }

    if (pCallbacks->onInit) {
        pCallbacks->onInit(pCallbacks->pUserData, pLayout);
    }
    {
        md_int32 originX;
        md_int32 originY;
        md_int32 penX;
        md_int32 penY;
        md_int32 lineSizeX;
        md_int32 lineSizeY = (pFont->metrics.ascent - pFont->metrics.descent);
        md_int32 tabSizeXInPixels = 0;

        originX = pLayout->boundsX + pLayout->textOffsetX + pLayout->padding.left;
        if (pLayout->borderWidth > 0) {
            originX += pLayout->borderWidth;
        }

        /* If we are not aligned to the top we need to measure so that we can position the text properly. */
        if (pLayout->alignmentY == md_alignment_top) {
            originY = pLayout->boundsY + pLayout->textOffsetY + pLayout->padding.top;
            if (pLayout->borderWidth > 0) {
                originY += pLayout->borderWidth;
            }
        } else {
            md_text_metrics metrics;
            result = md_font_get_text_layout_metrics_utf8(pFont, pTextUTF8, textLength, pLayout, &metrics);
            if (result != MD_SUCCESS) {
                md_free_itemize_state(&itemizeState);
                return result;
            }

            if (pLayout->alignmentY == md_alignment_bottom) {
                originY = pLayout->boundsY +  (pLayout->boundsSizeY - metrics.sizeY) + pLayout->textOffsetY - pLayout->padding.bottom;
                if (pLayout->borderWidth > 0) {
                    originY -= pLayout->borderWidth;
                }
            } else if (pLayout->alignmentY == md_alignment_center) {
                originY = pLayout->boundsY + ((pLayout->boundsSizeY - metrics.sizeY) / 2) + pLayout->textOffsetY;
            } else {
                return MD_INVALID_ARGS;
            }
        }

        penX = originX;
        penY = originY;


        if (pCallbacks->onBeginItems) {
            pCallbacks->onBeginItems(pCallbacks->pUserData, pLayout, penX, penY);
        }
        {
            iItem = 0;
            while (iItem < itemCount) {  /* For each line... */
                md_uint32 iLineBeg = iItem;
                md_uint32 iLineEnd = iItem;
                md_int32 lineX;

                lineSizeX = 0;

                /* Find the range of items making up this line. */
                for (; iLineEnd < itemCount; ++iLineEnd) {
                    md_item* pItem = &pItems[iLineEnd];
                    if (md_is_newline_utf8(pTextUTF8 + pItem->offset, pItem->length)) {
                        if (pLayout->singleLine == MD_FALSE) {
                            break;
                        }
                    } else if (md_is_tab_utf8(pTextUTF8 + pItem->offset, pItem->length)) {
                        /*
                        It's a tab item. To measure this we first find the width of the first tab character. And then we just take the remaining tab characters and multiply it by
                        the tab width.
                        */
                        if (tabSizeXInPixels == 0) {
                            char spaceCh[1] = {' '};
                            md_text_metrics spaceMetrics;
                            if (md_font_get_text_metrics_utf8(pFont, spaceCh, 1, &spaceMetrics) != MD_SUCCESS) {
                                MD_ZERO_OBJECT(&spaceMetrics);
                                spaceMetrics.sizeX = 8;
                                spaceMetrics.sizeY = (pFont->metrics.ascent - pFont->metrics.descent);
                            }

                            tabSizeXInPixels = spaceMetrics.sizeX * pLayout->tabWidthInSpaces;
                        }

                        lineSizeX = ((lineSizeX / tabSizeXInPixels) * tabSizeXInPixels) + (pItem->length * tabSizeXInPixels);
                    } else {
                        md_text_metrics itemMetrics;
                        result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, NULL, NULL, NULL, &itemMetrics);
                        if (result != MD_SUCCESS) {
                            break;
                        }

                        lineSizeX += itemMetrics.sizeX;

                        /* TODO: If word wrapping is enabled, and this item extends past the bounds, we need to split the line. */
                    }
                }

                lineX = penX;

                if (iLineBeg != iLineEnd) {
                    if (pLayout->alignmentX == md_alignment_right) {
                        penX = pLayout->boundsX + (pLayout->boundsSizeX - lineSizeX) - pLayout->padding.right;
                        if (pLayout->borderWidth > 0) {
                            penX -= pLayout->borderWidth;
                        }
                    } else if (pLayout->alignmentX == md_alignment_center) {
                        penX = pLayout->boundsX + ((pLayout->boundsSizeX - lineSizeX) / 2);
                    } else {
                        penX = pLayout->boundsX + pLayout->padding.left;
                        if (pLayout->borderWidth > 0) {
                            penX += pLayout->borderWidth;
                        }
                    }
                    penX += pLayout->textOffsetX;

                    lineX = penX;

                    if (pCallbacks->onBeginLine) {
                        pCallbacks->onBeginLine(pCallbacks->pUserData, pLayout, pTextUTF8, pItems + iLineBeg, iLineEnd - iLineBeg, penX, penY, lineSizeX, lineSizeY);
                    }
                    {
                        for (iItem = iLineBeg; iItem <= iLineEnd && iItem < itemCount; ++iItem) {
                            md_item* pItem = &pItems[iItem];
                            md_text_metrics itemMetrics;
                            size_t glyphCount;

                            if (pItem->length <= MD_COUNTOF(pClustersStack)) {
                                /* Put the clusters on the stack. */
                                pClusters = &pClustersStack[0];
                            } else {
                                /* Put the clusters on the heap. */
                                if (clustersHeapCap < pItem->length) {
                                    size_t* pNewClustersHeap = (size_t*)MD_REALLOC(pClustersHeap, sizeof(*pClustersHeap) * pItem->length);
                                    if (pNewClustersHeap == NULL) {
                                        MD_FREE(pClustersHeap);
                                        result = MD_OUT_OF_MEMORY;
                                        break;
                                    }

                                    pClustersHeap   = pNewClustersHeap;
                                    clustersHeapCap = pItem->length;
                                }

                                pClusters = pClustersHeap;
                            }

                            /* Now we need to shape so we can get access to glyphs and clusters. */
                            glyphCount = MD_COUNTOF(pGlyphsStack);
                            result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsStack, &glyphCount, pClusters, &itemMetrics);
                            if (result == MD_SUCCESS) {
                                pGlyphs = &pGlyphsStack[0];
                            } else if (result == MD_NO_SPACE) {
                                /* Try the heap. */
                                if (glyphCount > glyphsHeapCap) {
                                    md_glyph* pNewGlyphsHeap = (md_glyph*)MD_REALLOC(pGlyphsHeap, sizeof(*pGlyphsHeap) * glyphCount);
                                    if (pNewGlyphsHeap == NULL) {
                                        MD_FREE(pGlyphsHeap);
                                        result = MD_OUT_OF_MEMORY;
                                        break;
                                    }

                                    pGlyphsHeap   = pNewGlyphsHeap;
                                    glyphsHeapCap = glyphCount;
                                }

                                result = md_shape_utf8(pFont, pItem, pTextUTF8 + pItem->offset, pItem->length, pGlyphsHeap, &glyphCount, pClusters, &itemMetrics);
                                if (result != MD_SUCCESS) {
                                    break;  /* Failed to shape this item. */
                                }

                                pGlyphs = pGlyphsHeap;
                            } else {
                                break;  /* Failed to shape this item. */
                            }

                            /* Tabs need to be handled slightly differently to other characters due to it's different kind of spacing. */
                            if (md_is_tab_utf8(pTextUTF8 + pItem->offset, pItem->length)) {
                                /* It's a tab. */
                                md_int32 lineSizeXSoFar = penX - lineX;
                                md_int32 tabEndX = ((lineSizeXSoFar / tabSizeXInPixels) * tabSizeXInPixels) + (pItem->length * tabSizeXInPixels) + lineX;

                                if (pCallbacks->onItem) {
                                    result = pCallbacks->onItem(pCallbacks->pUserData, pLayout, pTextUTF8, pItem, pGlyphs, glyphCount, pClusters, penX, penY, tabEndX - penX, lineSizeY, MD_TEXT_LAYOUT_WHITESPACE);
                                    if (result == MD_CANCELLED) {
                                        break;
                                    }
                                }

                                penX = tabEndX;
                            } else {
                                md_uint32 flags = 0;
                                if (md_is_newline_utf8(pTextUTF8 + pItems[iItem].offset, pItems[iItem].length)) {
                                    flags |= MD_TEXT_LAYOUT_WHITESPACE | MD_TEXT_LAYOUT_NEWLINE;
                                }

                                if (pCallbacks->onItem) {
                                    pCallbacks->onItem(pCallbacks->pUserData, pLayout, pTextUTF8, pItem, pGlyphs, glyphCount, pClusters, penX, penY, itemMetrics.sizeX, itemMetrics.sizeY, 0);
                                    if (result == MD_CANCELLED) {
                                        break;
                                    }
                                }

                                penX += itemMetrics.sizeX;

                                if (!pLayout->singleLine && (flags & MD_TEXT_LAYOUT_NEWLINE) != 0) {
                                    penY += lineSizeY;
                                }
                            }
                        }
                    }
                    if (pCallbacks->onEndLine) {
                        pCallbacks->onEndLine(pCallbacks->pUserData, pLayout, penX, penY);
                    }
                }

                /* Don't continue iteration if we've cancelled.*/
                if (result == MD_CANCELLED) {
                    break;
                }
            }
        }
        if (pCallbacks->onEndItems) {
            pCallbacks->onEndItems(pCallbacks->pUserData, pLayout, penX, penY);
        }
    }
    if (pCallbacks->onUninit) {
        pCallbacks->onUninit(pCallbacks->pUserData, pLayout);
    }

    md_free_itemize_state(&itemizeState);

    MD_FREE(pGlyphsHeap);
    pGlyphsHeap = NULL;

    MD_FREE(pClustersHeap);
    pClustersHeap = NULL;

    MD_FREE(pItemsHeap);
    pItemsHeap = NULL;

    /* It's not an error if we cancelled. */
    if (result == MD_CANCELLED) {
        result =  MD_SUCCESS;
    }

    return result;
}


typedef struct
{
    md_api* pAPI;
    size_t cp;
    md_int32 x;
    md_int32 y;
    md_bool32 found;
} md_text_layout_cp_to_xy_utf8_state;

void md_text_layout_cp_to_xy_utf8__on_end_items(void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY)
{
    md_text_layout_cp_to_xy_utf8_state* pState = (md_text_layout_cp_to_xy_utf8_state*)pUserData;

    (void)pLayout;

    /* If we haven't yet found our coordinates we need to put ourselves at the end. */
    if (!pState->found) {
        pState->x = penX;
        pState->y = penY;
        pState->found = MD_TRUE;
    }
}

md_result md_text_layout_cp_to_xy_utf8__on_item(void* pUserData, const md_text_layout* pLayout, const char* pTextUTF8, const md_item* pItem, const md_glyph* pGlyphs, md_uint32 glyphCount, const size_t* pClusters, md_int32 penX, md_int32 penY, md_int32 sizeX, md_int32 sizeY, md_uint32 flags)
{
    md_text_layout_cp_to_xy_utf8_state* pState = (md_text_layout_cp_to_xy_utf8_state*)pUserData;

    (void)pLayout;
    (void)sizeX;
    (void)sizeY;
    (void)pTextUTF8;
    (void)flags;

    /* If this item contains our code point we can do a hit test on it. */
    if (pState->cp >= pItem->offset && pState->cp < pItem->offset + pItem->length) {
        /* It's in this item. Get the exact glyph position and then terminate. */
        pState->y = penY;
        
        md_index_to_x(pState->pAPI, pItem, pState->cp - pItem->offset, pItem->length, pClusters, pGlyphs, glyphCount, &pState->x);
        pState->x += penX;

        pState->found = MD_TRUE;
        return MD_CANCELLED;    /* We're done. Don't iterate any more items. */
    } else {
        /* It's not here. Continue. */
        return MD_SUCCESS;
    }
}

void md_text_layout_cp_to_xy_utf8(md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout, size_t cp, md_int32* pX, md_int32* pY)
{
    /* Safety. */
    if (pX != NULL) { *pX = 0; }
    if (pY != NULL) { *pY = 0; }

    if (pFont == NULL || pTextUTF8 == NULL || textLength == 0 || pLayout == NULL) {
        return;
    }

#if 0
    if (pGC->pAPI->procs.textLayoutCPToXYUTF8) {
        pGC->pAPI->procs.textLayoutCPToXYUTF8(pFont, pTextUTF8, textLength, pLayout, cp, pX, pY);
    } else
#endif
    {
        /* Generic implementation. This is based on the same algorithm as is used in md_gc_draw_text_layout_utf8(). */
        md_text_layout_cp_to_xy_utf8_state state;
        md_layout_callbacks callbacks;

        state.pAPI  = pFont->pAPI;
        state.cp    = cp;
        state.x     = 0;
        state.y     = 0;
        state.found = MD_FALSE;

        callbacks.pUserData    = &state;
        callbacks.onInit       = NULL;
        callbacks.onUninit     = NULL;
        callbacks.onBeginItems = NULL;
        callbacks.onEndItems   = md_text_layout_cp_to_xy_utf8__on_end_items;
        callbacks.onBeginLine  = NULL;
        callbacks.onEndLine    = NULL;
        callbacks.onItem       = md_text_layout_cp_to_xy_utf8__on_item;
        md_text_layout_do_layout_utf8(pFont, pTextUTF8, textLength, pLayout, &callbacks);

        if (pX != NULL) { *pX = state.x; }
        if (pY != NULL) { *pY = state.y; }
    }
}



typedef struct
{
    md_gc* pGC;
    md_int32 firstLineX;
    md_int32 firstLineY;
    md_int32 lineX;
    md_int32 lineY;
    md_int32 lineSizeX;
    md_int32 lineSizeY;
} md_gc_draw_text_layout_utf8_state;

void md_gc_draw_text_layout_utf8__on_init(void* pUserData, const md_text_layout* pLayout)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;
    md_gc_save(pState->pGC);

    (void)pLayout;
}

void md_gc_draw_text_layout_utf8__on_uninit(void* pUserData, const md_text_layout* pLayout)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;
    md_gc_restore(pState->pGC);

    (void)pLayout;
}

void md_gc_draw_text_layout_utf8__on_begin_items(void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;

    pState->firstLineX = penX;
    pState->firstLineY = penY;

    /* Draw the border first. */
    if (pLayout->borderWidth != 0) {
        if (pLayout->borderWidth > 0) {
            md_gc_rectangle_border_inner(pState->pGC, pLayout->boundsX, pLayout->boundsY, pLayout->boundsX + pLayout->boundsSizeX, pLayout->boundsY + pLayout->boundsSizeY, pLayout->borderWidth);
        } else {
            md_gc_rectangle_border_outer(pState->pGC, pLayout->boundsX, pLayout->boundsY, pLayout->boundsX + pLayout->boundsSizeX, pLayout->boundsY + pLayout->boundsSizeY, pLayout->borderWidth);
        }
        md_gc_set_fill_brush_solid(pState->pGC, pLayout->borderColor);
        md_gc_fill(pState->pGC);
    }

    /* We need to clip against the bounds in case of overflow. */
    md_gc_rectangle(pState->pGC, pLayout->boundsX + pLayout->padding.left, pLayout->boundsY + pLayout->padding.top, pLayout->boundsX + pLayout->boundsSizeX - pLayout->padding.right, pLayout->boundsY + pLayout->boundsSizeY - pLayout->padding.bottom);
    md_gc_clip(pState->pGC);
}

void md_gc_draw_text_layout_utf8__on_end_items(void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;

    (void)penX;

    /* Now we need to draw rectangles to fill the section above and below the main block of text. */
    if (pLayout->fillBackground) {
        md_color bgColor = md_gc_get_text_bg_color(pState->pGC);
        if (bgColor.a != 0) {
            md_int32 borderWidth = (pLayout->borderWidth > 0) ? pLayout->borderWidth : 0;

            md_gc_set_antialias_mode(pState->pGC, md_antialias_mode_none);
            md_gc_set_fill_brush_solid(pState->pGC, bgColor);
            md_gc_rectangle(pState->pGC, pLayout->boundsX + borderWidth, pLayout->boundsY + borderWidth, pLayout->boundsX + pLayout->boundsSizeX - borderWidth, pState->firstLineY);                                    /* Above the text. */
            md_gc_rectangle(pState->pGC, pLayout->boundsX + borderWidth, penY,                           pLayout->boundsX + pLayout->boundsSizeX - borderWidth, pLayout->boundsY + pLayout->boundsSizeY - borderWidth); /* Below the text. */
            md_gc_fill(pState->pGC);
        }
    }
}

void md_gc_draw_text_layout_utf8__on_begin_line(void* pUserData, const md_text_layout* pLayout, const char* pTextUTF8, const md_item* pItems, md_uint32 itemCount, md_int32 penX, md_int32 penY, md_int32 sizeX, md_int32 sizeY)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;

    (void)pLayout;
    (void)pTextUTF8;
    (void)pItems;
    (void)itemCount;

    pState->lineX     = penX;
    pState->lineY     = penY;
    pState->lineSizeX = sizeX;
    pState->lineSizeY = sizeY;
}

void md_gc_draw_text_layout_utf8__on_end_line(void* pUserData, const md_text_layout* pLayout, md_int32 penX, md_int32 penY)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;

    /* We need to draw the background of the layout bounds where the text doesn't cover. */
    if (pLayout->fillBackground) {
        md_color bgColor = md_gc_get_text_bg_color(pState->pGC);
        if (bgColor.a != 0) {
            md_int32 borderWidth = (pLayout->borderWidth > 0) ? pLayout->borderWidth : 0;

            md_gc_set_antialias_mode(pState->pGC, md_antialias_mode_none);
            md_gc_set_fill_brush_solid(pState->pGC, bgColor);
            md_gc_rectangle(pState->pGC, pLayout->boundsX + borderWidth, penY, pState->lineX,                                         penY + pState->lineSizeY);  /* Left of the line. */
            md_gc_rectangle(pState->pGC, penX,                           penY, pLayout->boundsX + pLayout->boundsSizeX - borderWidth, penY + pState->lineSizeY);  /* Right of the line. */
            md_gc_fill(pState->pGC);
        }
    }
}

md_result md_gc_draw_text_layout_utf8__on_item(void* pUserData, const md_text_layout* pLayout, const char* pTextUTF8, const md_item* pItem, const md_glyph* pGlyphs, md_uint32 glyphCount, const size_t* pClusters, md_int32 penX, md_int32 penY, md_int32 sizeX, md_int32 sizeY, md_uint32 flags)
{
    md_gc_draw_text_layout_utf8_state* pState = (md_gc_draw_text_layout_utf8_state*)pUserData;

    (void)pLayout;
    (void)pTextUTF8;
    (void)pClusters;

    if ((flags & MD_TEXT_LAYOUT_WHITESPACE) != 0) {
        /* Whitespace is drawn as a quad. */
        md_gc_set_antialias_mode(pState->pGC, md_antialias_mode_none);
        md_gc_set_fill_brush_solid(pState->pGC, md_gc_get_text_bg_color(pState->pGC));
        md_gc_rectangle(pState->pGC, penX, penY, penX + sizeX, penY + sizeY);
        md_gc_fill(pState->pGC);
    } else {
        /* This hack is specifically for GDI. We need to clip against the item metrics or else we'll overwrite one pixel too far to the left. Very annoying. */
        if (pState->pGC->pAPI->backend == md_backend_gdi) {
            md_gc_save(pState->pGC);
            {
                md_gc_rectangle(pState->pGC, penX, penY, penX + sizeX, penY + sizeY);
                md_gc_clip(pState->pGC);
                md_gc_draw_glyphs(pState->pGC, pItem, pGlyphs, glyphCount, penX, penY);
            }
            md_gc_restore(pState->pGC);
        } else {
            md_gc_draw_glyphs(pState->pGC, pItem, pGlyphs, glyphCount, penX, penY);
        }
    }

    return MD_SUCCESS;
}

void md_gc_draw_text_layout_utf8(md_gc* pGC, md_font* pFont, const md_utf8* pTextUTF8, size_t textLength, const md_text_layout* pLayout)
{
    if (pGC == NULL || pFont == NULL || pTextUTF8 == NULL || textLength == 0 || pLayout == NULL) {
        return;
    }

    if (textLength == (size_t)-1) {
        textLength = strlen(pTextUTF8);
    }

#if 0
    if (pGC->pAPI->procs.gcDrawTextLayoutUTF8) {
        pGC->pAPI->procs.gcDrawTextLayoutUTF8(pGC, pFont, pTextUTF8, textLength, pLayout);
    } else
#endif
    {
        md_gc_draw_text_layout_utf8_state state;
        md_layout_callbacks callbacks;

        MD_ZERO_OBJECT(&state);
        state.pGC = pGC;

        callbacks.pUserData    = &state;
        callbacks.onInit       = md_gc_draw_text_layout_utf8__on_init;
        callbacks.onUninit     = md_gc_draw_text_layout_utf8__on_uninit;
        callbacks.onBeginItems = md_gc_draw_text_layout_utf8__on_begin_items;
        callbacks.onEndItems   = md_gc_draw_text_layout_utf8__on_end_items;
        callbacks.onBeginLine  = md_gc_draw_text_layout_utf8__on_begin_line;
        callbacks.onEndLine    = md_gc_draw_text_layout_utf8__on_end_line;
        callbacks.onItem       = md_gc_draw_text_layout_utf8__on_item;
        md_text_layout_do_layout_utf8(pFont, pTextUTF8, textLength, pLayout, &callbacks);
    }
}

void md_gc_clear(md_gc* pGC, md_color color)
{
    if (pGC == NULL || color.a == 0) {
        return;
    }

    MD_ASSERT(pGC->pAPI != NULL);

    /* Clearing can be emulated if the backend does not provide an explicit implementation. */
    if (pGC->pAPI->procs.gcClear) {
        pGC->pAPI->procs.gcClear(pGC, color);
    } else {
        md_gc_save(pGC);
        {
            md_uint32 sizeX;
            md_uint32 sizeY;
            md_gc_get_size(pGC, &sizeX, &sizeY);

            md_gc_set_matrix_identity(pGC);
            md_gc_set_antialias_mode(pGC, md_antialias_mode_none);
            md_gc_set_fill_brush_solid(pGC, color);
            md_gc_rectangle(pGC, 0, 0, (md_int32)sizeX, (md_int32)sizeY);
            md_gc_fill(pGC);
        }
        md_gc_restore(pGC);
    }
}



void md_copy_image_data__no_conversion(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride, md_format format)
{
    md_uint32 bpp = md_get_bytes_per_pixel(format);

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

    if (dstStride == srcStride && dstStride == sizeX) {
        MD_COPY_MEMORY(pDst, pSrc, sizeX*sizeY*bpp);
    } else {
        md_uint32 x;
        md_uint32 y;

        /* Optimized path for 4 bytes per pixel. Can move everything per-pixel rather than per-byte. */
        if (bpp == 4) {
                  md_uint32* pDstRow =       (md_uint32*)pDst;
            const md_uint32* pSrcRow = (const md_uint32*)pSrc;

            for (y = 0; y < sizeY; ++y) {
                for (x = 0; x < sizeX; ++x) {
                    pDstRow[x] = pSrcRow[x];
                }

                pDstRow += dstStride;
                pSrcRow += srcStride;
            }
        } else {
                  md_uint8* pDstRow =       (md_uint8*)pDst;
            const md_uint8* pSrcRow = (const md_uint8*)pSrc;

            for (y = 0; y < sizeY; ++y) {
                for (x = 0; x < sizeX; ++x) {
                    md_uint32 b;
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

void md_copy_image_data__rgba_to_rgb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgba_to_bgra(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgba_to_bgr(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgba_to_argb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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


void md_copy_image_data__rgb_to_rgba(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgb_to_bgra(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgb_to_bgr(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__rgb_to_argb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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


void md_copy_image_data__bgra_to_rgba(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgra_to_rgb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgra_to_bgr(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgra_to_argb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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


void md_copy_image_data__bgr_to_rgba(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgr_to_rgb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgr_to_bgra(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__bgr_to_argb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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


void md_copy_image_data__argb_to_rgba(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__argb_to_rgb(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__argb_to_bgra(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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

void md_copy_image_data__argb_to_bgr(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_uint32 srcStride)
{
    md_uint32 x;
    md_uint32 y;
    md_uint8* pDstRow = (md_uint8*)pDst;
    const md_uint8* pSrcRow = (const md_uint8*)pSrc;

    MD_ASSERT(pDst != NULL);
    MD_ASSERT(pSrc != NULL);

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



void md_copy_image_data(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_format dstFormat, md_uint32 srcStride, md_format srcFormat)
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
        md_copy_image_data__no_conversion(pDst, pSrc, sizeX, sizeY, dstStride, srcStride, dstFormat);   /* Simple case. No conversion, just a copy. */
    } else {
        switch (dstFormat)
        {
            case md_format_rgba:
            {
                if (srcFormat == md_format_rgb) {
                    md_copy_image_data__rgb_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgra) {
                    md_copy_image_data__bgra_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgr) {
                    md_copy_image_data__bgr_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_argb) {
                    md_copy_image_data__argb_to_rgba(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case md_format_rgb:
            {
                if (srcFormat == md_format_rgba) {
                    md_copy_image_data__rgba_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgra) {
                    md_copy_image_data__bgra_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgr) {
                    md_copy_image_data__bgr_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_argb) {
                    md_copy_image_data__argb_to_rgb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case md_format_bgra:
            {
                if (srcFormat == md_format_rgba) {
                    md_copy_image_data__rgba_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_rgb) {
                    md_copy_image_data__rgb_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgr) {
                    md_copy_image_data__bgr_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_argb) {
                    md_copy_image_data__argb_to_bgra(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case md_format_bgr:
            {
                if (srcFormat == md_format_rgba) {
                    md_copy_image_data__rgba_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_rgb) {
                    md_copy_image_data__rgb_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgra) {
                    md_copy_image_data__bgra_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_argb) {
                    md_copy_image_data__argb_to_bgr(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case md_format_argb:
            {
                if (srcFormat == md_format_rgba) {
                    md_copy_image_data__rgba_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_rgb) {
                    md_copy_image_data__rgb_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgra) {
                    md_copy_image_data__bgra_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                } else if (srcFormat == md_format_bgr) {
                    md_copy_image_data__bgr_to_argb(pDst, pSrc, sizeX, sizeY, dstStride, srcStride);
                }
            } break;

            case md_format_unknown:
            default:
            {
            } break;
        }
    }
}

void md_flip_image_data_y(void* pDst, md_uint32 sizeX, md_uint32 sizeY, md_uint32 stride, md_format format)
{
    md_uint32 bpp;

    if (pDst == NULL) {
        return;
    }

    if (stride == 0) {
        stride = sizeX;
    }

    bpp = md_get_bytes_per_pixel(format);
    
    /*if (bpp == 4) {
        
    } else*/ {
        /* Generic naive implementation. */
        md_uint32 iRow;
        md_uint32 iCol;
        md_uint32 halfSizeY = sizeY >> 1;
        md_uint32 strideInBytes = stride * bpp;

        for (iRow = 0; iRow < halfSizeY; ++iRow) {
            md_uint8* pUpperRow = (md_uint8*)pDst + ((        iRow    ) * strideInBytes);
            md_uint8* pLowerRow = (md_uint8*)pDst + ((sizeY - iRow - 1) * strideInBytes);

            for (iCol = 0; iCol < sizeX; ++iCol) {
                md_uint32 iByte;
                for (iByte = 0; iByte < bpp; ++iByte) {
                    md_uint8 tmp = pUpperRow[iCol*bpp + iByte];
                    pUpperRow[iCol*bpp + iByte] = pLowerRow[iCol*bpp + iByte];
                    pLowerRow[iCol*bpp + iByte] = tmp;
                }
            }
        }
    }
}

void md_copy_and_flip_image_data_y(void* pDst, const void* pSrc, md_uint32 sizeX, md_uint32 sizeY, md_uint32 dstStride, md_format dstFormat, md_uint32 srcStride, md_format srcFormat)
{
    md_copy_image_data(pDst, pSrc, sizeX, sizeY, dstStride, dstFormat, srcStride, srcFormat);
    md_flip_image_data_y(pDst, sizeX, sizeY, dstStride, dstFormat);
}

md_uint32 md_get_bytes_per_pixel(md_format format)
{
    switch (format)
    {
        case md_format_rgba: return 4;
        case md_format_rgb:  return 3;
        case md_format_bgra: return 4;
        case md_format_bgr:  return 3;
        case md_format_argb: return 4;
        default: return 0;
    }
}

md_bool32 md_format_has_alpha(md_format format)
{
    switch (format)
    {
        case md_format_rgba:
        case md_format_bgra:
        case md_format_argb:
            return MD_TRUE;

        case md_format_rgb:
        case md_format_bgr:
        default:
            return MD_FALSE;
    }
}



/**************************************************************************************************************************************************************

Utilities

**************************************************************************************************************************************************************/
md_bool32 md_is_null_or_empty_utf8(const md_utf8* pUTF8)
{
    return pUTF8 == NULL || pUTF8[0] == '\0';
}

md_bool32 md_is_null_or_whitespace_utf8(const md_utf8* pUTF8)
{
    if (pUTF8 == NULL) {
        return MD_TRUE;
    }

    while (pUTF8[0] != '\0') {
        /* BEG AUTOGEN: is_null_or_whitespace_utf8 */
        if ((md_uint8)pUTF8[0] >= 0x9 && (md_uint8)pUTF8[0] <= 0xD) {
            pUTF8 += 1;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0x20) {
            pUTF8 += 1;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xC2 && (md_uint8)pUTF8[1] == 0x85) {
            pUTF8 += 2;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xC2 && (md_uint8)pUTF8[1] == 0xA0) {
            pUTF8 += 2;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE1 && (md_uint8)pUTF8[1] == 0x9A && (md_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x81) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x82) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x83) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x84) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x85) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x86) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x87) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x88) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x89) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x8A) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0xA8) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0xA9) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0xAF) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE2 && (md_uint8)pUTF8[1] == 0x81 && (md_uint8)pUTF8[2] == 0x9F) {
            pUTF8 += 3;
            continue;
        }
        if ((md_uint8)pUTF8[0] == 0xE3 && (md_uint8)pUTF8[1] == 0x80 && (md_uint8)pUTF8[2] == 0x80) {
            pUTF8 += 3;
            continue;
        }
        /* END AUTOGEN: is_null_or_whitespace_utf8 */

        return MD_FALSE;    /* Not whitespace. */
    }

    return MD_TRUE;
}
#endif  /* MINIDRAW_IMPLEMENTATION */

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
