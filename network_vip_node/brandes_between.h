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
    unsigned int node_num; //连接的网页数目
    int *node_list;        //连接的网页列表
    struct path *next;     //下一条路径信息
} Path;

//多线程计算介数时用到的传参结构体
typedef struct thread_arr
{
    int thread_id;                //此线程的编号
    int all_node_num;             //要计算的节点数目
    int start_num;                //此线程从第几列开始计算
    int end_num;                  //此线程到第几列结束计算
    unsigned long all_path_num;   //此线程遍历到的总路径数
    unsigned int *out_num;        //此线程遍历到的总路径数
    int *is_visited;              //从一个结点出发进行广度遍历，存储已访问节点列表
    int *new_visited;             //从一个结点出发进行广度遍历，存储新访问的节点列表
    unsigned int old_path_num;    //一轮遍历中，需要遍历的路径数
    unsigned int new_path_num;    //一轮遍历中，新增加的路径数
    struct path *path_point;      //一轮遍历中，记录指向当前便利的路径结构体的指针
    struct path *new_path;        //一轮遍历中，新创建路径的临时指针
    struct path *temp_path;       //一轮遍历中，临时指针
    struct path *path_queue_tail; //一轮遍历中，路径队列的队尾
    struct page *tail_page;       //一轮遍历中，访问页面结构体用到的临时指针
} ThreadArr;

void brandes_between(); //主函数

#endif