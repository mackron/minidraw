#define MINIDRAW_IMPLEMENTATION
#include "../minidraw.h"
#include "../tests/minidraw_testcommon.c"

md_font g_font;

md_result on_init(md_testapp* pApp)
{
    /* This is called by minidraw's testing framework at initialization time. This is where you will want to create fonts and whatnot. */

    md_result result;
    md_font_config fontConfig;

    MD_ZERO_OBJECT(&fontConfig);
    fontConfig.family = "Liberation Serif";
    fontConfig.sizeInPixels = 60;
    fontConfig.weight = md_font_weight_normal;
    fontConfig.slant = md_font_slant_none;
    fontConfig.antialiasMode = md_antialias_mode_subpixel;
    result = md_font_init(&pApp->mt, &fontConfig, &g_font);
    if (result != MD_SUCCESS) {
        return result;
    }

    return result;
}

void on_uninit(md_testapp* pApp)
{
    /* This is called when the application is being terminated. */

    md_font_uninit(&g_font);
}

void on_paint(md_testapp* pApp, md_gc* pGC)
{
    /*
    This is called when the window is being redrawn. Do all of your drawing here. In this example we're showing off a few different APIs for variety, but
    it certainly doesn't cover everything.
    */

    const char* text = "Hello, World!";
    md_uint32 gcSizeX;
    md_uint32 gcSizeY;
    md_text_metrics textMetrics;
    md_int32 textPosX;
    md_int32 textPosY;

    /* We need the size of the graphics context so we can know where to position the text. */
    md_gc_get_size(pGC, &gcSizeX, &gcSizeY);

    /* Clear the background to white. */
    md_gc_clear(pGC, md_rgb(48, 128, 224));

    /*
    Draw the text in the middle of the GC. There are several ways to do this, but in this example we're measuring the string first just to demonstrate
    the string measuring API.
    */
    md_font_get_text_metrics_utf8(&g_font, text, (size_t)-1, &textMetrics);

    /* Position the string in the middle of the graphics context. */
    textPosX = (gcSizeX-textMetrics.sizeX)/2;
    textPosY = (gcSizeY-textMetrics.sizeY)/2;

    /*
    If you want to clip your text, you can do it like this. In this example we're saving the GC state before setting the clipping region, then drawing
    the text, then restoring the previous GC state.
    */
    md_gc_save(pGC);
    {
        md_gc_rectangle(pGC, textPosX, textPosY, textPosX + textMetrics.sizeX, textPosY + textMetrics.sizeY);
        md_gc_clip(pGC);

        /* Draw the text using a high level helper API. More advanced text processing may require the low level text API. */
        md_gc_set_text_bg_color(pGC, md_rgb(255, 255, 255));
        md_gc_set_text_fg_color(pGC, md_rgb(0, 0, 0));
        md_gc_draw_text_utf8(pGC, &g_font, text, (size_t)-1, textPosX, textPosY, md_alignment_left, md_alignment_top, &textMetrics);
    }
    md_gc_restore(pGC);
    
    /* Draw an outline around the text. We know this will not be a rotated quad, so to ensure we get nice crisp lines we will want to disable anti-aliasing. */
    md_gc_set_antialias_mode(pGC, md_antialias_mode_none);
    md_gc_set_line_brush_solid(pGC, md_rgb(0, 0, 0));
    md_gc_rectangle(pGC, textPosX, textPosY, textPosX + textMetrics.sizeX, textPosY + textMetrics.sizeY);
    md_gc_stroke(pGC);
}

int main(int argc, char** argv)
{
    int exitCode;
    md_result result;
    md_testapp_config appConfig;
    md_testapp app;

    MD_ZERO_OBJECT(&appConfig);
    appConfig.pWindowTitle = "Graphics Test";
    appConfig.windowWidth  = 640;
    appConfig.windowHeight = 480;
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
    
    result = md_testapp_init(&appConfig, &app);
    if (result != MD_SUCCESS) {
        return (int)result;
    }

    exitCode = md_testapp_run(&app);

    md_testapp_uninit(&app);
    return exitCode;
}