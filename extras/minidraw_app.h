#ifndef MINIDRAW_APP_H
#define MINIDRAW_APP_H
typedef struct md_app md_app;

typedef md_result (* md_app_on_init)             (md_app* pApp);
typedef void      (* md_app_on_uninit)           (md_app* pApp);
typedef void      (* md_app_on_size)             (md_app* pApp, md_uint32 sizeX, md_uint32 sizeY);
typedef void      (* md_app_on_paint)            (md_app* pApp, md_gc* pGC);
typedef void      (* md_app_on_mouse_move)       (md_app* pApp, md_int32 x, md_int32 y);
typedef void      (* md_app_on_mouse_button_down)(md_app* pApp, md_int32 x, md_int32 y, md_uint32 button);
typedef void      (* md_app_on_mouse_button_up)  (md_app* pApp, md_int32 x, md_int32 y, md_uint32 button);

#define MD_MOUSE_BUTTON_LEFT    1
#define MD_MOUSE_BUTTON_RIGHT   2
#define MD_MOUSE_BUTTON_MIDDLE  3

typedef struct
{
    md_api_config apiConfig;
    const char* pWindowTitle;
    md_uint32 windowWidth;
    md_uint32 windowHeight;
    md_app_on_init onInit;
    md_app_on_uninit onUninit;
    md_app_on_size onSize;
    md_app_on_paint onPaint;
    md_app_on_mouse_move onMouseMove;
    md_app_on_mouse_button_down onMouseButtonDown;
    md_app_on_mouse_button_up onMouseButtonUp;
    void* pUserData;
#if defined(GLBIND_H)
    GLBapi* pGL;    /* When not NULL, creates an OpenGL-enabled window. */
#endif
} md_app_config;

struct md_app
{
    int argc;
    char** argv;
    md_api mt;
    md_app_on_init onInit;
    md_app_on_uninit onUninit;
    md_app_on_size onSize;
    md_app_on_paint onPaint;
    md_app_on_mouse_move onMouseMove;
    md_app_on_mouse_button_down onMouseButtonDown;
    md_app_on_mouse_button_up onMouseButtonUp;
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
#if defined(GLBIND_H)
    GLBapi* pGL;
#endif
};

md_result md_app_init(md_app_config* pConfig, md_app* pApp);
void md_app_uninit(md_app* pApp);
int md_app_run(md_app* pApp);
void md_app_get_size(md_app* pApp, md_uint32* pSizeX, md_uint32* pSizeY);
void md_app_scheduled_redraw(md_app* pApp, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom);
void md_app_scheduled_redraw_whole(md_app* pApp);

#endif  /* MINIDRAW_APP_H */


#if defined(MINIDRAW_IMPLEMENTATION)
#if defined(MD_WIN32)
#define MD_GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define MD_GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

static md_uint32 md_wm_event_to_mouse_button__win32(UINT msg)
{
    switch (msg)
    {
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        {
            return MD_MOUSE_BUTTON_RIGHT;
        }

        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        {
            return MD_MOUSE_BUTTON_MIDDLE;
        }

        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        default:
        {
            return MD_MOUSE_BUTTON_LEFT;
        }
    }
}

