/*
This is used for doing code generation for minidraw. Code is generated primarily based on the Unicode Character Database.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

/* Sized types. Prefer built-in types. Fall back to stdint. */
#include <stddef.h> /* For size_t. */
#ifdef _MSC_VER
    #if defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
    typedef   signed __int8  mdbuild_int8;
    typedef unsigned __int8  mdbuild_uint8;
    typedef   signed __int16 mdbuild_int16;
    typedef unsigned __int16 mdbuild_uint16;
    typedef   signed __int32 mdbuild_int32;
    typedef unsigned __int32 mdbuild_uint32;
    typedef   signed __int64 mdbuild_int64;
    typedef unsigned __int64 mdbuild_uint64;
    #if defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
#else
    #define MDBUILD_HAS_STDINT
    #include <stdint.h>
    typedef int8_t   mdbuild_int8;
    typedef uint8_t  mdbuild_uint8;
    typedef int16_t  mdbuild_int16;
    typedef uint16_t mdbuild_uint16;
    typedef int32_t  mdbuild_int32;
    typedef uint32_t mdbuild_uint32;
    typedef int64_t  mdbuild_int64;
    typedef uint64_t mdbuild_uint64;
#endif

#ifdef MDBUILD_HAS_STDINT
    typedef uintptr_t mdbuild_uintptr;
#else
    #if defined(_WIN32)
        #if defined(_WIN64)
            typedef mdbuild_uint64 mdbuild_uintptr;
        #else
            typedef mdbuild_uint32 mdbuild_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
            typedef mdbuild_uint64 mdbuild_uintptr;
        #else
            typedef mdbuild_uint32 mdbuild_uintptr;
        #endif
    #else
        typedef mdbuild_uint64 mdbuild_uintptr;   /* Fallback. */
    #endif
#endif
typedef mdbuild_uint8    mdbuild_bool8;
typedef mdbuild_uint32   mdbuild_bool32;
#define MDBUILD_TRUE     1
#define MDBUILD_FALSE    0

typedef mdbuild_uint32 mdbuild_utf32;

typedef int mdbuild_result;
#define MDBUILD_SUCCESS              0
#define MDBUILD_ERROR               -1
#define MDBUILD_INVALID_ARGS        -2
#define MDBUILD_INVALID_OPERATION   -3
#define MDBUILD_OUT_OF_MEMORY       -4

#define MDBUILD_UCD_PATH    "../../external/ucd"

#define MDBUILD_ZERO_OBJECT(p)   memset((p), 0, sizeof(*(p)))

mdbuild_result mdbuild_fopen(FILE** ppFile, const char* filePath, const char* openMode)
{
#if _MSC_VER
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (filePath == NULL || openMode == NULL || ppFile == NULL) {
        return MDBUILD_INVALID_ARGS;
    }

#if _MSC_VER
    err = fopen_s(ppFile, filePath, openMode);
    if (err != 0) {
        return MDBUILD_ERROR;
    }
#else
#if defined(_WIN32) || defined(__APPLE__)
    *ppFile = fopen(filePath, openMode);
#else
    *ppFile = fopen64(filePath, openMode);
#endif
    if (*ppFile == NULL) {
        return MDBUILD_ERROR;
    }
#endif

    return MDBUILD_SUCCESS;
}

mdbuild_result mdbuild_open_and_read_file(const char* filePath, size_t* pFileSizeOut, void** ppFileData)
{
    mdbuild_result result;
    mdbuild_uint64 fileSize;
    FILE* pFile;
    void* pFileData;
    size_t bytesRead;

    assert(filePath != NULL);

    result = mdbuild_fopen(&pFile, filePath, "rb");
    if (result != MDBUILD_SUCCESS) {
        return MDBUILD_ERROR;
    }

    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (fileSize > SIZE_MAX) {
        fclose(pFile);
        return MDBUILD_ERROR;
    }

    pFileData = malloc((size_t)fileSize);    /* <-- Safe cast due to the check above. */
    if (pFileData == NULL) {
        fclose(pFile);
        return MDBUILD_OUT_OF_MEMORY;
    }

    bytesRead = fread(pFileData, 1, (size_t)fileSize, pFile);
    if (bytesRead != fileSize) {
        free(pFileData);
        fclose(pFile);
        return MDBUILD_ERROR;
    }

    fclose(pFile);

    if (pFileSizeOut) {
        *pFileSizeOut = (size_t)fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        free(pFileData);
    }

    return MDBUILD_SUCCESS;
}

