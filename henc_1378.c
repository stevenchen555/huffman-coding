// Li Jung Chen cs610 1378 prp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_TREE_HT 256
#define MAX_BUFFER_SIZE 256
#define MAX_CODE_LENGTH 256

unsigned long bits_num_compressed = 0;
unsigned char buffer[MAX_BUFFER_SIZE];
int bits_in_buffer = 0;
struct node_t *nodes;

struct MinHeapNode {
	unsigned char data;
	unsigned freq;
	struct MinHeapNode *left, *right;
};

struct MinHeap {
	unsigned size;
	unsigned capacity;
	struct MinHeapNode **array;
};

struct node_t {
	int bit_num;
	int stack[MAX_CODE_LENGTH];
};

struct MinHeapNode* newNode(unsigned char data, unsigned freq)
{
	struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
	temp -> left = temp -> right = NULL;
	temp -> data = data;
	temp -> freq = freq;
	return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) 
{
	struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap -> size = 0; 
	minHeap->capacity = capacity;
	minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a,struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx)
{
	int smallest = idx;
	int left = 2 * idx + 1; 
	int right = 2 * idx + 2;

	if (left < minHeap->size && minHeap->array[left]-> freq < minHeap->array[smallest]->freq) 
		smallest = left; 
		  
	if (right < minHeap->size && minHeap->array[right]-> freq < minHeap->array[smallest]->freq) 
		smallest = right; 
			    
	if (smallest != idx) {
		swapMinHeapNode(&minHeap->array[smallest],&minHeap->array[idx]); 
		minHeapify(minHeap, smallest); 
									             } 
}

int isSizeOne(struct MinHeap* minHeap) 
{ 
      return (minHeap->size == 1); 
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) 
{ 
	struct MinHeapNode* temp = minHeap->array[0]; 
	minHeap->array[0] = minHeap->array[minHeap->size - 1]; 
	--minHeap->size; 
	minHeapify(minHeap, 0); 
	return temp; 
}

void insertMinHeap(struct MinHeap* minHeap,struct MinHeapNode* minHeapNode) 
{ 
	++minHeap->size; 
	int i = minHeap->size - 1; 
	while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) { 
		minHeap->array[i] = minHeap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	minHeap->array[i] = minHeapNode; 				
}

void buildMinHeap(struct MinHeap* minHeap) 
{ 
	int n = minHeap->size - 1; 
	int i; 
	for (i = (n - 1) / 2; i >= 0; --i) 
		minHeapify(minHeap, i); 
}

void printArr(int arr[], int n) 
{ 
    int i; 
    for (i = 0; i < n; ++i) 
	printf("%d", arr[i]); 
		  
    printf("\n"); 
}

int isLeaf(struct MinHeapNode* root) 
{ 
	return !(root->left) && !(root->right); 
} 

struct MinHeap* createAndBuildMinHeap(unsigned char data[],unsigned int freq[], int size) 
{
	int i = 0;
	struct MinHeap* minHeap = createMinHeap(size); 
	for (i = 0; i < size; ++i) 
		minHeap->array[i] = newNode(data[i], freq[i]); 
		        
	minHeap->size = size; 
	buildMinHeap(minHeap); 
	return minHeap; 
}

struct MinHeapNode* buildHuffmanTree(unsigned char data[],unsigned int freq[], int size)
{
	struct MinHeapNode *left, *right, *top;
	struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);
	while (!isSizeOne(minHeap)) {
		left = extractMin(minHeap);
		right = extractMin(minHeap);
		top = newNode('$', left->freq + right->freq);
		top->left = left;
		top->right = right;
		insertMinHeap(minHeap, top);
	}
	return extractMin(minHeap);
}

void printCodes(struct MinHeapNode* root, int arr[], int top)
{
	if (root->left) {
		arr[top] = 0;
		printCodes(root->left, arr, top + 1);
	}
	if (root->right) { 
	        arr[top] = 1; 
		printCodes(root->right, arr, top + 1); 
	}
	if (isLeaf(root)) { 
	        printf("%c: ", root->data); 
		printArr(arr, top);
	}
}

void CreateTable(struct MinHeapNode* root, int arr[], int top)
{
	int i=0;
	if (root->left) {
		arr[top] = 0;
		CreateTable(root->left, arr, top + 1);
	}
	if (root->right) { 
	        arr[top] = 1; 
		CreateTable(root->right, arr, top + 1); 
	}
	if (isLeaf(root)) { 
		nodes[(int)(root->data)].bit_num = top;
		for (i=0;i<top;i++){
			nodes[(int)(root->data)].stack[i] = arr[i];
		}
	}
}