LRESULT md_app_MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    md_result result;
    md_app* pApp = (md_app*)GetWindowLongPtrA(hWnd, 0);

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

            md_app_scheduled_redraw(pApp, 0, 0, sizeX, sizeY);
        } break;

        case WM_MOUSEMOVE:
        {
            if (pApp->onMouseMove) {
                md_int32 newPosX = MD_GET_X_LPARAM(lParam);
                md_int32 newPosY = MD_GET_Y_LPARAM(lParam);
                pApp->onMouseMove(pApp, newPosX, newPosY);
            }
        } break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            if (pApp->onMouseButtonDown) {
                md_int32 x = MD_GET_X_LPARAM(lParam);
                md_int32 y = MD_GET_Y_LPARAM(lParam);
                pApp->onMouseButtonDown(pApp, x, y, md_wm_event_to_mouse_button__win32(msg));
            }
        } break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            if (pApp->onMouseButtonUp) {
                md_int32 x = MD_GET_X_LPARAM(lParam);
                md_int32 y = MD_GET_Y_LPARAM(lParam);
                pApp->onMouseButtonUp(pApp, x, y, md_wm_event_to_mouse_button__win32(msg));
            }
        } break;

        case WM_PAINT:
        {
            md_gc_config gcConfig;
            md_gc gc;

            MD_ZERO_OBJECT(&gcConfig);

        #if defined(GLBIND_H)
            if (pApp->pGL == NULL) {
        #endif
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
        #if defined(GLBIND_H)
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
        #endif
        } break;

        default: break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

HWND md_app_create_HWND(md_app_config* pConfig, md_app* pApp)
{
    WNDCLASSEXA wc;
    HWND hWnd;

    MD_ZERO_OBJECT(&wc);
    wc.cbSize        = sizeof(wc);
    wc.cbWndExtra    = sizeof(pApp);
    wc.lpfnWndProc   = (WNDPROC)md_app_MainWindowProc;
    wc.lpszClassName = "md_app";
    wc.hCursor       = LoadCursorA(NULL, MAKEINTRESOURCEA(32512));
    wc.hIcon         = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(101));
    wc.style         = CS_OWNDC | CS_DBLCLKS;
    if (!RegisterClassExA(&wc)) {
        return NULL;
    }

    hWnd = CreateWindowExA(0, "md_app", (pConfig->pWindowTitle != NULL) ? pConfig->pWindowTitle : "Test App", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, pConfig->windowWidth, pConfig->windowHeight, NULL, NULL, NULL, NULL);
    if (hWnd == NULL) {
        return NULL;
    }

    SetWindowLongPtrA(hWnd, 0, (LONG_PTR)pApp);   /* HWND user data. */

#if defined(GLBIND_H)
    /* May want to enable the window for OpenGL. */
    if (pConfig->pGL != NULL) {
        SetPixelFormat(GetDC(hWnd), glbGetPixelFormat(), glbGetPFD());
        pConfig->pGL->wglMakeCurrent(GetDC(hWnd), glbGetRC());
    }
#endif

    ShowWindow(hWnd, SW_SHOWNORMAL);

    return hWnd;
}
#elif defined(MD_APPLE)
/* Implement me. */
#else
#define MD_TESTAPP_GTK_PROPERTY_NAME_USERDATA   "mt.app.userdata"

static gboolean md_window__on_close__gtk(GtkWidget* pWidget, GdkEvent* pEvent, gpointer pUserData)
{
    (void)pWidget;
    (void)pEvent;

    md_app* pApp = (md_app*)pUserData;
    if (pApp == NULL) {
        return MD_TRUE;
    }

    gtk_main_quit();
    
    return MD_TRUE;
}

static gboolean md_window__on_configure__gtk(GtkWidget* pWidget, GdkEventConfigure* pEvent, gpointer pUserData)
{
    md_app* pApp = (md_app*)pUserData;
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

    md_app* pApp = (md_app*)pUserData;
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

GtkWidget* md_app_create_window__gtk(md_app_config* pConfig, md_app* pApp)
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

md_result md_app_init(md_app_config* pConfig, md_app* pApp)
{
    md_result result;

    if (pApp == NULL || pConfig == NULL) {
        return MD_INVALID_ARGS;
    }

    MD_ZERO_OBJECT(pApp);
#if defined(GLBIND_H)
    pApp->pGL = pConfig->pGL;
#endif
    pApp->onInit            = pConfig->onInit;
    pApp->onUninit          = pConfig->onUninit;
    pApp->onSize            = pConfig->onSize;
    pApp->onPaint           = pConfig->onPaint;
    pApp->onMouseMove       = pConfig->onMouseMove;
    pApp->onMouseButtonDown = pConfig->onMouseButtonDown;
    pApp->onMouseButtonUp   = pConfig->onMouseButtonUp;
    pApp->pUserData         = pConfig->pUserData;

    /* API */
    result = md_init(&pConfig->apiConfig, &pApp->mt);
    if (result != MD_SUCCESS) {
        return result;
    }

    /* Window */
#if defined(MD_WIN32)
    pApp->hWnd = md_app_create_HWND(pConfig, pApp);
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

            pApp->pWindowWidget = md_app_create_window__gtk(pConfig, pApp);
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

void md_app_uninit(md_app* pApp)
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

int md_app_run(md_app* pApp)
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

void md_app_get_size(md_app* pApp, md_uint32* pSizeX, md_uint32* pSizeY)
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

void md_app_scheduled_redraw(md_app* pApp, md_int32 left, md_int32 top, md_int32 right, md_int32 bottom)
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

void md_app_scheduled_redraw_whole(md_app* pApp)
{
    md_uint32 sizeX;
    md_uint32 sizeY;
    md_app_get_size(pApp, &sizeX, &sizeY);
    md_app_scheduled_redraw(pApp, 0, 0, (md_int32)sizeX, (md_int32)sizeY);
}
#endif  /* MINIDRAW_IMPLEMENTATION */