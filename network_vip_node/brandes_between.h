#ifndef brandes_between_head
#define brandes_between_head

//网页结点
typedef struct page
{
    int link_num;   //连接的网页数目
    char *path;     //网页路径
    int *link_list; //连接的网页列表
} Page;

typedef struct path
{
    int node_num;   //连接的网页数目
    int *node_list; //连接的网页列表
    struct path *next;//下一条路径信息
} Path;

void brandes_between(); //主函数

#endif