#ifndef build_graph_head
#define build_graph_head

#define web_page_dir_path "/news.sohu.com" //网页文件存放根目录

#define link_mask_num 7 //网站前缀掩码

#define dir_sp "/" //文件夹分隔符格式

#define char_len sizeof(char)

#define int_len sizeof(int)

typedef struct file_node //代表网页文件的数据结点
{
    char *path; //此网页的路径
    int id;
    struct file_node *next; //队列中下一个结点
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