#ifndef	node_struct
#define node_struct

typedef struct HashNode {
	char str[100];
	struct HashNode* next;
}HashNode;

/* 文件操作Handler,增加文件缓冲区加快读写速度 */
typedef struct FileHandler {
	FILE* file;		//被操作的文件指针
	char file_name[99];		//文件名
	int open_status;		//打开状态，为0时为关闭，1为read模式，2write模式
	int point;//指向在当前缓冲区中的读写位置
	unsigned int buffer_size;//文件缓冲区的大小
	char* buffer;//文件缓冲区
}FileHandler;

#endif
