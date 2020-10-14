#ifndef search_mtrie
#define search_mtrie

#define M 2 //M叉树的叉数

typedef struct m_tree_node //M叉树的节点
{
    char is_end;                       //是否有字符串以此节点结束
    struct m_tree_node *child[1 << M]; //M个子节点
} MTNode;

void mtrie_init_tree();              //初始化树
int mtrie_insert_recoder(char *str); //插入节点
int mtrie_query_recoder(char *str);  //查询节点
void mtrie_destroy_tree();           //释放树

#endif