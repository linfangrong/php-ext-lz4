#ifndef __LZ4_H__
#define __LZ4_H__

/* from lz4.c */

typedef enum {
    LZ4_OK = 0,
    LZ4_INVALID_INPUT = 1,
    LZ4_BUFFER_TOO_SMALL = 2,
    LZ4_MALLOC_MEMORY_ERROR = 3,
} lz4_status;

long lz4_max_compressed_length(long source_length);
lz4_status lz4_compress(const char* source_str, long source_length, char* dest_str, long *dest_length);
lz4_status lz4_uncompressed_length(const char* source_str, long source_length, long *dest_length);
lz4_status lz4_uncompress(const char* source_str, long source_length, char* dest_str, long *dest_length);


#endif
