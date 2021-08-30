/* Include this file after the implementation of minidraw.h */
#include "../extras/minidraw_app.h"

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
#if defined(_MSC_VER) && _MSC_VER >= 1400
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (filePath == NULL || openMode == NULL || ppFile == NULL) {
        return MD_INVALID_ARGS;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
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
