#if 0   /* Enable fopen64(). */
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#endif

#define MINITYPE_IMPLEMENTATION
#include "../minitype.h"
#include "../tests/minitype_testcommon.c"

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

mt_font g_font;
mt_color g_textBGColor;
mt_color g_textFGColor;
mt_color g_midLineColor;
const char* g_text = "minitype";

void draw_logo(mt_gc* pGC)
{
    mt_result result;
    mt_uint32 gcSizeX;
    mt_uint32 gcSizeY;
    mt_uint32 midX;
    mt_uint32 midY;
    float dashes[2] = {4, 4};

    /* We position everything relative to the size of the graphics context. */
    mt_gc_get_size(pGC, &gcSizeX, &gcSizeY);
    midX = (mt_int32)(gcSizeX / 2);
    midY = (mt_int32)(gcSizeY / 2);

    mt_gc_save(pGC);
    {
        /* Clear the whole surface before drawing anything just to be keep it clean. */
        mt_gc_clear(pGC, g_textBGColor);

        /* Origin lines. */
        {
    #if 0
            printf("midX = %d, midY = %d\n", midX, midY);

            mt_gc_set_line_brush_solid(pGC, g_midLineColor);
            mt_gc_set_line_width(pGC, 2);

            /* Vertical. */
            mt_gc_move_to(pGC, midX, 0);
            mt_gc_line_to(pGC, midX, gcSizeY);

            /* Horizontal. */
            mt_gc_move_to(pGC, 0,       midY);
            mt_gc_line_to(pGC, gcSizeX, midY);

            /* Draw the lines. */
            mt_gc_stroke(pGC);
    #endif
        }

        /* Text output. This is centered on the screen. */
        {
            mt_gc_set_blend_op(pGC, mt_blend_op_src_over);
            mt_gc_set_antialias_mode(pGC, mt_antialias_mode_none);  /* Results in thick lines is AA is enabled. */
            mt_gc_set_line_width(pGC, 1);
            mt_gc_set_text_bg_color(pGC, mt_rgba(0, 0, 0, 0));      /* Foreground color is controlled by the clear color. */
            mt_gc_set_text_fg_color(pGC, g_textFGColor);

            {
                mt_itemize_state itemizeState;
                mt_item items[128];
                mt_uint32 itemCount = MT_COUNTOF(items);
                result = mt_itemize_utf8(&g_font, g_text, strlen(g_text), items, &itemCount, &itemizeState);
                if (result == MT_SUCCESS) {
                    mt_uint32 iItem;
                    for (iItem = 0; iItem < itemCount; ++iItem) {
                        mt_item* pItem = &items[iItem];
                        mt_glyph glyphs[128];
                        size_t glyphCount = MT_COUNTOF(glyphs);
                        size_t clusters[128];
                        mt_text_metrics runMetrics;
                        result = mt_shape_utf8(&g_font, pItem, g_text + pItem->offset, pItem->length, glyphs, &glyphCount, clusters, &runMetrics);
                        if (result == MT_SUCCESS) {
                            mt_int32 cornerRadius = 4;
                            mt_int32 textPosX = midX - (runMetrics.sizeX/2);
                            mt_int32 textPosY = midY - (runMetrics.sizeY/2);
                            mt_gc_draw_glyphs(pGC, pItem, glyphs, glyphCount, textPosX, textPosY);

                            /* Text position. */
                            /*
                            mt_gc_set_fill_brush_solid(pGC, mt_rgb(255, 0, 0));
                            mt_gc_rectangle(pGC, textPosX - 4, textPosY - 4, textPosX + 4, textPosY + 4);
                            mt_gc_fill(pGC);
                            */

                            /* Left and right boundaries. */
                            mt_gc_set_line_brush_solid(pGC, mt_rgba(160, 64, 32, 255));
                            mt_gc_move_to(pGC, textPosX, 0);
                            mt_gc_line_to(pGC, textPosX, gcSizeY);
                            mt_gc_move_to(pGC, textPosX + runMetrics.sizeX, 0);
                            mt_gc_line_to(pGC, textPosX + runMetrics.sizeX, gcSizeY);
                            mt_gc_stroke(pGC);

                            /* Base line. */
                            mt_gc_set_line_brush_solid(pGC, mt_rgba(0, 0, 0, 255));
                            mt_gc_move_to(pGC, 0,       textPosY + g_font.metrics.ascent);
                            mt_gc_line_to(pGC, gcSizeX, textPosY + g_font.metrics.ascent);
                            mt_gc_stroke(pGC);

                            /* Ascender line. */
                            mt_gc_set_line_dash(pGC, dashes, MT_COUNTOF(dashes));
                            mt_gc_move_to(pGC, 0,       textPosY);
                            mt_gc_line_to(pGC, gcSizeX, textPosY);
                            mt_gc_stroke(pGC);

                            /* Descender line. */
                            mt_gc_set_line_dash(pGC, dashes, MT_COUNTOF(dashes));
                            mt_gc_move_to(pGC, 0,       textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            mt_gc_line_to(pGC, gcSizeX, textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            mt_gc_stroke(pGC);

                            /* Corder dots. */
                            mt_gc_set_antialias_mode(pGC, mt_antialias_mode_default);
                            mt_gc_set_fill_brush_solid(pGC, mt_rgba(92, 92, 92, 255));
                            mt_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            mt_gc_arc(    pGC, textPosX,                                   textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY + (g_font.metrics.ascent - g_font.metrics.descent));
                            mt_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY + (g_font.metrics.ascent - g_font.metrics.descent), cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY                                                   );
                            mt_gc_arc(    pGC, textPosX,                                   textPosY                                                   , cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY                                                   );
                            mt_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY                                                   , cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_move_to(pGC, textPosX + cornerRadius,                    textPosY + g_font.metrics.ascent                           );
                            mt_gc_arc(    pGC, textPosX,                                   textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_move_to(pGC, textPosX + runMetrics.sizeX + cornerRadius, textPosY + g_font.metrics.ascent                           );
                            mt_gc_arc(    pGC, textPosX + runMetrics.sizeX,                textPosY + g_font.metrics.ascent                           , cornerRadius, 0, MT_RADIANSF(360));
                            mt_gc_fill(pGC);
                        }
                    }

                    mt_free_itemize_state(&itemizeState);
                }
            }
        }
    }
    mt_gc_restore(pGC);
}

mt_result on_init(mt_testapp* pApp)
{
    mt_result result;
    mt_font_config fontConfig;

    MT_ZERO_OBJECT(&fontConfig);
    fontConfig.family = "Liberation Serif";
    fontConfig.sizeInPixels = 120;
    fontConfig.weight = mt_font_weight_normal;
    fontConfig.slant = mt_font_slant_none;
    fontConfig.antialiasMode = mt_antialias_mode_subpixel;
    result = mt_font_init(&pApp->mt, &fontConfig, &g_font);
    if (result != MT_SUCCESS) {
        return result;
    }

    g_textBGColor  = mt_rgba(255, 255, 255, 255);
    g_textFGColor  = mt_rgba(0,   0,   0,   255);
    g_midLineColor = mt_rgba(192, 192, 192, 255);

    /* Generate the logo. This tests retrieving of the image data of the device context. */
    {
        mt_gc gc;
        mt_gc_config gcConfig;
        void* pImageData;

        MT_ZERO_OBJECT(&gcConfig);
        gcConfig.sizeX  = LOGO_SIZE_X;
        gcConfig.sizeY  = LOGO_SIZE_Y;
        gcConfig.format = mt_format_rgba;
        result = mt_gc_init(&pApp->mt, &gcConfig, &gc);
        if (result != MT_SUCCESS) {
            mt_font_uninit(&g_font);
            printf("Failed to create logo GC.\n");
            return result;
        }

        draw_logo(&gc);

        pImageData = MT_MALLOC(LOGO_SIZE_X * LOGO_SIZE_Y * 4);
        if (pImageData == NULL) {
            mt_gc_uninit(&gc);
            mt_font_uninit(&g_font);
            printf("Failed to allocate memory for logo image data.\n");
            return MT_OUT_OF_MEMORY;
        }

        result = mt_gc_get_image_data(&gc, mt_format_rgba, pImageData);
        mt_gc_uninit(&gc);

        if (result != MT_SUCCESS) {
            MT_FREE(pImageData);
            mt_font_uninit(&g_font);
            printf("Failed to retrieve logo image data.\n");
            return result;
        }

        if (stbi_write_png("res/logo.png", LOGO_SIZE_X, LOGO_SIZE_Y, 4, pImageData, LOGO_SIZE_X*4) == 0) {
            MT_FREE(pImageData);
            mt_font_uninit(&g_font);
            printf("Failed to write PNG.\n");
            return result;
        }

        MT_FREE(pImageData);
    }

    return MT_SUCCESS;
}

void on_uninit(mt_testapp* pApp)
{
    mt_font_uninit(&g_font);
}

void on_size(mt_testapp* pApp, mt_uint32 sizeX, mt_uint32 sizeY)
{
    mt_testapp_scheduled_redraw(pApp, 0, 0, sizeX, sizeY);
}

void on_paint(mt_testapp* pApp, mt_gc* pGC)
{
    draw_logo(pGC);
}

int main(int argc, char** argv)
{
    int exitCode;
    mt_result result;
    mt_testapp_config appConfig;
    mt_testapp app;

    MT_ZERO_OBJECT(&appConfig);
    appConfig.pWindowTitle = "Graphics Test";
    appConfig.windowWidth  = WINDOW_SIZE_X;
    appConfig.windowHeight = WINDOW_SIZE_Y;
    appConfig.onInit       = on_init;
    appConfig.onUninit     = on_uninit;
    appConfig.onSize       = on_size;
    appConfig.onPaint      = on_paint;
    appConfig.pUserData    = NULL;
    
    result = mt_testapp_init(&appConfig, &app);
    if (result != MT_SUCCESS) {
        return (int)result;
    }

    exitCode = mt_testapp_run(&app);

    mt_testapp_uninit(&app);
    return exitCode;
}
