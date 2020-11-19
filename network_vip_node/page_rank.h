#ifndef page_rank_head
#define page_rank_head

//网页结点
typedef struct page_node
{
    char *path;            //网页路径
    int out_link_num;      //出链数目
    float out_link_weight; //出链的权重
    int *out_link_list;    //出链列表
    int next_out_index;    //遍历时使用，下一个被遍历的出链在出链列表的索引
    int next_out_id;       //遍历时使用，下一个被遍历的出链对应的网页id
} PageNode;

//权重结点，在出链结点结构体中会用到
typedef struct page_weight
{
    int page_id;       //网页id
    float page_weight; //网页权重
} PageWeight;

//出链结点
typedef struct link_node
{
    int page_num;                  //有此出链的网页的数目
    int next_page_index;           //遍历时用到，下一个被遍历的权重结点在page_list中的位置
    struct page_weight *page_list; //有此出链的网页的数组
} LinkNode;

//page rank多线程计算时用到的传参结构体
typedef struct thread_arr
{
    float *source_rank; //旧rank数组
    float *dis_rank;    //新rank数组，计算结果要存进去
    int start_num;      //此线程从第几列开始计算
    int end_num;        //此线程到第几列结束计算
} ThreadArr;

void page_rank_v1(); //计算网页rank主函数，未优化版本
void page_rank_v2(); //计算网页rank主函数，优化后版本
void page_rank_v3(); //计算网页rank主函数，第二次优化后版本

#endif