#ifndef output_head
#define output_head
typedef struct Link
{
    char str[256];         //存储字符
    struct Link *next; //下一个节点
    int num;           //出现的总次数
    //struct Lines *line_num; //行数的头指针
    int line_num[3];
} Link;


//static Link *root;
//Link *InitLink();
void init();                          //初始化存储数据结构
void insert(char *str, int line_num); //数据结构中插入一行数据,str为要插入的内容，line_num为此字符串在pattern.txt中出现的行号
void output();                        //把数据结构中的数据保存到result.txt，具体格式看实验三介绍PDF里面的实验要求章节

#endif