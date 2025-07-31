// main.c
// A complete Huffman Coding implementation in C for file compression.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

// --- Data Structures ---

// Represents a node in the Huffman Tree
struct MinHeapNode {
    unsigned char data; // The character
    unsigned freq;      // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right children
};

// Represents a Min-Heap (used as a priority queue)
struct MinHeap {
    unsigned size;     // Current size of min heap
    unsigned capacity; // Capacity of min heap
    struct MinHeapNode** array; // Array of min heap node pointers
};

// --- Min-Heap Helper Functions ---

// Creates a new min heap node
struct MinHeapNode* newNode(unsigned char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Creates a new min heap
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Swaps two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Heapify function to maintain heap property
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Checks if the heap size is 1
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Extracts the minimum frequency node from the heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Inserts a new node to the min heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Builds the min heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// --- Huffman Tree Core Functions ---

// Builds the Huffman Tree and returns the root
struct MinHeapNode* buildHuffmanTree(unsigned char data[], unsigned freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // '$' is a special value for internal nodes, not used
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Stores codes in an array
void storeCodes(struct MinHeapNode* root, int arr[], int top, char** huffmanCodes) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, huffmanCodes);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, huffmanCodes);
    }
    if (!root->left && !root->right) { // If this is a leaf node
        huffmanCodes[root->data] = (char*)malloc(top + 1);
        for (int i = 0; i < top; ++i) {
            huffmanCodes[root->data][i] = arr[i] + '0';
        }
        huffmanCodes[root->data][top] = '\0';
    }
}

// --- File I/O and Compression/Decompression Logic ---

// Wrapper function to generate Huffman Codes
void generateCodes(unsigned char data[], unsigned freq[], int size, char** huffmanCodes) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    storeCodes(root, arr, top, huffmanCodes);
}

// Compresses the file
void compressFile(const char* inputPath, const char* outputPath) {
    // 1. Calculate frequency of each character
    unsigned freq[256] = {0};
    FILE* inputFile = fopen(inputPath, "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return;
    }

    unsigned char c;
    long long originalSize = 0;
    while (fread(&c, 1, 1, inputFile)) {
        freq[c]++;
        originalSize++;
    }
    fclose(inputFile);

    // 2. Build Huffman Codes
    unsigned char data[256];
    unsigned char_freq[256];
    int size = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            data[size] = (unsigned char)i;
            char_freq[size] = freq[i];
            size++;
        }
    }

    char* huffmanCodes[256] = {0};
    generateCodes(data, char_freq, size, huffmanCodes);

    // 3. Write header and compressed data
    FILE* outputFile = fopen(outputPath, "wb");
    if (!outputFile) {
        perror("Error opening output file");
        return;
    }

    // Write original file size (for decompression)
    fwrite(&originalSize, sizeof(originalSize), 1, outputFile);
    // Write frequency table
    fwrite(freq, sizeof(unsigned), 256, outputFile);

    inputFile = fopen(inputPath, "rb");
    unsigned char buffer = 0;
    int bit_count = 0;

    while (fread(&c, 1, 1, inputFile)) {
        char* code = huffmanCodes[c];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer <<= 1;
            if (code[i] == '1') {
                buffer |= 1;
            }
            bit_count++;
            if (bit_count == 8) {
                fwrite(&buffer, 1, 1, outputFile);
                buffer = 0;
                bit_count = 0;
            }
        }
    }

    // Write any remaining bits
    while (bit_count > 0 && bit_count < 8) {
        buffer <<= 1;
        bit_count++;
    }
    if (bit_count == 8) {
       fwrite(&buffer, 1, 1, outputFile);
    }


    fclose(inputFile);
    fclose(outputFile);
    printf("File compressed successfully!\n");

    // Free allocated memory for codes
    for (int i = 0; i < 256; i++) {
        if (huffmanCodes[i]) {
            free(huffmanCodes[i]);
        }
    }
}

// Decompresses the file
void decompressFile(const char* inputPath, const char* outputPath) {
    // 1. Read header and rebuild Huffman Tree
    FILE* inputFile = fopen(inputPath, "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return;
    }

    long long originalSize;
    unsigned freq[256];
    fread(&originalSize, sizeof(originalSize), 1, inputFile);
    fread(freq, sizeof(unsigned), 256, inputFile);

    unsigned char data[256];
    unsigned char_freq[256];
    int size = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            data[size] = (unsigned char)i;
            char_freq[size] = freq[i];
            size++;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(data, char_freq, size);

    // 2. Read compressed data and decompress
    FILE* outputFile = fopen(outputPath, "wb");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    struct MinHeapNode* current = root;
    unsigned char buffer;
    long long decodedCount = 0;

    while (decodedCount < originalSize && fread(&buffer, 1, 1, inputFile)) {
        for (int i = 7; i >= 0; i--) {
            if (decodedCount >= originalSize) break;

            int bit = (buffer >> i) & 1;
            if (bit == 1) {
                current = current->right;
            } else {
                current = current->left;
            }

            if (!current->left && !current->right) {
                fwrite(&current->data, 1, 1, outputFile);
                decodedCount++;
                current = root;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    printf("File decompressed successfully!\n");
}


// --- Main Driver ---
int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [c|d] [input_file] [output_file]\n", argv[0]);
        fprintf(stderr, "  c - compress\n");
        fprintf(stderr, "  d - decompress\n");
        return 1;
    }

    char mode = argv[1][0];
    const char* inputFile = argv[2];
    const char* outputFile = argv[3];

    if (mode == 'c') {
        compressFile(inputFile, outputFile);
    } else if (mode == 'd') {
        decompressFile(inputFile, outputFile);
    } else {
        fprintf(stderr, "Invalid mode '%c'. Use 'c' for compress or 'd' for decompress.\n", mode);
        return 1;
    }

    return 0;
}