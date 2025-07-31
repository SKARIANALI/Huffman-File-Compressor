
# Huffman File Compressor

A simple and efficient command-line tool written in C for compressing and decompressing files using the Huffman coding algorithm. This project demonstrates the implementation of priority queues (min-heaps), binary trees, and bit-level file I/O for lossless data compression.

***

## Features 🚀

* **Compress**: Reduces the size of any file by encoding it into a compact binary format (`.huf`).
* **Decompress**: Perfectly restores the original file from its compressed version.
* **Lossless**: The decompressed file is a bit-for-bit identical match to the original.
* **Portable**: Written in standard C with no external dependencies, making it easy to compile and run on any system with a C compiler.

***

## How It Works

The program uses the Huffman coding algorithm, which assigns variable-length binary codes to characters based on their frequencies in the source file.

1.  **Frequency Analysis**: The compressor first reads the input file to count the occurrences of each character (0-255).
2.  **Tree Construction**: A binary tree (the Huffman Tree) is built using a min-heap. Characters with lower frequencies are placed deeper in the tree, while more frequent characters are placed closer to the root.
3.  **Code Generation**: By traversing the tree, a unique prefix-free binary code is generated for each character (e.g., 'a' might become `01`, 'b' might become `110`).
4.  **Encoding**: The compressor writes the frequency table (as a header) to the output file, followed by the data, where each character is replaced by its new binary code.
5.  **Decoding**: The decompressor reads the header to rebuild the exact same Huffman Tree, then reads the compressed data bit-by-bit to traverse the tree and decode the original characters.

***

## How to Use 🛠️

### Prerequisites

You need a C compiler, such as **GCC**, installed on your system.

### Compilation

Clone the repository and compile the source code using the following command:

```bash
gcc main.c -o huffman -mconsole
