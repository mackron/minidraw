/* Include this file after the implementation of minidraw.h */
#include <stdio.h>
#include <assert.h>

#define GLBIND_IMPLEMENTATION
#include "../external/glbind/glbind.h"

#if defined(MD_HAS_CAIRO)
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#endif

md_result md_fopen(FILE** ppFile, const char* filePath, const char* openMode)
{
#if _MSC_VER
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (filePath == NULL || openMode == NULL || ppFile == NULL) {
        return MD_INVALID_ARGS;
    }

#if _MSC_VER
    err = fopen_s(ppFile, filePath, openMode);
    if (err != 0) {
        return md_result_from_errno(err);
    }
#else
#if defined(_WIN32) || defined(__APPLE__)
    *ppFile = fopen(filePath, openMode);
#else
    #if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64 && defined(_LARGEFILE64_SOURCE)
        *ppFile = fopen64(filePath, openMode);
    #else
        *ppFile = fopen(filePath, openMode);
    #endif
#endif
    if (*ppFile == NULL) {
        md_result result = md_result_from_errno(errno);
        if (result == MD_SUCCESS) {
            return MD_ERROR;   /* Just a safety check to make sure we never ever return success when pFile == NULL. */
        }
    }
#endif

    return MD_SUCCESS;
}

md_result md_open_and_read_file(const char* filePath, size_t* pFileSizeOut, void** ppFileData)
{
    md_result result;
    md_uint64 fileSize;
    FILE* pFile;
    void* pFileData;
    size_t bytesRead;

    MD_ASSERT(filePath != NULL);

    result = md_fopen(&pFile, filePath, "rb");
    if (result != MD_SUCCESS) {
        return MD_FAILED_TO_OPEN_FILE;
    }

    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (fileSize > SIZE_MAX) {
        fclose(pFile);
        return MD_TOO_BIG;
    }

    pFileData = MD_MALLOC((size_t)fileSize);    /* <-- Safe cast due to the check above. */
    if (pFileData == NULL) {
        fclose(pFile);
        return MD_OUT_OF_MEMORY;
    }

    bytesRead = fread(pFileData, 1, (size_t)fileSize, pFile);
    if (bytesRead != fileSize) {
        MD_FREE(pFileData);
        fclose(pFile);
        return MD_FAILED_TO_READ_FILE;
    }

    fclose(pFile);

    if (pFileSizeOut) {
        *pFileSizeOut = (size_t)fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        MD_FREE(pFileData);
    }

    return MD_SUCCESS;
}

md_result md_open_and_read_file_utf8(const char* filePath, size_t* pFileSizeOutInUTF8, md_utf8** ppFileData)
{
    md_result result = md_open_and_read_file(filePath, pFileSizeOutInUTF8, (void**)ppFileData);
    *pFileSizeOutInUTF8 /= sizeof(md_utf8);

    return result;
}

md_result md_open_and_read_file_utf16(const char* filePath, size_t* pFileSizeOutInUTF16, md_utf16** ppFileData)
{
    md_result result = md_open_and_read_file(filePath, pFileSizeOutInUTF16, (void**)ppFileData);
    *pFileSizeOutInUTF16 /= sizeof(md_utf16);

    return result;
}

md_result md_open_and_read_file_utf32(const char* filePath, size_t* pFileSizeOutInUTF32, md_utf32** ppFileData)
{
    md_result result = md_open_and_read_file(filePath, pFileSizeOutInUTF32, (void**)ppFileData);
    *pFileSizeOutInUTF32 /= sizeof(md_utf32);

    return result;
}



typedef struct md_testapp md_testapp;

typedef md_result (* md_testapp_on_init)  (md_testapp* pApp);
typedef void      (* md_testapp_on_uninit)(md_testapp* pApp);
typedef void      (* md_testapp_on_size)  (md_testapp* pApp, md_uint32 sizeX, md_uint32 sizeY);
typedef void      (* md_testapp_on_paint) (md_testapp* pApp, md_gc* pGC);

typedef struct
{
    md_api_config apiConfig;
    GLBapi* pGL;    /* When not NULL, creates an OpenGL-enabled window. */
    const char* pWindowTitle;
    md_uint32 windowWidth;
    md_uint32 windowHeight;
    md_testapp_on_init onInit;
    md_testapp_on_uninit onUninit;
    md_testapp_on_size onSize;
    md_testapp_on_paint onPaint;
    void* pUserData;
} md_testapp_config;

