#ifndef ac_tree_head
#define ac_tree_head

//辅助存储字符串出现次数等信息
typedef struct out
{
    char *str;        //对应的字符串
    unsigned int num; //出现次数
    int line_1;       //第一次出现的行号
    int line_2;       //第二次出现的行号
    int line_3;       //第三次出现的行号
} Out;

//状态机的节点
typedef struct node
{
    char str;
    unsigned short is_end; //此节点是否对应某字符串结束
    struct node *father;   //父节点
    struct node *child;    //子节点
    struct node *brother;  //兄弟节点
    struct node *turn;     //失效节点
    struct out *out;       //指向此节点对应的输出信息
} Node;

void init_tree();                            //初始化状态机
void insert_recoder(char *str);              //状态机插入字符串
void query_recoder(char *str, int line_num); //查询字符串，line_num为行号，用来输出到输出文件的行号
void make_turn();                            //给整个树构建失效指针
void out_to_result();                        //整理输出到文件

#endif
