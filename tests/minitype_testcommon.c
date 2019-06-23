/* Include this file after minitype.h */
#include <stdio.h>
#include <assert.h>

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcomment"  /* // comments are not allowed in this language [-Wcomment] */
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

mt_result mt_fopen(FILE** ppFile, const char* filePath, const char* openMode)
{
#if _MSC_VER
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (filePath == NULL || openMode == NULL || ppFile == NULL) {
        return MT_INVALID_ARGS;
    }

#if _MSC_VER
    err = fopen_s(ppFile, filePath, openMode);
    if (err != 0) {
        return mt_result_from_errno(err);
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
        mt_result result = mt_result_from_errno(errno);
        if (result == MT_SUCCESS) {
            return MT_ERROR;   /* Just a safety check to make sure we never ever return success when pFile == NULL. */
        }
    }
#endif

    return MT_SUCCESS;
}

mt_result mt_open_and_read_file(const char* filePath, size_t* pFileSizeOut, void** ppFileData)
{
    mt_result result;
    mt_uint64 fileSize;
    FILE* pFile;
    void* pFileData;
    size_t bytesRead;

    MT_ASSERT(filePath != NULL);

    result = mt_fopen(&pFile, filePath, "rb");
    if (result != MT_SUCCESS) {
        return MT_FAILED_TO_OPEN_FILE;
    }

    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (fileSize > SIZE_MAX) {
        fclose(pFile);
        return MT_TOO_BIG;
    }

    pFileData = MT_MALLOC((size_t)fileSize);    /* <-- Safe cast due to the check above. */
    if (pFileData == NULL) {
        fclose(pFile);
        return MT_OUT_OF_MEMORY;
    }

    bytesRead = fread(pFileData, 1, (size_t)fileSize, pFile);
    if (bytesRead != fileSize) {
        MT_FREE(pFileData);
        fclose(pFile);
        return MT_FAILED_TO_READ_FILE;
    }

    fclose(pFile);

    if (pFileSizeOut) {
        *pFileSizeOut = (size_t)fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        MT_FREE(pFileData);
    }

    return MT_SUCCESS;
}

mt_result mt_open_and_read_file_utf8(const char* filePath, size_t* pFileSizeOutInUTF8, mt_utf8** ppFileData)
{
    mt_result result = mt_open_and_read_file(filePath, pFileSizeOutInUTF8, (void**)ppFileData);
    *pFileSizeOutInUTF8 /= sizeof(mt_utf8);

    return result;
}

mt_result mt_open_and_read_file_utf16(const char* filePath, size_t* pFileSizeOutInUTF16, mt_utf16** ppFileData)
{
    mt_result result = mt_open_and_read_file(filePath, pFileSizeOutInUTF16, (void**)ppFileData);
    *pFileSizeOutInUTF16 /= sizeof(mt_utf16);

    return result;
}

mt_result mt_open_and_read_file_utf32(const char* filePath, size_t* pFileSizeOutInUTF32, mt_utf32** ppFileData)
{
    mt_result result = mt_open_and_read_file(filePath, pFileSizeOutInUTF32, (void**)ppFileData);
    *pFileSizeOutInUTF32 /= sizeof(mt_utf32);

    return result;
}



typedef struct mt_testapp mt_testapp;

typedef mt_result (* mt_testapp_on_init)  (mt_testapp* pApp);
typedef void      (* mt_testapp_on_uninit)(mt_testapp* pApp);
typedef void      (* mt_testapp_on_size)  (mt_testapp* pApp, mt_uint32 sizeX, mt_uint32 sizeY);
typedef void      (* mt_testapp_on_paint) (mt_testapp* pApp, mt_gc* pGC);

typedef struct
{
    const char* pWindowTitle;
    mt_uint32 windowWidth;
    mt_uint32 windowHeight;
    mt_testapp_on_init onInit;
    mt_testapp_on_uninit onUninit;
    mt_testapp_on_size onSize;
    mt_testapp_on_paint onPaint;
    void* pUserData;
} mt_testapp_config;

struct mt_testapp
{
    int argc;
    char** argv;
    mt_api mt;
    mt_testapp_on_init onInit;
    mt_testapp_on_uninit onUninit;
    mt_testapp_on_size onSize;
    mt_testapp_on_paint onPaint;
    void* pUserData;
#if defined(MT_WIN32)
    HWND hWnd;  /* Main window handle. */
#endif
};

#if defined(MT_WIN32)
LRESULT mt_testapp_MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    mt_result result;
    mt_testapp* pApp = (mt_testapp*)GetWindowLongPtrA(hWnd, 0);

    switch (msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        case WM_SIZE:
        {
            if (pApp->onSize) {
                mt_uint32 sizeX = LOWORD(lParam);
                mt_uint32 sizeY = HIWORD(lParam);
                pApp->onSize(pApp, sizeX, sizeY);
            }
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);
            if (hDC != NULL) {
                mt_gc_config gcConfig;
                mt_gc gc;

                MT_ZERO_OBJECT(&gcConfig);
                gcConfig.gdi.hDC = hDC;

                result = mt_gc_init(&pApp->mt, &gcConfig, &gc); /* <-- Make this transient! */
                if (result == MT_SUCCESS) {
                    if (pApp->onPaint) {
                        pApp->onPaint(pApp, &gc);
                    }
                    mt_gc_uninit(&gc);
                }
            }
            EndPaint(hWnd, &ps);
        } break;

        default: break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

