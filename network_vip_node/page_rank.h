#ifndef page_rank_head
#define page_rank_head

//网页结点
typedef struct page_node
{
    char *path;            //网页路径
    int out_link_num;      //出链数目
    float out_link_weight; //出链的权重
    int *out_link_list;    //出链列表
} PageNode;

void page_rank(); //计算网页rank主函数，未优化版本

#endif