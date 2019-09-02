// Li Jung Chen cs610 1378 prp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_TREE_HT 256
#define MAX_BUFFER_SIZE 256
#define MAX_HEADER_SIZE 1024
#define MAX_CODE_LENGTH 256

unsigned char buffer[MAX_BUFFER_SIZE];
int bits_in_buffer = 0;
int current_bit = 0;
int eof_input = 0;

struct MinHeapNode {
	unsigned char data;
	unsigned freq;
	struct MinHeapNode *left, *right;
};

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

int read_bit(FILE *f) {
	if (current_bit == bits_in_buffer) {
		if (eof_input)
			return -1;
		else {
			size_t bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, f);
			if (bytes_read < MAX_BUFFER_SIZE) {
				if (feof(f))
					eof_input = 1;
			}
			bits_in_buffer = bytes_read << 3;
			current_bit = 0;
		}
	}
	if (bits_in_buffer == 0)
		return -1;
	int bit = (buffer[current_bit >> 3] >> (7 - current_bit % 8)) & 0x1;
	++current_bit;
	return bit;
}

unsigned char decode_alphabet(struct MinHeapNode *root,FILE *f)
{
	int bit;
	while(1){
		bit = read_bit(f);
		if(!bit) root = root->left;
		else root = root->right;
		if(isLeaf(root)) return root->data;	
	}
}

unsigned char read_byte(FILE *f)
{
	int i=0;
	int bit;
	unsigned char c = 0x00;
	for (i=7;i>=0;i--){
		bit = read_bit(f);
		if(bit) c |= (0x01 << i);
	}
	return c;
}

struct MinHeapNode* read_header(FILE *f)
{
	int bit;
	unsigned char c;
	bit = read_bit(f);
	if(bit == 1){
		c = read_byte(f);
		struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
		temp -> data = c;
		temp -> left = NULL;
		temp -> right = NULL;
		return temp;
	}else{
		struct MinHeapNode* leftchild = read_header(f);
		struct MinHeapNode* rightchild = read_header(f);
		struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
		temp -> data = '$';
		temp -> left = leftchild;
		temp -> right = rightchild;
		return temp;
	}
}

unsigned int read_size(FILE *f)
{
	int i=0,j=0;
	unsigned int byte_count;
	unsigned char bytes[4];
	for(i=0;i<4;i++){
		bytes[i] = read_byte(f);
	}
	byte_count = (int)((unsigned char)(bytes[0]) << 24 |
	            (unsigned char)(bytes[1]) << 16 |
		    (unsigned char)(bytes[2]) << 8 |
		    (unsigned char)(bytes[3]));
	return byte_count;
}

int main(int argc, char **argv) 
{ 
      int c, status;
      FILE *fin ,*fout;
      unsigned int original_size=0;
      char output_file[MAX_BUFFER_SIZE];
      char *pattern = ".huf";
      if ((fin = fopen(argv[1], "rb")) == NULL) {
      		perror("Failed to open input file");
		return -1;
      }
      char *pch = strstr(argv[1],pattern);
      if(!pch) printf("please feed file end with '.huf'\n");
      strncpy(output_file, argv[1], (pch - argv[1]));
      output_file[(pch - argv[1])] = '\0';

      if ((fout = fopen(output_file, "wb")) == NULL) {
      		perror("Failed to open output file");
		fclose(fin);
		return -1;
      }

      original_size = read_size(fin);
      struct MinHeapNode* root = read_header(fin);
      //int arrc[MAX_TREE_HT], top = 0; 
      //printCodes(root, arrc, top);
      while (original_size>0){
      	c = decode_alphabet(root,fin);
      	fputc(c,fout);
	original_size--;
      }
      status = remove(argv[1]);
      if(status) printf("delete file %s fails\n",argv[1]);
      fclose(fin);
      fclose(fout);

      return 0;
}





