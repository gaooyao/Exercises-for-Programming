#ifndef search_mtrie
#define search_mtrie

#define M 4

typedef struct m_tree_node
{
    char is_end;
    struct m_tree_node *child[1 << M];
} MTNode;

typedef struct ex_str
{
    char *str;
    struct ex_str *next;
} EXStr;

void mtrie_init_tree();
int mtrie_insert_recoder(char *str);
int mtrie_query_recoder(char *str);
void mtrie_destroy_tree();

#endif