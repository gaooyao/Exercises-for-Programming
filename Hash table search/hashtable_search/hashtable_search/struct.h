#ifndef	node_struct
#define node_struct

typedef struct HashNode {
	char str[100];
	struct HashNode* next;
}HashNode;

typedef struct FileHandler {
	FILE* file;
	char file_name[99];
	int open_type;
	int point;
	char buffer[1048576];
	struct FileHandler* next;
}FileHandler;

#endif