struct md_testapp
{
    int argc;
    char** argv;
    GLBapi* pGL;
    md_api mt;
    md_testapp_on_init onInit;
    md_testapp_on_uninit onUninit;
    md_testapp_on_size onSize;
    md_testapp_on_paint onPaint;
    void* pUserData;
#if defined(MD_WIN32)
    HWND hWnd;  /* Main window handle. */
#elif defined(MD_APPLE)
#else
    #if defined(MD_HAS_CAIRO)
        GtkWidget* pWindowWidget;
        md_int32 posXGTK;
        md_int32 posYGTK;
        md_int32 sizeXGTK;
        md_int32 sizeYGTK;
    #endif
#endif
};

void md_testapp_scheduled_redraw(md_testapp* pApp, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom);

#if defined(MD_WIN32)
LRESULT md_testapp_MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    md_result result;
    md_testapp* pApp = (md_testapp*)GetWindowLongPtrA(hWnd, 0);

    switch (msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        case WM_SIZE:
        {
            md_uint32 sizeX = LOWORD(lParam);
            md_uint32 sizeY = HIWORD(lParam);

            if (pApp->onSize) {
                pApp->onSize(pApp, sizeX, sizeY);
            }

            md_testapp_scheduled_redraw(pApp, 0, 0, sizeX, sizeY);
        } break;

        case WM_PAINT:
        {
            md_gc_config gcConfig;
            md_gc gc;

            MD_ZERO_OBJECT(&gcConfig);

            if (pApp->pGL == NULL) {
                PAINTSTRUCT ps;
                HDC hDC = BeginPaint(hWnd, &ps);
                if (hDC != NULL) {
                    gcConfig.gdi.hDC = hDC;

                    result = md_gc_init(&pApp->mt, &gcConfig, &gc);
                    if (result == MD_SUCCESS) {
                        if (pApp->onPaint) {
                            pApp->onPaint(pApp, &gc);
                        }
                        md_gc_uninit(&gc);
                    }
                }
                EndPaint(hWnd, &ps);
            } else {
                /* OpenGL */
                result = md_gc_init(&pApp->mt, &gcConfig, &gc);
                if (result == MD_SUCCESS) {
                    if (pApp->onPaint) {
                        pApp->onPaint(pApp, &gc);
                    }
                    md_gc_uninit(&gc);
                }
                SwapBuffers(GetDC(hWnd));
            }
        } break;

        default: break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

HWND md_testapp_create_HWND(md_testapp_config* pConfig, md_testapp* pApp)
{
    WNDCLASSEXA wc;
    HWND hWnd;

    MD_ZERO_OBJECT(&wc);
    wc.cbSize        = sizeof(wc);
    wc.cbWndExtra    = sizeof(pApp);
    wc.lpfnWndProc   = (WNDPROC)md_testapp_MainWindowProc;
    wc.lpszClassName = "md_testapp";
    wc.hCursor       = LoadCursorA(NULL, MAKEINTRESOURCEA(32512));
    wc.hIcon         = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(101));
    wc.style         = CS_OWNDC | CS_DBLCLKS;
    if (!RegisterClassExA(&wc)) {
        return NULL;
    }

    hWnd = CreateWindowExA(0, "md_testapp", (pConfig->pWindowTitle != NULL) ? pConfig->pWindowTitle : "Test App", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, pConfig->windowWidth, pConfig->windowHeight, NULL, NULL, NULL, NULL);
    if (hWnd == NULL) {
        return NULL;
    }

    SetWindowLongPtrA(hWnd, 0, (LONG_PTR)pApp);   /* HWND user data. */

    /* May want to enable the window for OpenGL. */
    if (pConfig->pGL != NULL) {
        SetPixelFormat(GetDC(hWnd), glbGetPixelFormat(), glbGetPFD());
        pConfig->pGL->wglMakeCurrent(GetDC(hWnd), glbGetRC());
    }
    

    ShowWindow(hWnd, SW_SHOWNORMAL);

    return hWnd;
}
#elif defined(MD_APPLE)
/* Implement me. */
#else
#define MD_TESTAPP_GTK_PROPERTY_NAME_USERDATA   "mt.testapp.userdata"

