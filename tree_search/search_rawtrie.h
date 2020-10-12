#ifndef search_rawtrie
#define search_rawtrie
typedef struct TrieNode {
    struct TrieNode* child [256];
    int isEndOfWord;
    int count;  //子节点的数量
    char value;//该节点的字符
} TrieNode;
void rawtrie_init_tree();
int rawtrie_insert_recoder(char *str);
int rawtrie_query_recoder(char *str);
void rawtrie_destroy_tree();

#endif