mdbuild_bool32 mdbuild_is_hex_digit(char ch)
{
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}

mdbuild_utf32 mdbuild_cp_str_to_uint(const char* str)
{
    return (mdbuild_utf32)strtoul(str, NULL, 16);
}

int mdbuild_itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
{
#ifdef _MSC_VER
    return _itoa_s(value, dst, dstSizeInBytes, radix);
#else
    if (dst == NULL || dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (radix < 2 || radix > 36) {
        dst[0] = '\0';
        return EINVAL;
    }

    int sign = (value < 0 && radix == 10) ? -1 : 1;     // The negative sign is only used when the base is 10.

    unsigned int valueU;
    if (value < 0) {
        valueU = -value;
    } else {
        valueU = value;
    }

    char* dstEnd = dst;
    do
    {
        int remainder = valueU % radix;
        if (remainder > 9) {
            *dstEnd = (char)((remainder - 10) + 'a');
        } else {
            *dstEnd = (char)(remainder + '0');
        }

        dstEnd += 1;
        dstSizeInBytes -= 1;
        valueU /= radix;
    } while (dstSizeInBytes > 0 && valueU > 0);

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  // Ran out of room in the output buffer.
    }

    if (sign < 0) {
        *dstEnd++ = '-';
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  // Ran out of room in the output buffer.
    }

    *dstEnd = '\0';


    // At this point the string will be reversed.
    dstEnd -= 1;
    while (dst < dstEnd) {
        char temp = *dst;
        *dst = *dstEnd;
        *dstEnd = temp;

        dst += 1;
        dstEnd -= 1;
    }

    return 0;
#endif
}


char* mdbuild_string_append(char* str, const char* other)
{
    size_t len0;
    size_t len1;

    if (str == NULL) {
        str = "";
    }

    len0 = strlen(str);
    len1 = strlen(other);
    
    char* newStr = (char*)malloc(len0 + len1 + 1);
    if (newStr == NULL) {
        return NULL;
    }

    strcpy_s(newStr,        len0 + len1 + 1, str);
    strcpy_s(newStr + len0,        len1 + 1, other);

    return newStr;
}

char* mdbuild_string_append_utf8_cp_hex(char* pCodeOut, char utf8)
{
    size_t i;
    char str[16];
    str[0] = '0';
    str[1] = 'x';
    mdbuild_itoa_s((mdbuild_uint8)utf8, str+2, sizeof(str)-2, 16);

    /* For aesthetics I want to use upper case characters. */
    for (i = 2; i < sizeof(str); ++i) {
        if (str[i] == '\0') {
            break;
        }

        str[i] = toupper(str[i]);
    }

    return mdbuild_string_append(pCodeOut, str);
}


typedef struct
{
    mdbuild_utf32 beg;
    mdbuild_utf32 end;
} mdbuild_codepoint_range;

typedef struct
{
    mdbuild_codepoint_range cp;
    const char* category;   /* An offset of mdbuild_ucd_proplist._pRawData. */
} mdbuild_ucd_proplist_item;

typedef struct
{
    mdbuild_ucd_proplist_item* pItems;
    size_t itemCount;
    char* _pRawData;
} mdbuild_ucd_proplist;

typedef struct
{
    mdbuild_ucd_proplist proplist;
} mdbuild_ucd;