int write_bit(FILE *f, int bit)
{
	if (bits_in_buffer == MAX_BUFFER_SIZE << 3) {
		size_t bytes_written = fwrite(buffer, 1, MAX_BUFFER_SIZE, f);		     if (bytes_written < MAX_BUFFER_SIZE && ferror(f))
			return -1;
		bits_in_buffer = 0;
		memset(buffer, 0, MAX_BUFFER_SIZE);
	}
	if (bit)
		buffer[bits_in_buffer >> 3] |= (0x1 << (7 - bits_in_buffer % 8));
	++bits_in_buffer;
	return 0;
}

int flush_buffer(FILE *f) {
	if (bits_in_buffer) {
		size_t bytes_written = fwrite(buffer, 1,(bits_in_buffer + 7) >> 3, f);
		if (bytes_written < MAX_BUFFER_SIZE && ferror(f))
			return -1;
		bits_in_buffer = 0;
	}
	return 0;
}

void encode_alphabet(FILE *fout, int c)
{
	int i=0;
	for(i=0;i<nodes[c].bit_num;i++){
		write_bit(fout, nodes[c].stack[i]);
		bits_num_compressed++;
	}
}

void write_header(struct MinHeapNode *root, FILE *f)
{
	int i=0;
	if(isLeaf(root)){
		write_bit(f,1);
		for(i=7;i>=0;i--){
			write_bit(f,((root->data)>>i)& 1);
		}		
	}else{
		write_bit(f,0);
		write_header(root->left, f);
		write_header(root->right, f);
	}
}

void write_size(unsigned int count,FILE *f)
{
	unsigned char bytes[4];
	int i=0,j=0;
	bytes[0] = (count >> 24) & 0xff;
	bytes[1] = (count >> 16) & 0xff;
	bytes[2] = (count >> 8) & 0xff;
	bytes[3] = (count) & 0xff;
	for(i=0;i<4;i++){
		for(j=7;j>=0;j--){
			write_bit(f,((bytes[i])>>j)& 1);
		}
	}
}

int main(int argc, char **argv) 
{ 
      int n = 256;
      int c;
      int i, status, num=0, cur=0;
      unsigned int compressed_file_size = 0;
      FILE *fin ,*fout;
      unsigned int *frequency = NULL;
      unsigned char *arr = NULL;
      unsigned int *freq = NULL;
      unsigned int byte_count=0;
      char output_file[MAX_BUFFER_SIZE];
      if ((fin = fopen(argv[1], "rb")) == NULL) {
      		perror("Failed to open input file");
		return -1;
      }
      strcpy(output_file, argv[1]);
      strcat(output_file,".huf");
      if ((fout = fopen(output_file, "wb")) == NULL) {
      		perror("Failed to open output file");
		fclose(fin);
		return -1;
      }

      frequency = (int *)calloc(n, sizeof(int));
      nodes = (struct node_t *)calloc(n, sizeof(struct node_t));

      while ((c = fgetc(fin)) != EOF) {
      		if(!frequency[c]) num++;
		frequency[c]++;
		byte_count++;
      }
      arr = (unsigned char *)calloc(num, sizeof(unsigned char));
      freq = (unsigned int *)calloc(num, sizeof(unsigned int));

      for (i = 0; i < n; i++){
		if(frequency[i]){
			arr[cur] = (unsigned char)i;
			freq[cur] = frequency[i];
			cur++;
		}
      }
      int size = num;

      struct MinHeapNode* root = buildHuffmanTree(arr, freq, size); 
      int arrc[MAX_TREE_HT], top = 0; 
      //printCodes(root, arrc, top); 
      CreateTable(root, arrc, top);
      //printf("byte_count is %d\n",byte_count);
      write_size(byte_count,fout);
      write_header(root,fout);
      fseek(fin, 0, SEEK_SET);
      while ((c = fgetc(fin)) != EOF)
      	encode_alphabet(fout, c);
      flush_buffer(fout);
      fseek(fout, 0, SEEK_END);
      compressed_file_size = ftell(fout);
      printf("number of bytes of the compressed file: %d\n",compressed_file_size);
      printf("total number of bits for encoding the original file: %ld\n",bits_num_compressed);

      free(nodes);
      free(frequency);
      free(freq);
      free(arr);
      status = remove(argv[1]);	
      if(status) printf("delete file %s fails\n",argv[1]);
      fclose(fin);
      fclose(fout);

      return 0;
}





