#ifndef	node_struct
#define node_struct

typedef struct HashNode {
	char str[100];
	struct HashNode* next;
}HashNode;


#endif