mdbuild_result mdbuild_ucd_proplist_load(mdbuild_ucd_proplist* pPropList)
{
    mdbuild_result result;
    size_t fileSize;
    size_t iByte;
    size_t iItem;
    mdbuild_bool32 skippingLine = MDBUILD_FALSE;

    assert(pPropList != NULL);

    MDBUILD_ZERO_OBJECT(pPropList);

    result = mdbuild_open_and_read_file(MDBUILD_UCD_PATH"/PropList.txt", &fileSize, (void**)&pPropList->_pRawData);
    if (result != MDBUILD_SUCCESS) {
        return result;
    }

    /*
    Parse the file in two parses. The first parse counts the number of items. The second parse allocates a single block of memory and
    fills it with data.
    */
    for (iByte = 0; iByte < fileSize; ++iByte) {
        if (skippingLine) {
            if (pPropList->_pRawData[iByte] == '\n') {
                skippingLine = MDBUILD_FALSE;       /* Found the start of the end of the line. */
                continue;
            }
        } else {
            /* If the line starts with a hex digit we can treat it as a valid item. Otherwise we skip the line. */
            if (mdbuild_is_hex_digit(pPropList->_pRawData[iByte])) {
                pPropList->itemCount += 1;
                skippingLine = MDBUILD_TRUE;        /* Skip the rest of the line since we don't need to do anything more with it. */
            } else {
                if (pPropList->_pRawData[iByte] != '\n') {
                    skippingLine = MDBUILD_TRUE;   /* Not a hex digit so skip the line. */
                }
            }
        }
    }

    pPropList->pItems = (mdbuild_ucd_proplist_item*)malloc(sizeof(*pPropList->pItems) * pPropList->itemCount);
    if (pPropList->pItems == NULL) {
        return MDBUILD_OUT_OF_MEMORY;
    }

    /* This part fills the content of each item. */
    iItem = 0;
    for (iByte = 0; iByte < fileSize; ++iByte) {
        if (skippingLine) {
            if (pPropList->_pRawData[iByte] == '\n') {
                skippingLine = MDBUILD_FALSE;       /* Found the start of the end of the line. */
                continue;
            }
        } else {
            /* If the line starts with a hex digit we can treat it as a valid item. Otherwise we skip the line. */
            if (mdbuild_is_hex_digit(pPropList->_pRawData[iByte])) {
                /* Now we need to parse the code point range and category. We insert null terminators directly into the raw data. */
                char* cpBeg = NULL;
                char* cpEnd = NULL;
                char* category = NULL;

                /* First code point. */
                cpBeg = pPropList->_pRawData + iByte;
                cpEnd = cpBeg;
                iByte += 4; /* This file uses a minimum of 4 characters for code points. We can safely skip past these. */
                for (; iByte < fileSize; ++iByte) {
                    if (pPropList->_pRawData[iByte] == '.') {
                        /* We have a code point range. */
                        pPropList->_pRawData[iByte] = '\0';

                        /* Second code point. */
                        iByte += 2; /* Skip past the ".." */
                        cpEnd = pPropList->_pRawData + iByte;
                        iByte += 4; /* This file uses a minimum of 4 characters for code points. We can safely skip past these. */
                        for (; iByte < fileSize; ++iByte) {
                            if (pPropList->_pRawData[iByte] == ' ' || pPropList->_pRawData[iByte] == ';') {
                                pPropList->_pRawData[iByte] = '\0';
                                break;  /* Found the end of the second code point. */
                            }
                        }

                        break;
                    } else {
                        /* It's a single code point. */
                        pPropList->_pRawData[iByte] = '\0';
                        break;
                    }
                }

                /* Now we need to retrieve the category name. The first character that is not a space and not a semi-colon is the start of the category name. */
                iByte += 1;
                for (; iByte < fileSize; ++iByte) {
                    if (pPropList->_pRawData[iByte] != ' ' && pPropList->_pRawData[iByte] != ';') {
                        category = pPropList->_pRawData + iByte;
                        break;
                    }
                }

                iByte += 1;
                for (; iByte < fileSize; ++iByte) {
                    if (pPropList->_pRawData[iByte] == ' ' || pPropList->_pRawData[iByte] == '#' || pPropList->_pRawData[iByte] == '\r' || pPropList->_pRawData[iByte] == '\n') {
                        pPropList->_pRawData[iByte] = '\0';
                        break;
                    }
                }

                /* At this point we have our item. We can now convert our code points to integers and insert the item. */
                pPropList->pItems[iItem].cp.beg = mdbuild_cp_str_to_uint(cpBeg);
                pPropList->pItems[iItem].cp.end = mdbuild_cp_str_to_uint(cpEnd);
                pPropList->pItems[iItem].category = category;
                iItem += 1;

                if (pPropList->_pRawData[iByte] != '\n' && pPropList->_pRawData[iByte] != '\0') {
                    skippingLine = MDBUILD_TRUE;    /* Skip the rest of the line since we don't need to do anything more with it. */
                }
            } else {
                if (pPropList->_pRawData[iByte] != '\n') {
                    skippingLine = MDBUILD_TRUE;   /* Not a hex digit so skip the line. */
                }
            }
        }
    }

    return MDBUILD_SUCCESS;
}