static gboolean md_window__on_close__gtk(GtkWidget* pWidget, GdkEvent* pEvent, gpointer pUserData)
{
    (void)pWidget;
    (void)pEvent;

    md_testapp* pApp = (md_testapp*)pUserData;
    if (pApp == NULL) {
        return MD_TRUE;
    }

    gtk_main_quit();
    
    return MD_TRUE;
}

static gboolean md_window__on_configure__gtk(GtkWidget* pWidget, GdkEventConfigure* pEvent, gpointer pUserData)
{
    md_testapp* pApp = (md_testapp*)pUserData;
    if (pApp == NULL) {
        return MD_FALSE;
    }

    if (pEvent->x != pApp->posXGTK || pEvent->y != pApp->posYGTK) {    
        /* Position has changed. */
        pApp->posXGTK = pEvent->x;
        pApp->posYGTK = pEvent->y;
    }

    if (pEvent->width != pApp->sizeXGTK || pEvent->height != pApp->sizeYGTK) {    
        /* Size has changed. */
        pApp->sizeXGTK = pEvent->width;
        pApp->sizeYGTK = pEvent->height;
        if (pApp->onSize) {
            pApp->onSize(pApp, (md_uint32)pEvent->width, (md_uint32)pEvent->height);
        }
    }

    return MD_FALSE;
}

static gboolean md_window__on_draw__gtk(GtkWidget* pWidget, cairo_t* cr, gpointer pUserData)
{
    md_result result;
    md_gc_config gcConfig;
    md_gc gc;

    md_testapp* pApp = (md_testapp*)pUserData;
    if (pApp == NULL) {
        return MD_FALSE;
    }

#if 0
    double clipLeft;
    double clipTop;
    double clipRight;
    double clipBottom;
    cairo_clip_extents(cr, &clipLeft, &clipTop, &clipRight, &clipBottom);
    printf("on_draw: %f %f %f %f\n", clipLeft, clipTop, clipRight, clipBottom);
#endif

    MD_ZERO_OBJECT(&gcConfig);
    gcConfig.sizeX = pApp->sizeXGTK;
    gcConfig.sizeY = pApp->sizeYGTK;
    gcConfig.cairo.pCairoContext = cr;

    result = md_gc_init(&pApp->mt, &gcConfig, &gc);
    if (result == MD_SUCCESS) {
        if (pApp->onPaint) {
            pApp->onPaint(pApp, &gc);
        }
        md_gc_uninit(&gc);
    }

    return MD_TRUE;
}

GtkWidget* md_testapp_create_window__gtk(md_testapp_config* pConfig, md_testapp* pApp)
{
    GtkWidget* pWindowWidget;

    pWindowWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (pWindowWidget == NULL) {
        md_uninit(&pApp->mt);
        return NULL;
    }

    g_object_set_data(G_OBJECT(pWindowWidget), MD_TESTAPP_GTK_PROPERTY_NAME_USERDATA, pApp);

    gtk_window_set_title(GTK_WINDOW(pWindowWidget), (pConfig->pWindowTitle != NULL) ? pConfig->pWindowTitle : "Test App");
    gtk_window_set_resizable(GTK_WINDOW(pWindowWidget), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(pWindowWidget), (gint)pConfig->windowWidth, (gint)pConfig->windowHeight);
    gtk_window_resize(GTK_WINDOW(pWindowWidget), (gint)pConfig->windowWidth, (gint)pConfig->windowHeight);

    g_signal_connect(pWindowWidget, "delete-event",    G_CALLBACK(md_window__on_close__gtk),     pApp); /* Close */
    g_signal_connect(pWindowWidget, "configure-event", G_CALLBACK(md_window__on_configure__gtk), pApp); /* Size/Move */
    g_signal_connect(pWindowWidget, "draw",            G_CALLBACK(md_window__on_draw__gtk),      pApp); /* Paint */

    /* Show the window. */
    gtk_widget_realize(pWindowWidget);
    gtk_window_present(GTK_WINDOW(pWindowWidget));

    return pWindowWidget;
}
#endif

