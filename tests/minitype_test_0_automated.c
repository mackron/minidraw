#define MINITYPE_IMPLEMENTATION
#include "../minitype.h"
#include "minitype_testcommon.c"

int print_error(const char* format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vprintf(format, args);
    va_end(args);

    return result;
}

/*
Things to test
==============
- Normal, valid conversions
- When the input UTF-8 and UTF-16 strings do not contain enough code units to convert to a whole UTF-32 code point.
  - Example: A UTF-8 code point that's missing the last bytes of a code point. Should return MT_INVALID_ARGS.
- When the output buffer does not have enough room for the conversion. Should return MT_OUT_OF_MEMORY.
- Check invalid character replacements. When MT_ERROR_ON_INVALID_CODE_POINT is _not_ set.
- Check that MT_INVALID_CODE_POINT is returned when MT_ERROR_ON_INVALID_CODE_POINT is set.
- Check that the _length() functions are exactly consistent with the normal conversion APIs with the same flags and whatnot.
- Check that the output lengths are correct, including when the output buffer is too small.
- Check that UTF-16 and UTF-32 BOMs are handled properly. Conversion routines
*/

int test__utf8_to_utf16le_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf8* testvectorInput;
    size_t testvectorInputSize;
    mt_utf16* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf16 actualOutputUTF16[6];
    size_t actualOutputLen;
    size_t utf8LenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf8("test_vectors/mt_utf8_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf16le_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf16("test_vectors/mt_utf16le_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf8_to_utf16le_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf8_to_utf16le(actualOutputUTF16, MT_COUNTOF(actualOutputUTF16), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf8LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf16le_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf16le_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf8LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf16le_0: UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf8LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF16[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf8_to_utf16le_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf8_to_utf16le_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf16le_0: mt_utf8_to_utf16le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf8_to_utf16le_0: mt_utf8_to_utf16le_length() != mt_utf8_to_utf16le().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF16, sizeof(actualOutputUTF16));
    actualOutputLen = -1;
    utf8LenProcessed = -1;

    result = mt_utf8_to_utf16le(actualOutputUTF16, MT_COUNTOF(actualOutputUTF16), &actualOutputLen, testvectorInput, (size_t)-1, &utf8LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf16le_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf16le_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf8LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf16le_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf8LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF16[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf8_to_utf16le_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf8_to_utf16le_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf16le_0: mt_utf8_to_utf16le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf8_to_utf16le_0: mt_utf8_to_utf16le_length() != mt_utf8_to_utf16le().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}



int test__utf8_to_utf32le_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf8* testvectorInput;
    size_t testvectorInputSize;
    mt_utf32* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf32 actualOutputUTF32[5];
    size_t actualOutputLen;
    size_t utf8LenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf8("test_vectors/mt_utf8_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf32le_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf32("test_vectors/mt_utf32le_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf8_to_utf32le_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf8_to_utf32le(actualOutputUTF32, MT_COUNTOF(actualOutputUTF32), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf8LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf32le_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf32le_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf8LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf32le_0: UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf8LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF32[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf8_to_utf32le_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf8_to_utf32le_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf32le_0: mt_utf8_to_utf32le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf8_to_utf32le_0: mt_utf8_to_utf32le_length() != mt_utf8_to_utf32le().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF32, sizeof(actualOutputUTF32));
    actualOutputLen = -1;
    utf8LenProcessed = -1;

    result = mt_utf8_to_utf32le(actualOutputUTF32, MT_COUNTOF(actualOutputUTF32), &actualOutputLen, testvectorInput, (size_t)-1, &utf8LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf32le_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf32le_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf8LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf8_to_utf32le_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf8LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF32[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf8_to_utf32le_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf8_to_utf32le_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf8_to_utf32le_0: mt_utf8_to_utf16le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf8_to_utf32le_0: mt_utf8_to_utf32le_length() != mt_utf8_to_utf32le().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}



int test__utf16le_to_utf8_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf16* testvectorInput;
    size_t testvectorInputSize;
    mt_utf8* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf8 actualOutputUTF8[11];
    size_t actualOutputLen;
    size_t utf16LenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf16("test_vectors/mt_utf16le_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf8_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf8("test_vectors/mt_utf8_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf16le_to_utf8_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf16le_to_utf8(actualOutputUTF8, MT_COUNTOF(actualOutputUTF8), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf16LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf8_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf8_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf16LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf8_0: UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf16LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF8[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf16le_to_utf8_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf16le_to_utf8_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf8_0: mt_utf16le_to_utf8_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf16le_to_utf8_0: mt_utf16le_to_utf8_0_length() != mt_utf16le_to_utf8().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF8, sizeof(actualOutputUTF8));
    actualOutputLen = -1;
    utf16LenProcessed = -1;

    result = mt_utf16le_to_utf8(actualOutputUTF8, MT_COUNTOF(actualOutputUTF8), &actualOutputLen, testvectorInput, (size_t)-1, &utf16LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf8_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf8_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf16LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf8_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf16LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF8[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf16le_to_utf8_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf16le_to_utf8_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf8_0: mt_utf16le_to_utf8_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf16le_to_utf8_0: mt_utf16le_to_utf8_length() != mt_utf16le_to_utf8().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}



int test__utf16le_to_utf32le_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf16* testvectorInput;
    size_t testvectorInputSize;
    mt_utf32* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf32 actualOutputUTF32[5];
    size_t actualOutputLen;
    size_t utf16LenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf16("test_vectors/mt_utf16le_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf32le_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf32("test_vectors/mt_utf32le_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf16le_to_utf32le_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf16le_to_utf32le(actualOutputUTF32, MT_COUNTOF(actualOutputUTF32), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf16LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf32le_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf32le_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf16LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf32le_0: UTF-8 processed length does not equal the test vector input length. utf32leLenProcessed=%d, testvectorInputLen=%d\n", (int)utf16LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF32[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf16le_to_utf32le_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf16le_to_utf32le_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf32le_0: mt_utf16le_to_utf32le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf16le_to_utf32le_0: mt_utf16le_to_utf32le_0_length() != mt_utf16le_to_utf32le().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF32, sizeof(actualOutputUTF32));
    actualOutputLen = -1;
    utf16LenProcessed = -1;

    result = mt_utf16le_to_utf32le(actualOutputUTF32, MT_COUNTOF(actualOutputUTF32), &actualOutputLen, testvectorInput, (size_t)-1, &utf16LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf32le_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf32le_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf16LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf16le_to_utf32le_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf32leLenProcessed=%d, testvectorInputLen=%d\n", (int)utf16LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF32[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf16le_to_utf32le_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf16le_to_utf32le_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf16le_to_utf32le_0: mt_utf16le_to_utf32le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf16le_to_utf32le_0: mt_utf16le_to_utf32le_length() != mt_utf16le_to_utf32le().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}



int test__utf32le_to_utf8_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf32* testvectorInput;
    size_t testvectorInputSize;
    mt_utf8* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf8 actualOutputUTF8[11];
    size_t actualOutputLen;
    size_t utf32LenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf32("test_vectors/mt_utf32le_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf8_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf8("test_vectors/mt_utf8_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf32le_to_utf8_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf32le_to_utf8(actualOutputUTF8, MT_COUNTOF(actualOutputUTF8), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf32LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf8_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf8_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf32LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf8_0: UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf32LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF8[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf32le_to_utf8_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf32le_to_utf8_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf8_0: mt_utf32le_to_utf8_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf32le_to_utf8_0: mt_utf32le_to_utf8_0_length() != mt_utf32le_to_utf8().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF8, sizeof(actualOutputUTF8));
    actualOutputLen = -1;
    utf32LenProcessed = -1;

    result = mt_utf32le_to_utf8(actualOutputUTF8, MT_COUNTOF(actualOutputUTF8), &actualOutputLen, testvectorInput, (size_t)-1, &utf32LenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf8_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf8_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf32LenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf8_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf8LenProcessed=%d, testvectorInputLen=%d\n", (int)utf32LenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF8[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf32le_to_utf8_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf32le_to_utf8_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf8_0: mt_utf32le_to_utf8_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf32le_to_utf8_0: mt_utf32le_to_utf8_length() != mt_utf32le_to_utf8().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}


int test__utf32le_to_utf16le_0()
{
    mt_result result = MT_SUCCESS;
    mt_utf32* testvectorInput;
    size_t testvectorInputSize;
    mt_utf16* testvectorOutput;
    size_t testvectorOutputSize;
    mt_utf16 actualOutputUTF16[6];
    size_t actualOutputLen;
    size_t utf32leLenProcessed;
    size_t i;

    result = mt_open_and_read_file_utf32("test_vectors/mt_utf32le_0.bin", &testvectorInputSize, &testvectorInput);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf16le_0: Failed to load test vector input.\n");
        return result;
    }

    result = mt_open_and_read_file_utf16("test_vectors/mt_utf16le_0.bin", &testvectorOutputSize, &testvectorOutput);
    if (result != MT_SUCCESS) {
        MT_FREE(testvectorInput);
        print_error("test__utf32le_to_utf16le_0: Failed to load test vector output.\n");
        return result;
    }


    result = mt_utf32le_to_utf16le(actualOutputUTF16, MT_COUNTOF(actualOutputUTF16), &actualOutputLen, testvectorInput, testvectorInputSize-1, &utf32leLenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf16le_0: An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf16le_0: Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf32leLenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf16le_0: UTF-8 processed length does not equal the test vector input length. utf32leLenProcessed=%d, testvectorInputLen=%d\n", (int)utf32leLenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF16[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf32le_to_utf16le_0: Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf32le_to_utf16le_length(&actualOutputLen, testvectorInput, testvectorInputSize-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf16le_0: mt_utf32le_to_utf16le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf32le_to_utf16le_0: mt_utf32le_to_utf16le_length() != mt_utf32le_to_utf16le().\n");
        goto done;
    }



    /* Null terminated. */
    MT_ZERO_MEMORY(actualOutputUTF16, sizeof(actualOutputUTF16));
    actualOutputLen = -1;
    utf32leLenProcessed = -1;

    result = mt_utf32le_to_utf16le(actualOutputUTF16, MT_COUNTOF(actualOutputUTF16), &actualOutputLen, testvectorInput, (size_t)-1, &utf32leLenProcessed, 0);  /* Fixed length input string. */
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf16le_0 (null terminated): An error occurred when converting the string.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf16le_0 (null terminated): Output length does not equal the test vector output length. actualOutputLen=%d, testvectorOutputLen=%d\n", (int)actualOutputLen, (int)testvectorOutputSize-1);
        goto done;
    }

    if (utf32leLenProcessed != testvectorInputSize-1) {
        result = MT_ERROR;
        print_error("test__utf32le_to_utf16le_0 (null terminated): UTF-8 processed length does not equal the test vector input length. utf32leLenProcessed=%d, testvectorInputLen=%d\n", (int)utf32leLenProcessed, (int)testvectorInputSize-1);
        goto done;
    }
    
    for (i = 0; i < actualOutputLen+1; ++i) {   /* <-- +1 to ensure the null terminator is inserted properly. */
        if (actualOutputUTF16[i] != testvectorOutput[i]) {
            result = MT_ERROR;
            print_error("test__utf32le_to_utf16le_0 (null terminated): Test vector output != actual output\n");
            goto done;
        }
    }

    result = mt_utf32le_to_utf16le_length(&actualOutputLen, testvectorInput, (size_t)-1, 0);
    if (result != MT_SUCCESS) {
        print_error("test__utf32le_to_utf16le_0: mt_utf32le_to_utf16le_length() failed.\n");
        goto done;
    }

    if (actualOutputLen != testvectorOutputSize-1) {
        print_error("test__utf32le_to_utf16le_0: mt_utf32le_to_utf16le_length() != mt_utf32le_to_utf16le().\n");
        goto done;
    }

done:
    MT_FREE(testvectorInput);
    MT_FREE(testvectorOutput);
    return result;
}



int test__utf_conversion()
{
    int result = 0;

    /* UTF-8 -> UTF-16 */
    result = test__utf8_to_utf16le_0();
    if (result != 0) {
        return result;
    }

    /* UTF-8 -> UTF-32 */
    result = test__utf8_to_utf32le_0();
    if (result != 0) {
        return result;
    }

    /* UTF-16 -> UTF-8 */
    result = test__utf16le_to_utf8_0();
    if (result != 0) {
        return result;
    }

    /* UTF-16 -> UTF-32 */
    result = test__utf16le_to_utf32le_0();
    if (result != 0) {
        return result;
    }

    /* UTF-32 -> UTF-8 */
    result = test__utf32le_to_utf8_0();
    if (result != 0) {
        return result;
    }

    /* UTF-32 -> UTF-16 */
    result = test__utf32le_to_utf16le_0();
    if (result != 0) {
        return result;
    }

    return 0;
}


int main(int argc, char** argv)
{
    /* UTF Conversion */
    test__utf_conversion();



    (void)argc;
    (void)argv;
    return 0;
}