void mdbuild_ucd_proplist_unload(mdbuild_ucd_proplist* pPropList)
{
    assert(pPropList != NULL);
    free(pPropList->pItems);
    free(pPropList->_pRawData);
}

mdbuild_result mdbuild_ucd_proplist_generate_code__is_null_or_whitespace_utf8(mdbuild_ucd_proplist* pPropList, char** ppCodeOut)
{
    char* pCodeOut = NULL;
    size_t iItem;
    mdbuild_utf32 cp;

    assert(pPropList != NULL);
    assert(ppCodeOut != NULL);

    *ppCodeOut = NULL;



    /* Naive implementation for now. This can be optimized. */
    for (iItem = 0; iItem < pPropList->itemCount; ++iItem) {
        mdbuild_ucd_proplist_item* pItem = &pPropList->pItems[iItem];
        if (strcmp(pItem->category, "White_Space") == 0) {
            const char* utf8LenStr = "";

            /* A quick little hack for sequential ASCII characters. This will be made generic later. */
            if (pItem->cp.beg <= 0x7F && pItem->cp.end <= 0x7F && pItem->cp.beg != pItem->cp.end) {
                utf8LenStr = "1";
                pCodeOut = mdbuild_string_append(pCodeOut, "        if ((md_uint8)pUTF8[0] >= ");
                pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, (char)pItem->cp.beg);
                pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[0] <= ");
                pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, (char)pItem->cp.end);

                pCodeOut = mdbuild_string_append(pCodeOut, ") {\n");
                pCodeOut = mdbuild_string_append(pCodeOut, "            pUTF8 += "); pCodeOut = mdbuild_string_append(pCodeOut, utf8LenStr); pCodeOut = mdbuild_string_append(pCodeOut, ";\n");
                pCodeOut = mdbuild_string_append(pCodeOut, "            continue;\n");
                pCodeOut = mdbuild_string_append(pCodeOut, "        }\n");
            } else {
                for (cp = pItem->cp.beg; cp <= pItem->cp.end; cp += 1) {
                    if (cp <= 0x7F) {
                        mdbuild_uint8 utf8_0 = (mdbuild_uint8)cp;
                        utf8LenStr = "1";

                        pCodeOut = mdbuild_string_append(pCodeOut, "        if ((md_uint8)pUTF8[0] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_0);
                    } else if (cp <= 0x7FF) {
                        mdbuild_uint8 utf8_0 = 0xC0 | ((cp & 0x07C0) >> 6);
                        mdbuild_uint8 utf8_1 = 0x80 |  (cp & 0x003F);
                        utf8LenStr = "2";

                        pCodeOut = mdbuild_string_append(pCodeOut, "        if ((md_uint8)pUTF8[0] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_0);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[1] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_1);
                    } else if (cp <= 0xFFFF) {
                        mdbuild_uint8 utf8_0 = 0xE0 | ((cp & 0xF000) >> 12);
                        mdbuild_uint8 utf8_1 = 0x80 | ((cp & 0x0FC0) >>  6);
                        mdbuild_uint8 utf8_2 = 0x80 |  (cp & 0x003F);
                        utf8LenStr = "3";

                        pCodeOut = mdbuild_string_append(pCodeOut, "        if ((md_uint8)pUTF8[0] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_0);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[1] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_1);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[2] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_2);
                    } else if (cp <= 0x10FFFF) {
                        mdbuild_uint8 utf8_0 = 0xF0 | ((cp & 0x1C0000) >> 18);
                        mdbuild_uint8 utf8_1 = 0x80 | ((cp & 0x03F000) >> 12);
                        mdbuild_uint8 utf8_2 = 0x80 | ((cp & 0x000FC0) >>  6);
                        mdbuild_uint8 utf8_3 = 0x80 |  (cp & 0x00003F);
                        utf8LenStr = "4";

                        pCodeOut = mdbuild_string_append(pCodeOut, "        if ((md_uint8)pUTF8[0] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_0);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[1] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_1);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[2] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_2);
                        pCodeOut = mdbuild_string_append(pCodeOut, " && (md_uint8)pUTF8[3] == ");
                        pCodeOut = mdbuild_string_append_utf8_cp_hex(pCodeOut, utf8_3);
                    }

                    pCodeOut = mdbuild_string_append(pCodeOut, ") {\n");
                    pCodeOut = mdbuild_string_append(pCodeOut, "            pUTF8 += "); pCodeOut = mdbuild_string_append(pCodeOut, utf8LenStr); pCodeOut = mdbuild_string_append(pCodeOut, ";\n");
                    pCodeOut = mdbuild_string_append(pCodeOut, "            continue;\n");
                    pCodeOut = mdbuild_string_append(pCodeOut, "        }\n");
                }
            }
        }
    }

    *ppCodeOut = pCodeOut;
    return MDBUILD_SUCCESS;
}


mdbuild_result mdbuild_ucd_load(mdbuild_ucd* pUCD)
{
    mdbuild_result result;

    assert(pUCD != NULL);

    result = mdbuild_ucd_proplist_load(&pUCD->proplist);
    if (result != MDBUILD_SUCCESS) {
        return result;
    }

    return MDBUILD_SUCCESS;
}

void mdbuild_ucd_unload(mdbuild_ucd* pUCD)
{
    assert(pUCD != NULL);
    mdbuild_ucd_proplist_unload(&pUCD->proplist);
}

mdbuild_result mdbuild_ucd_generate_code(mdbuild_ucd* pUCD)
{
    mdbuild_result result;
    char* pCodeGen_IsNullOrWhitespaceUTF8;

    assert(pUCD != NULL);

    result = mdbuild_ucd_proplist_generate_code__is_null_or_whitespace_utf8(&pUCD->proplist, &pCodeGen_IsNullOrWhitespaceUTF8);
    if (result == MDBUILD_SUCCESS) {
        return result;
    }

    printf("%s", pCodeGen_IsNullOrWhitespaceUTF8);


    free(pCodeGen_IsNullOrWhitespaceUTF8);
    return MDBUILD_SUCCESS;
}


int main(int argc, char** argv)
{
    mdbuild_result result;
    mdbuild_ucd ucd;

    result = mdbuild_ucd_load(&ucd);
    if (result != MDBUILD_SUCCESS) {
        return (int)result;
    }

    result = mdbuild_ucd_generate_code(&ucd);
    if (result != MDBUILD_SUCCESS) {
        mdbuild_ucd_unload(&ucd);
        return (int)result;
    }

    mdbuild_ucd_unload(&ucd);

    (void)argc;
    (void)argv;
    return 0;
}