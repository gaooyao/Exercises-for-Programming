#ifndef build_graph_head
#define build_graph_head


typedef struct file_node{
    char *path;
    struct file_node *next;
} FileNode;

//状态机的节点
typedef struct ac_tree_node
{
    char str;
    unsigned short is_end; //此节点是否对应某字符串结束
    struct ac_tree_node *father;   //父节点
    struct ac_tree_node *child;    //子节点
    struct ac_tree_node *brother;  //兄弟节点
    struct ac_tree_node *turn;     //失效节点
    int file_id;
} AcTreeNode;


void build_graph();

#endif