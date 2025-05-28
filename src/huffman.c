#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "huffman.h"

#define MAX_TREE_HT 256

typedef struct HuffmanNode {
    unsigned char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    HuffmanNode **array;
} MinHeap;

// Create a new node
HuffmanNode* newNode(unsigned char data, unsigned freq) {
    HuffmanNode* temp = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    temp->data = data;
    temp->freq = freq;
    temp->left = temp->right = NULL;
    return temp;
}

// Create a min heap of given capacity
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode**)malloc(minHeap->capacity * sizeof(HuffmanNode*));
    return minHeap;
}

void swapHuffmanNode(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* t = *a;
    *a = *b;
    *b = t;
}

// Heapify at given index
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < (int)minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < (int)minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
    if (smallest != idx) {
        swapHuffmanNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Check if size is one
int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Extract min node
HuffmanNode* extractMin(MinHeap* minHeap) {
    HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);
    return temp;
}

// Insert a new node
void insertMinHeap(MinHeap* minHeap, HuffmanNode* node) {
    minHeap->size++;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

// Build min heap
void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; i--)
        minHeapify(minHeap, i);
}

// Create and build min heap from frequencies
MinHeap* createAndBuildMinHeap(unsigned freq[]) {
    int capacity = 0;
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            capacity++;
    MinHeap* minHeap = createMinHeap(capacity);

    int idx = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            minHeap->array[idx++] = newNode((unsigned char)i, freq[i]);
            minHeap->size++;
        }
    }
    buildMinHeap(minHeap);
    return minHeap;
}

// Build Huffman Tree and return root
HuffmanNode* buildHuffmanTree(unsigned freq[]) {
    HuffmanNode *left, *right, *top;

    MinHeap* minHeap = createAndBuildMinHeap(freq);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    HuffmanNode* root = extractMin(minHeap);
    free(minHeap->array);
    free(minHeap);
    return root;
}

// Array to store codes
char codes[256][MAX_TREE_HT];
char codeArr[MAX_TREE_HT];

// Generate codes from Huffman tree
void generateCodes(HuffmanNode* root, int top) {
    if (root->left) {
        codeArr[top] = '0';
        generateCodes(root->left, top + 1);
    }
    if (root->right) {
        codeArr[top] = '1';
        generateCodes(root->right, top + 1);
    }
    if (!root->left && !root->right) {
        codeArr[top] = '\0';
        strcpy(codes[root->data], codeArr);
    }
}

// Huffman encode input bytes into a binary string of '0' and '1'
char* huffman_encode(const unsigned char *input, size_t len) {
    unsigned freq[256] = {0};
    for (size_t i = 0; i < len; i++)
        freq[input[i]]++;

    HuffmanNode* root = buildHuffmanTree(freq);
    generateCodes(root, 0);

    // Estimate output size, allocate buffer
    size_t bufsize = len * MAX_TREE_HT;
    char *output = malloc(bufsize);
    if (!output) {
        fprintf(stderr, "⚠️ Memory allocation failed in huffman_encode\n");
        return NULL;
    }
    output[0] = '\0';

    // Build the encoded string
    for (size_t i = 0; i < len; i++)
        strcat(output, codes[input[i]]);

    // Free Huffman Tree
    freeTree(root);

    return output;
}

// Helper function to free nodes
void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}
