#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz4.c"

int main() {
    char *source_str = "1111111111111111111111111111111111";
    long source_str_len = strlen(source_str);

    printf("%s %d\n", source_str, source_str_len);

    long dest_str_len = lz4_max_compressed_length(source_str_len);
    printf("%d\n", dest_str_len);

    char *dest_str = (char *)malloc(dest_str_len * sizeof(char));

    lz4_compress(source_str, source_str_len, dest_str, &dest_str_len);

    return 0;
}
