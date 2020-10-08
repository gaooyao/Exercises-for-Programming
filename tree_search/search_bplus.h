#ifndef search_bplus
#define search_bplus

#define MAX_CHILD_NUMBER 1000

typedef struct BPlusTreeNode
{
    int isRoot, isLeaf;
    int key_num;
    int key[MAX_CHILD_NUMBER];
    struct BPlusTreeNode *child[MAX_CHILD_NUMBER];
    struct BPlusTreeNode *father;
    struct BPlusTreeNode *next;
    struct BPlusTreeNode *last;
} BPlusTreeNode;

extern void BPlusTree_SetMaxChildNumber(int);
extern void BPlusTree_Init();
extern void BPlusTree_Destroy();
extern int BPlusTree_Insert(int, void *);
extern int BPlusTree_GetTotalNodes();
extern void BPlusTree_Query_Key(int);
extern void BPlusTree_Query_Range(int, int);
extern void BPlusTree_Modify(int, void *);
extern void BPlusTree_Delete(int);

void bplus_init_tree();
int bplus_insert_recoder(char *str);
int bplus_query_recoder(char *str);
void bplus_destroy_tree();

#endif