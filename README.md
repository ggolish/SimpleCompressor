# Simple Compressor

A simple text compressor created for Systems Programming class.

## Usage:

Build executable with the following command:

```
make compress456
```

To compress a file, use:

```
compress456 -e <file>
```

To decompress, use:

```
compress456 -d <file-with-456-extension>
```

## Algorithms Used:

* [Burrows-Wheeler Transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform) - Reversible transform that creates runs of similar characters.
* [Move-to-Front Transform](https://en.wikipedia.org/wiki/Move-to-front_transform) - Reversible transform that reduces entropy in plaintext
* [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding) - The actual lossless compression step

## Author:

Written by **Gage Golish**.

## License:

This software is licensed under the terms of the [MIT License](https://github.com/ggolish/SimpleCompressor/blob/master/LICENSE.md).
