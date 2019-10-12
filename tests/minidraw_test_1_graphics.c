#if 0   /* Enable fopen64(). */
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#endif

#define MINIDRAW_IMPLEMENTATION
#include "../minidraw.h"
#include "../tests/minidraw_testcommon.c"

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcomment"          /* // comments are not allowed in this language [-Wcomment] */
    #pragma GCC diagnostic ignored "-Wc99-extensions"   /* initializer for aggregate is not a compile-time constant */
#endif
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4204)   /* nonstandard extension used: non-constant aggregate initializer */
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#include <stdio.h>

#define WINDOW_SIZE_X   1280
#define WINDOW_SIZE_Y   720
#define LOGO_SIZE_X     880
#define LOGO_SIZE_Y     260

md_font g_font;
md_font g_fontSmall;
md_color g_textBGColor;
md_color g_textFGColor;
md_color g_midLineColor;
const char* g_text = "minidraw";
const char* g_textLong =
    "Lorem ipsum dolor sit amet, consectetur\n"
    "adipiscing elit. Morbi sit amet purus\n"
    "egestas, consequat tellus in, lacinia magna.";

void draw_logo(md_gc* pGC)
{
    /*md_result result;*/
    md_uint32 gcSizeX;
    md_uint32 gcSizeY;
    md_uint32 midX;
    md_uint32 midY;
    md_text_metrics metrics;
    float dashes[2] = {4, 4};

    /* We position everything relative to the size of the graphics context. */
    md_gc_get_size(pGC, &gcSizeX, &gcSizeY);
    midX = (md_int32)(gcSizeX / 2);
    midY = (md_int32)(gcSizeY / 2);

    md_gc_save(pGC);
    {
        /* Clear the whole surface before drawing anything just to be keep it clean. */
        md_gc_clear(pGC, g_textBGColor);

        /* Text output. This is centered on the screen. */
        {
            md_gc_set_blend_op(pGC, md_blend_op_src_over);
            md_gc_set_antialias_mode(pGC, md_antialias_mode_none);  /* Results in thick lines if AA is enabled. */
            md_gc_set_text_bg_color(pGC, md_rgba(0, 0, 0, 0));      /* Foreground color is controlled by the clear color. */
            md_gc_set_text_fg_color(pGC, g_textFGColor);

            md_gc_draw_text_utf8(pGC, &g_font, g_text, (size_t)-1, midX, midY, md_alignment_center, md_alignment_center, &metrics);

            {
                md_int32 cornerRadius = 4;
                md_int32 textPosX = midX - (metrics.sizeX/2);
                md_int32 textPosY = midY - (metrics.sizeY/2);

                /* Text position. */
                /*
                md_gc_set_fill_brush_solid(pGC, md_rgb(255, 0, 0));
                md_gc_rectangle(pGC, textPosX - 4, textPosY - 4, textPosX + 4, textPosY + 4);
                md_gc_fill(pGC);
                */

                /* Left and right boundaries. */
                md_gc_set_line_brush_solid(pGC, md_rgba(160, 64, 32, 255));
                md_gc_move_to(pGC, textPosX, 0);
                md_gc_line_to(pGC, textPosX, gcSizeY);
                md_gc_move_to(pGC, textPosX + metrics.sizeX, 0);
                md_gc_line_to(pGC, textPosX + metrics.sizeX, gcSizeY);
                md_gc_stroke(pGC);

                /* Base line. */
                md_gc_set_line_brush_solid(pGC, md_rgba(0, 0, 0, 255));
                md_gc_move_to(pGC, 0,       textPosY + g_font.metrics.ascent);
                md_gc_line_to(pGC, gcSizeX, textPosY + g_font.metrics.ascent);
                md_gc_stroke(pGC);

                /* Ascender line. */
                md_gc_set_line_dash(pGC, dashes, MD_COUNTOF(dashes));
                md_gc_move_to(pGC, 0,       textPosY);
                md_gc_line_to(pGC, gcSizeX, textPosY);
                md_gc_stroke(pGC);

                /* Descender line. */
                md_gc_set_line_dash(pGC, dashes, MD_COUNTOF(dashes));
                md_gc_move_to(pGC, 0,       textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                md_gc_line_to(pGC, gcSizeX, textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                md_gc_stroke(pGC);

                /* Corder dots. */
                md_gc_set_antialias_mode(pGC, md_antialias_mode_default);
                md_gc_set_fill_brush_solid(pGC, md_rgba(92, 92, 92, 255));
                md_gc_move_to(pGC, textPosX + cornerRadius,                 textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                md_gc_arc(    pGC, textPosX,                                textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MD_RADIANSF(360));
                md_gc_move_to(pGC, textPosX + metrics.sizeX + cornerRadius, textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                md_gc_arc(    pGC, textPosX + metrics.sizeX,                textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MD_RADIANSF(360));
                md_gc_move_to(pGC, textPosX + cornerRadius,                 textPosY                                                   );
                md_gc_arc(    pGC, textPosX,                                textPosY                                                   , cornerRadius, 0, MD_RADIANSF(360));
                md_gc_move_to(pGC, textPosX + metrics.sizeX + cornerRadius, textPosY                                                   );
                md_gc_arc(    pGC, textPosX + metrics.sizeX,                textPosY                                                   , cornerRadius, 0, MD_RADIANSF(360));
                md_gc_move_to(pGC, textPosX + cornerRadius,                 textPosY + g_font.metrics.ascent                           );
                md_gc_arc(    pGC, textPosX,                                textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MD_RADIANSF(360));
                md_gc_move_to(pGC, textPosX + metrics.sizeX + cornerRadius, textPosY + g_font.metrics.ascent                           );
                md_gc_arc(    pGC, textPosX + metrics.sizeX,                textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MD_RADIANSF(360));
                md_gc_fill(pGC);
            }
        }
    }
    md_gc_restore(pGC);
}

md_result on_init(md_app* pApp)
{
    md_result result;
    md_font_config fontConfig;

    MD_ZERO_OBJECT(&fontConfig);
    fontConfig.family = "Liberation Serif";
    fontConfig.sizeInPixels = 120;
    fontConfig.weight = md_font_weight_normal;
    fontConfig.slant = md_font_slant_none;
    fontConfig.antialiasMode = md_antialias_mode_subpixel;
    result = md_font_init(&pApp->mt, &fontConfig, &g_font);
    if (result != MD_SUCCESS) {
        return result;
    }

    MD_ZERO_OBJECT(&fontConfig);
    fontConfig.family = "Liberation Sans";
    fontConfig.sizeInPoints = 12;
    fontConfig.weight = md_font_weight_normal;
    fontConfig.slant = md_font_slant_none;
    fontConfig.antialiasMode = md_antialias_mode_subpixel;
    result = md_font_init(&pApp->mt, &fontConfig, &g_fontSmall);
    if (result != MD_SUCCESS) {
        md_font_uninit(&g_font);
        return result;
    }

    g_textBGColor  = md_rgba(255, 255, 255, 255);
    g_textFGColor  = md_rgba(0,   0,   0,   255);
    g_midLineColor = md_rgba(192, 192, 192, 255);

    /* Generate the logo. This tests retrieving of the image data of the device context. */
    {
        md_gc gc;
        md_gc_config gcConfig;
        void* pImageData;

        MD_ZERO_OBJECT(&gcConfig);
        gcConfig.sizeX  = LOGO_SIZE_X;
        gcConfig.sizeY  = LOGO_SIZE_Y;
        gcConfig.format = md_format_rgba;
        result = md_gc_init(&pApp->mt, &gcConfig, &gc);
        if (result != MD_SUCCESS) {
            md_font_uninit(&g_font);
            md_font_uninit(&g_fontSmall);
            printf("Failed to create logo GC.\n");
            return result;
        }

        draw_logo(&gc);

        pImageData = MD_MALLOC(LOGO_SIZE_X * LOGO_SIZE_Y * 4);
        if (pImageData == NULL) {
            md_gc_uninit(&gc);
            md_font_uninit(&g_font);
            md_font_uninit(&g_fontSmall);
            printf("Failed to allocate memory for logo image data.\n");
            return MD_OUT_OF_MEMORY;
        }

        result = md_gc_get_image_data(&gc, md_format_rgba, pImageData);
        md_gc_uninit(&gc);

        if (result != MD_SUCCESS) {
            MD_FREE(pImageData);
            md_font_uninit(&g_font);
            md_font_uninit(&g_fontSmall);
            printf("Failed to retrieve logo image data.\n");
            return result;
        }

        if (stbi_write_png("res/logo.png", LOGO_SIZE_X, LOGO_SIZE_Y, 4, pImageData, LOGO_SIZE_X*4) == 0) {
            MD_FREE(pImageData);
            md_font_uninit(&g_font);
            md_font_uninit(&g_fontSmall);
            printf("Failed to write PNG.\n");
            return MD_ERROR;
        }

        MD_FREE(pImageData);
    }

    return MD_SUCCESS;
}

void on_uninit(md_app* pApp)
{
    md_font_uninit(&g_font);
    md_font_uninit(&g_fontSmall);

    (void)pApp;
}

void on_paint(md_app* pApp, md_gc* pGC)
{
    draw_logo(pGC);

    /* Draw some text to test alignment. */
    md_gc_save(pGC);
    {
        md_uint32 gcSizeX;
        md_uint32 gcSizeY;
        md_uint32 midX;
        md_uint32 midY;
        md_text_metrics metrics;

        /* We position everything relative to the size of the graphics context. */
        md_gc_get_size(pGC, &gcSizeX, &gcSizeY);
        midX = (md_int32)(gcSizeX / 2);
        midY = (md_int32)(gcSizeY / 2);

        md_gc_set_text_bg_color(pGC, md_rgba(0, 0, 0, 0));      /* Foreground color is controlled by the clear color. */
        md_gc_set_text_fg_color(pGC, g_textFGColor);

        /*md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, 0,      0,      md_alignment_left,   md_alignment_top,    &metrics);*/
        md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, midX,   0,      md_alignment_center, md_alignment_top,    &metrics);
        md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, midX*2, 0,      md_alignment_right,  md_alignment_top,    &metrics);

        md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, 0,      midY*2, md_alignment_left,   md_alignment_bottom, &metrics);
        md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, midX,   midY*2, md_alignment_center, md_alignment_bottom, &metrics);
        md_gc_draw_text_utf8(pGC, &g_fontSmall, g_textLong, (size_t)-1, midX*2, midY*2, md_alignment_right,  md_alignment_bottom, &metrics);

        /* Text Layout */
        md_gc_save(pGC);
        {
            md_text_layout layout;

            /*md_gc_translate(pGC, 128, 128);
            md_gc_rotate(pGC, MD_RADIANSF(90));*/

            md_gc_set_text_fg_color(pGC, md_rgb(0,   0,  0));
            md_gc_set_text_bg_color(pGC, md_rgb(224, 64, 32));

            MD_ZERO_OBJECT(&layout);
            layout.boundsX = 32;
            layout.boundsY = 32;
            layout.boundsSizeX = 256;
            layout.boundsSizeY = 64;
            layout.textOffsetX = 0;
            layout.textOffsetY = 0;
            layout.alignmentX = md_alignment_right;
            layout.alignmentY = md_alignment_center;
            layout.tabWidthInPixels = 0;
            layout.tabWidthInSpaces = 4;
            layout.fillBackground = MD_TRUE;
            layout.singleLine = MD_FALSE;
            md_gc_draw_text_layout_utf8(pGC, &g_fontSmall, "Hello,\t\tWorld!\n$1,\t00\t0,000.00", (size_t)-1, &layout);

            md_gc_set_antialias_mode(pGC, md_antialias_mode_none);
            md_gc_set_line_brush_solid(pGC, md_rgb(0, 0, 0));
            md_gc_rectangle(pGC, layout.boundsX, layout.boundsY, layout.boundsX + layout.boundsSizeX, layout.boundsY + layout.boundsSizeY);
            md_gc_stroke(pGC);
        }
        md_gc_restore(pGC);
        


        /* Origin lines. */
        {
    #if 0
            printf("midX = %d, midY = %d\n", midX, midY);

            md_gc_set_line_brush_solid(pGC, g_midLineColor);
            md_gc_set_line_width(pGC, 2);

            /* Vertical. */
            md_gc_move_to(pGC, midX, 0);
            md_gc_line_to(pGC, midX, gcSizeY);

            /* Horizontal. */
            md_gc_move_to(pGC, 0,       midY);
            md_gc_line_to(pGC, gcSizeX, midY);

            /* Draw the lines. */
            md_gc_stroke(pGC);
    #endif
        }
    }
    md_gc_restore(pGC);

    (void)pApp;
}

int main(int argc, char** argv)
{
    int exitCode;
    md_result result;
    md_app_config appConfig;
    md_app app;

    (void)argc;
    (void)argv;

    MD_ZERO_OBJECT(&appConfig);
    appConfig.pWindowTitle = "Graphics Test";
    appConfig.windowWidth  = WINDOW_SIZE_X;
    appConfig.windowHeight = WINDOW_SIZE_Y;
    appConfig.onInit       = on_init;
    appConfig.onUninit     = on_uninit;
    appConfig.onPaint      = on_paint;
    appConfig.pUserData    = NULL;

#if defined(MD_WIN32)
    appConfig.apiConfig.backend = md_backend_gdi;
#elif defined(MD_APPLE)
    appConfig.apiConfig.backend = md_backend_coregraphics;
#else
    appConfig.apiConfig.backend = md_backend_cairo;
#endif
    
    result = md_app_init(&appConfig, &app);
    if (result != MD_SUCCESS) {
        return (int)result;
    }

    exitCode = md_app_run(&app);

    md_app_uninit(&app);
    return exitCode;
}
