// Forward declarations for HuffmanNode and MinHeap
typedef struct HuffmanNode HuffmanNode;
typedef struct MinHeap MinHeap;

char* huffman_encode(const unsigned char *input, size_t len);
void generateCodes(HuffmanNode* root, int top);
void freeTree(HuffmanNode* node);
HuffmanNode* buildHuffmanTree(unsigned freq[]);
MinHeap* createAndBuildMinHeap(unsigned freq[]);
void buildMinHeap(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, HuffmanNode* node);
HuffmanNode* extractMin(MinHeap* minHeap);
int isSizeOne(MinHeap* minHeap);
void minHeapify(MinHeap* minHeap, int idx);
void swapHuffmanNode(HuffmanNode** a, HuffmanNode** b);
MinHeap* createMinHeap(unsigned capacity);
HuffmanNode* newNode(unsigned char data, unsigned freq);