md_result md_testapp_init(md_testapp_config* pConfig, md_testapp* pApp)
{
    md_result result;

    if (pApp == NULL || pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pApp);
    pApp->pGL       = pConfig->pGL;
    pApp->onInit    = pConfig->onInit;
    pApp->onUninit  = pConfig->onUninit;
    pApp->onSize    = pConfig->onSize;
    pApp->onPaint   = pConfig->onPaint;
    pApp->pUserData = pConfig->pUserData;

    /* API */
    result = md_init(&pConfig->apiConfig, &pApp->mt);
    if (result != MD_SUCCESS) {
        return result;
    }

    /* Window */
#if defined(MD_WIN32)
    pApp->hWnd = md_testapp_create_HWND(pConfig, pApp);
    if (pApp->hWnd == NULL) {
        md_uninit(&pApp->mt);
        return MD_ERROR;
    }
#elif defined(MD_APPLE)
    /* Implement me. */
#else
    #if defined(MD_SUPPORT_CAIRO)
        if (pConfig->apiConfig.backend == md_backend_cairo) {
            /* GTK */
            if (!gtk_init_check(0, NULL)) {
                md_uninit(&pApp->mt);
                return MD_ERROR;
            }

            pApp->pWindowWidget = md_testapp_create_window__gtk(pConfig, pApp);
            if (pApp->pWindowWidget == NULL) {
                md_uninit(&pApp->mt);
                return MD_ERROR;
            }
        }
    #endif
    #if defined(MD_HAS_XFT)
        #if 0
        if (pConfig->apiConfig.backend == md_backend_xft) {
            /* X */
            md_uninit(&pApp->mt);
            return MD_ERROR;    /* Not yet implemented. */
        }
        #endif
    #endif
#endif

    if (pApp->onInit) {
        result = pApp->onInit(pApp);
        if (result != MD_SUCCESS) {
        #if defined(MD_WIN32)
            DestroyWindow(pApp->hWnd);
        #elif defined(MD_APPLE)
            /* TODO: Implement me. */
        #else
            gtk_widget_destroy(GTK_WIDGET(pApp->pWindowWidget));
        #endif
            md_uninit(&pApp->mt);
            return result;
        }
    }

    return MD_SUCCESS;
}

void md_testapp_uninit(md_testapp* pApp)
{
    if (pApp == NULL) {
        return;
    }

    if (pApp->onUninit) {
        pApp->onUninit(pApp);
    }

#if defined(MD_WIN32)
    DestroyWindow(pApp->hWnd);
#elif defined(MD_APPLE)
    /* TODO: Implement me. */
#else
    if (pApp->mt.backend == md_backend_cairo) {
        gtk_widget_destroy(GTK_WIDGET(pApp->pWindowWidget));
    }
#endif

    md_uninit(&pApp->mt);
}

int md_testapp_run(md_testapp* pApp)
{
    int exitCode = 0;

#if defined(MD_WIN32)
    for (;;) {
        MSG msg;
        BOOL result = GetMessageA(&msg, NULL, 0, 0);
        if (result == -1) {
            exitCode = -1;   /* Unknown error. */
            break;
        }
        if (result == 0) {
            exitCode = (int)msg.wParam;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
#elif defined(MD_APPLE)
    /* TODO: Implement me. */
#else
    gtk_main();
    exitCode = 0;
#endif

    (void)pApp;

    return exitCode;
}

void md_testapp_get_size(md_testapp* pApp, md_uint32* pSizeX, md_uint32* pSizeY)
{
#if defined(MD_WIN32)
    RECT rect;
#endif

    MD_ASSERT(pApp != NULL);

    if (pSizeX) *pSizeX = 0;
    if (pSizeY) *pSizeY = 0;

#if defined(MD_WIN32)
    GetClientRect(pApp->hWnd, &rect);
    if (pSizeX) *pSizeX = (md_uint32)(rect.right  - rect.left);
    if (pSizeY) *pSizeY = (md_uint32)(rect.bottom - rect.top);
#elif defined(MD_APPLE)
    /* Implement me. */
#else
    if (pSizeX) *pSizeX = (md_uint32)pApp->sizeXGTK;
    if (pSizeY) *pSizeY = (md_uint32)pApp->sizeYGTK;
#endif
}

void md_testapp_scheduled_redraw(md_testapp* pApp, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom)
{
#if defined(MD_WIN32)
    RECT rect;
#endif

    MD_ASSERT(pApp != NULL);

#if defined(MD_WIN32)
    rect.left   = left;
    rect.top    = top;
    rect.right  = right;
    rect.bottom = bottom;
    RedrawWindow(pApp->hWnd, &rect, NULL, RDW_INVALIDATE);
#elif defined(MD_APPLE)
    /* Implement me. */
#else
    gtk_widget_queue_draw_area(GTK_WIDGET(pApp->pWindowWidget), left, top, (right - left), (bottom - top));
#endif
}
