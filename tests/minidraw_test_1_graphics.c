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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#include <stdio.h>

#define WINDOW_SIZE_X   1280
#define WINDOW_SIZE_Y   720
#define LOGO_SIZE_X     880
#define LOGO_SIZE_Y     260

md_font g_font;
md_color g_textBGColor;
md_color g_textFGColor;
md_color g_midLineColor;
const char* g_text = "minidraw";

void draw_logo(md_gc* pGC)
{
    md_result result;
    md_uint32 gcSizeX;
    md_uint32 gcSizeY;
    md_uint32 midX;
    md_uint32 midY;
    float dashes[2] = {4, 4};

    /* We position everything relative to the size of the graphics context. */
    md_gc_get_size(pGC, &gcSizeX, &gcSizeY);
    midX = (md_int32)(gcSizeX / 2);
    midY = (md_int32)(gcSizeY / 2);

    md_gc_save(pGC);
    {
        /* Clear the whole surface before drawing anything just to be keep it clean. */
        md_gc_clear(pGC, g_textBGColor);

        md_gc_set_text_bg_color(pGC, md_rgba(0, 0, 0, 0));      /* Foreground color is controlled by the clear color. */
        md_gc_set_text_fg_color(pGC, g_textFGColor);
        md_gc_draw_text_utf8(pGC, &g_font, "Hello,\r\nWorld!", (size_t)-1, midX*2, 0, md_alignment_right, md_alignment_top);

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

        /* Text output. This is centered on the screen. */
        {
            md_gc_set_blend_op(pGC, md_blend_op_src_over);
            md_gc_set_antialias_mode(pGC, md_antialias_mode_none);  /* Results in thick lines is AA is enabled. */
            md_gc_set_line_width(pGC, 1);
            md_gc_set_text_bg_color(pGC, md_rgba(0, 0, 0, 0));      /* Foreground color is controlled by the clear color. */
            md_gc_set_text_fg_color(pGC, g_textFGColor);

            {
                md_itemize_state itemizeState;
                md_item items[128];
                md_uint32 itemCount = MD_COUNTOF(items);
                result = md_itemize_utf8(&g_font, g_text, strlen(g_text), items, &itemCount, &itemizeState);
                if (result == MD_SUCCESS) {
                    md_uint32 iItem;
                    for (iItem = 0; iItem < itemCount; ++iItem) {
                        md_item* pItem = &items[iItem];
                        md_glyph glyphs[128];
                        size_t glyphCount = MD_COUNTOF(glyphs);
                        size_t clusters[128];
                        md_text_metrics runMetrics;
                        result = md_shape_utf8(&g_font, pItem, g_text + pItem->offset, pItem->length, glyphs, &glyphCount, clusters, &runMetrics);
                        if (result == MD_SUCCESS) {
                            md_int32 cornerRadius = 4;
                            md_int32 textPosX = midX - (runMetrics.sizeX/2);
                            md_int32 textPosY = midY - (runMetrics.sizeY/2);
                            md_gc_draw_glyphs(pGC, pItem, glyphs, glyphCount, textPosX, textPosY);

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
                            md_gc_move_to(pGC, textPosX + runMetrics.sizeX, 0);
                            md_gc_line_to(pGC, textPosX + runMetrics.sizeX, gcSizeY);
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
                            md_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            md_gc_arc(    pGC, textPosX,                                   textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            md_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY                                                   );
                            md_gc_arc(    pGC, textPosX,                                   textPosY                                                   , cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY                                                   );
                            md_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY                                                   , cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY + g_font.metrics.ascent                           );
                            md_gc_arc(    pGC, textPosX,                                   textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY + g_font.metrics.ascent                           );
                            md_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MD_RADIANSF(360));
                            md_gc_fill(pGC);
                        }
                    }

                    md_free_itemize_state(&itemizeState);
                }
            }
        }
    }
    md_gc_restore(pGC);
}

md_result on_init(md_testapp* pApp)
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
            printf("Failed to create logo GC.\n");
            return result;
        }

        draw_logo(&gc);

        pImageData = MD_MALLOC(LOGO_SIZE_X * LOGO_SIZE_Y * 4);
        if (pImageData == NULL) {
            md_gc_uninit(&gc);
            md_font_uninit(&g_font);
            printf("Failed to allocate memory for logo image data.\n");
            return MD_OUT_OF_MEMORY;
        }

        result = md_gc_get_image_data(&gc, md_format_rgba, pImageData);
        md_gc_uninit(&gc);

        if (result != MD_SUCCESS) {
            MD_FREE(pImageData);
            md_font_uninit(&g_font);
            printf("Failed to retrieve logo image data.\n");
            return result;
        }

        if (stbi_write_png("res/logo.png", LOGO_SIZE_X, LOGO_SIZE_Y, 4, pImageData, LOGO_SIZE_X*4) == 0) {
            MD_FREE(pImageData);
            md_font_uninit(&g_font);
            printf("Failed to write PNG.\n");
            return MD_ERROR;
        }

        MD_FREE(pImageData);
    }

    return MD_SUCCESS;
}

void on_uninit(md_testapp* pApp)
{
    md_font_uninit(&g_font);
}

void on_size(md_testapp* pApp, md_uint32 sizeX, md_uint32 sizeY)
{
    md_testapp_scheduled_redraw(pApp, 0, 0, sizeX, sizeY);
}

void on_paint(md_testapp* pApp, md_gc* pGC)
{
    draw_logo(pGC);
}

int main(int argc, char** argv)
{
    int exitCode;
    md_result result;
    md_testapp_config appConfig;
    md_testapp app;

    MD_ZERO_OBJECT(&appConfig);
    appConfig.pWindowTitle = "Graphics Test";
    appConfig.windowWidth  = WINDOW_SIZE_X;
    appConfig.windowHeight = WINDOW_SIZE_Y;
    appConfig.onInit       = on_init;
    appConfig.onUninit     = on_uninit;
    appConfig.onSize       = on_size;
    appConfig.onPaint      = on_paint;
    appConfig.pUserData    = NULL;

#if defined(MD_WIN32)
    appConfig.apiConfig.backend = md_backend_gdi;
#elif defined(MD_APPLE)
    appConfig.apiConfig.backend = md_backend_coregraphics;
#else
    appConfig.apiConfig.backend = md_backend_cairo;
#endif
    
    result = md_testapp_init(&appConfig, &app);
    if (result != MD_SUCCESS) {
        return (int)result;
    }

    exitCode = md_testapp_run(&app);

    md_testapp_uninit(&app);
    return exitCode;
}
