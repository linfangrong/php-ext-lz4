#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz4.h"
#include "lz4lib/lz4.h"

enum {
    BLOCK_BYTES = 1024 * 8,         // limit LZ4_MAX_INPUT_SIZE
    BLOCK_PREFIX_LEN = sizeof(int),
};

long lz4_max_compressed_length(long source_length) {
    long block_num = source_length / BLOCK_BYTES;
    long block_remaining = source_length % BLOCK_BYTES;
    return block_num * (BLOCK_PREFIX_LEN+LZ4_compressBound(BLOCK_BYTES)) + 
        (block_remaining > 0 ? (BLOCK_PREFIX_LEN+LZ4_compressBound(block_remaining)) : 0);
}

lz4_status lz4_compress(const char* source_str, long source_length, char* dest_str, long *dest_length) {
    if (*dest_length < lz4_max_compressed_length(source_length)) {
        return LZ4_BUFFER_TOO_SMALL;
    }

    LZ4_stream_t lz4Stream_body;
    LZ4_stream_t* lz4Stream = &lz4Stream_body;
    int compressBufMaxLen = LZ4_compressBound(BLOCK_BYTES);
    char* compressBuf = NULL;
    long inpBytes = 0, outBytes = 0;
    int compressBufLen = 0, compressBytes = 0;

    LZ4_resetStream(lz4Stream);
    compressBuf = (char *)malloc(compressBufMaxLen * sizeof(char));
    if (!compressBuf) return LZ4_MALLOC_MEMORY_ERROR;

    for (;;) {
        memset(compressBuf, 0, compressBufMaxLen);

        compressBufLen = (source_length - inpBytes) > BLOCK_BYTES ? BLOCK_BYTES : (source_length - inpBytes);
        compressBytes = LZ4_compress_fast_continue(lz4Stream, source_str+inpBytes, compressBuf, compressBufLen, compressBufMaxLen, 1);
        if (compressBytes <= 0) break;

        memcpy(dest_str + outBytes, &compressBytes, BLOCK_PREFIX_LEN);
        memcpy(dest_str + outBytes + BLOCK_PREFIX_LEN, compressBuf, compressBytes);
        outBytes += (BLOCK_PREFIX_LEN+compressBytes);

        inpBytes += BLOCK_BYTES;
        if (inpBytes >= source_length) break;
    }
    free(compressBuf);
    *dest_length = outBytes;
    return LZ4_OK;
}

lz4_status lz4_uncompressed_length(const char* source_str, long source_length, long *dest_length) {
    int compressBytes = 0;
    long inpBytes = 0, outBytes = 0;
    
    for (;;) {
        memcpy(&compressBytes, source_str + inpBytes, BLOCK_PREFIX_LEN);
        if (compressBytes <= 0) return LZ4_INVALID_INPUT;
        outBytes += BLOCK_BYTES;

        inpBytes += (BLOCK_PREFIX_LEN+compressBytes);
        if (inpBytes >= source_length) break;
    }
    *dest_length = outBytes;
    return LZ4_OK;
}


lz4_status lz4_uncompress(const char* source_str, long source_length, char* dest_str, long *dest_length) {
    long uncompressed_length = 0;
    if (lz4_uncompressed_length(source_str, source_length, &uncompressed_length) != LZ4_OK) {
        return LZ4_INVALID_INPUT;
    }
    if (*dest_length < uncompressed_length) {
        return LZ4_BUFFER_TOO_SMALL;
    }

    LZ4_streamDecode_t lz4StreamDecode_body;
    LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;
    int compressBufMaxLen = LZ4_compressBound(BLOCK_BYTES);
    char* compressBuf = NULL;
    long inpBytes = 0, outBytes = 0;
    int compressBufLen = 0, compressBytes = 0;

    LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);
    compressBuf = (char *)malloc(compressBufMaxLen * sizeof(char));
    if (!compressBuf) return LZ4_MALLOC_MEMORY_ERROR;

    for (;;) {
        memset(compressBuf, 0, compressBufMaxLen);

        memcpy(&compressBytes, source_str + inpBytes, BLOCK_PREFIX_LEN);
        if (compressBytes <= 0 || compressBytes > compressBufMaxLen) goto failure;
        memcpy(compressBuf, source_str + inpBytes + BLOCK_PREFIX_LEN, compressBytes);
        compressBufLen = LZ4_decompress_safe_continue(lz4StreamDecode, compressBuf, dest_str+outBytes, compressBytes, BLOCK_BYTES);
        if (compressBufLen <= 0) goto failure;
        outBytes += compressBufLen;

        inpBytes += (BLOCK_PREFIX_LEN+compressBytes);
        if (inpBytes >= source_length) break;
    }
    free(compressBuf);
    *dest_length = outBytes;
    return LZ4_OK;
failure:
    free(compressBuf);
    return LZ4_INVALID_INPUT;
}