HWND mt_testapp_create_HWND(mt_testapp_config* pConfig, mt_testapp* pApp)
{
    WNDCLASSEXA wc;
    HWND hWnd;

    MT_ZERO_OBJECT(&wc);
    wc.cbSize        = sizeof(wc);
    wc.cbWndExtra    = sizeof(pApp);
    wc.lpfnWndProc   = (WNDPROC)mt_testapp_MainWindowProc;
    wc.lpszClassName = "mt_testapp";
    wc.hCursor       = LoadCursorA(NULL, MAKEINTRESOURCEA(32512));
    wc.hIcon         = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(101));
    wc.style         = CS_OWNDC | CS_DBLCLKS;
    if (!RegisterClassExA(&wc)) {
        return NULL;
    }

    hWnd = CreateWindowExA(0, "mt_testapp", (pConfig->pWindowTitle != NULL) ? pConfig->pWindowTitle : "Test App", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, pConfig->windowWidth, pConfig->windowHeight, NULL, NULL, NULL, NULL);
    if (hWnd == NULL) {
        return NULL;
    }

    SetWindowLongPtrA(hWnd, 0, (LONG_PTR)pApp);   /* HWND user data. */

    ShowWindow(hWnd, SW_SHOWNORMAL);

    return hWnd;
}
#elif defined(MT_APPLE)
/* Implement me. */
#else

#endif

mt_result mt_testapp_init(mt_testapp_config* pConfig, mt_testapp* pApp)
{
    mt_result result;
    mt_api_config apiConfig;

    if (pApp == NULL || pConfig == NULL) {
        return MT_INVALID_ARGS;
    }

    MT_ZERO_OBJECT(pApp);
    pApp->onInit    = pConfig->onInit;
    pApp->onUninit  = pConfig->onUninit;
    pApp->onSize    = pConfig->onSize;
    pApp->onPaint   = pConfig->onPaint;
    pApp->pUserData = pConfig->pUserData;

    /* API */
    MT_ZERO_OBJECT(&apiConfig);
#if defined(MT_WIN32)
    apiConfig.backend = mt_backend_gdi;
#elif defined(MT_APPLE)
    apiConfig.backend = mt_backend_coregraphics;
#else
    apiConfig.backend = mt_backend_cairo;
#endif
    result = mt_init(&apiConfig, &pApp->mt);
    if (result != MT_SUCCESS) {
        return result;
    }

    /* Window */
#if defined(MT_WIN32)
    pApp->hWnd = mt_testapp_create_HWND(pConfig, pApp);
    if (pApp->hWnd == NULL) {
        mt_uninit(&pApp->mt);
        return MT_ERROR;
    }
#elif defined(MT_APPLE)
    /* Implement me. */
#else
    
#endif

    if (pApp->onInit) {
        result = pApp->onInit(pApp);
        if (result != MT_SUCCESS) {
        #if defined(MT_WIN32)
            DestroyWindow(pApp->hWnd);
        #elif defined(MT_APPLE)
        #else

        #endif
            mt_uninit(&pApp->mt);
            return result;
        }
    }

    return MT_SUCCESS;
}

void mt_testapp_uninit(mt_testapp* pApp)
{
    if (pApp == NULL) {
        return;
    }

    if (pApp->onUninit) {
        pApp->onUninit(pApp);
    }

#if defined(MT_WIN32)
    DestroyWindow(pApp->hWnd);
#elif defined(MT_APPLE)
#else

#endif

    mt_uninit(&pApp->mt);
}

int mt_testapp_run(mt_testapp* pApp)
{
    int exitCode = 0;

#if defined(MT_WIN32)
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
#elif defined(MT_APPLE)
#else

#endif

    return exitCode;
}

void mt_testapp_get_size(mt_testapp* pApp, mt_uint32* pSizeX, mt_uint32* pSizeY)
{
#if defined(MT_WIN32)
    RECT rect;
#endif

    MT_ASSERT(pApp != NULL);

    if (pSizeX) *pSizeX = 0;
    if (pSizeY) *pSizeY = 0;

#if defined(MT_WIN32)
    GetClientRect(pApp->hWnd, &rect);
    if (pSizeX) *pSizeX = (mt_uint32)(rect.right  - rect.left);
    if (pSizeY) *pSizeY = (mt_uint32)(rect.bottom - rect.top);
#elif defined(MT_APPLE)
    /* Implement me. */
#else
    /* Implement me. */
#endif
}

void mt_testapp_scheduled_redraw(mt_testapp* pApp, mt_int32 left, mt_int32 top, mt_int32 right, mt_int32 bottom)
{
#if defined(MT_WIN32)
    RECT rect;
#endif

    MT_ASSERT(pApp != NULL);

#if defined(MT_WIN32)
    rect.left   = left;
    rect.top    = top;
    rect.right  = right;
    rect.bottom = bottom;
    RedrawWindow(pApp->hWnd, &rect, NULL, RDW_INVALIDATE);
#elif defined(MT_APPLE)
    /* Implement me. */
#else
    /* Implement me. */
#endif
}
