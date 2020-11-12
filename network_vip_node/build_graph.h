#ifndef build_graph_head
#define build_graph_head

typedef struct file_id //内存中网页的出链存放采用队列格式，此为队列结点
{
    unsigned int file_id; //出链指向的网页的id
    struct file_id *next; //下一个出链结点
} FileId;

typedef struct file_node //代表网页文件的数据结点
{
    char *path;                     //此网页的路径
    struct file_id *out_file_tail;  //存储此网页出链信息的队列的队头
    struct file_id *out_file_front; //存储此网页出链信息的队列的队尾
    struct file_node *next;         //队列中下一个结点
} FileNode;

//状态机的节点，本次实验借助构建ac状态机搜索出链信息
typedef struct ac_tree_node
{
    char str;
    unsigned short is_end;        //此节点是否对应某字符串结束
    struct ac_tree_node *father;  //父节点
    struct ac_tree_node *child;   //子节点
    struct ac_tree_node *brother; //兄弟节点
    struct ac_tree_node *turn;    //失效节点
    int file_id;                  //此节点对应的FileNode在file_index_list数组中的位置
} AcTreeNode;

void build_graph(); //遍历网页文件夹，构建graph.bin文件

#endif