#ifndef search_rawtrie
#define search_rawtrie

typedef struct TrieNode
{                                //rawrite树节点
    struct TrieNode *child[256]; //子节点数组
    unsigned char is_end;        //此节点是否为某字符串结尾
} TrieNode;

void rawtrie_init_tree();              //初始化树
int rawtrie_insert_recoder(char *str); //插入记录
int rawtrie_query_recoder(char *str);  //查询记录
void rawtrie_destroy_tree();           //释放树

#endif