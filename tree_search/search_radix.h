#ifndef search_radix
#define search_radix

//radix节点
typedef struct radix_node
{
    char *str;                  //节点内的二进制序列
    unsigned short len;         //节点内的二进制序列有效长度
    unsigned char is_end;       //以此节点为结尾的字符串是否出现过
    struct radix_node *child;   //节点的子节点
    struct radix_node *brother; //节点的兄弟节点
} Radix_Node;

void radix_init_tree();              //初始化radix树
int radix_insert_recoder(char *str); //插入记录
int radix_query_recoder(char *str);  //查询记录
void radix_destroy_tree();           //销毁书

#endif