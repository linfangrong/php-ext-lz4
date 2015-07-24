# php-ext-lz4 #

This PHP extension is LZ4.

## Build ##

    % phpize
    % ./configure
    % make
    % make install

## Configration ##

lz4.ini:

    extension=lz4.so

## Function : lz4_compress ##

string lz4_compress( string $data )

### parameters ###

data:

    The data to compress.

### return values ###

The compressed string or FALSE if an error occurred.

## Function : lz4_uncompress ##

string lz4_uncompress( string $data )

### parameters ###

name:

    The data compressed by snappy_gzcompress(). 

### return values ###

The original uncompressed data or FALSE on error.

## Example ##

    $compressed = lz4_compress('Compress me');

    $uncompressed = lz4_uncompress($compressed);

    echo $uncompressed;

