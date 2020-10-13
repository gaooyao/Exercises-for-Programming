#include "search_radix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* 函数名称：create_radix_node
* 函数功能：创建radix树节点并初始化
* 参数：	str：节点内的字符串
* 返回值：  新创建的节点
*/
Radix_Node *create_radix_node(char *str)
{
    Radix_Node *node = (Radix_Node *)malloc(sizeof(Radix_Node));
    node->child = NULL;
    node->brother = NULL;
    node->len = 0;
    node->is_end = 0;
    node->str = str;
    return node;
}

/*
* 函数名称：get_bit
* 函数功能：求字符串某一bit位的值为0还是1
* 参数：	str：被查询的字符串
            index： 查询第index位
* 返回值：  根据字符串第index位的值返回0或1
*/
unsigned short get_bit(char *str, unsigned short index)
{
    return ((unsigned char)((unsigned char)(str[index / 8]) << (index % 8)) >> 7);
}

/*
* 函数名称：set_bit
* 函数功能：设置字符串某一bit位的值（0或1）
* 参数：	str：被修改的字符串
            index： 第index位被修改
            bit：第index位被修改为0还是1
* 返回值：  无
*/
void set_bit(char *str, unsigned short index, unsigned short bit)
{
    if (bit)
    {
        unsigned char c = 1;
        c = c << (7 - (index % 8));
        str[index / 8] = str[index / 8] | c;
    }
    else
    {
        unsigned char c = 1;
        c = c << (7 - (index % 8));
        str[index / 8] = str[index / 8] & (~c);
    }
}

/*
* 函数名称：create_new_str
* 函数功能：根据传入的字符串，开始位置，结束位置，生产新的str并返回
* 参数：	str：模板字符串
            start_p：新的str从传入的str的start_p Bit位开始复制
            end_p：新的str从传入的str的end_p Bit位结束复制
* 返回值：  新的str
*/
char *create_new_str(char *str, unsigned short start_p, unsigned short end_p)
{
    int k = end_p - start_p;
    int l = (k) / 8;
    if ((k % 8))
    {
        l++;
    }
    char *s = (char *)malloc(l);
    int j;
    for (j = 0; j < k; j++)
    {
        set_bit(s, j, get_bit(str, start_p + j));
    }
    return s;
}

Radix_Node *root_l, *root_r, *point;

/*
* 函数名称：radix_init_tree
* 函数功能：初始化radix树
*/
void radix_init_tree()
{
    char temp = 'r';
    root_l = create_radix_node(&temp);
    root_l->len = 65535;
    root_r = create_radix_node(&temp);
    root_r->len = 65535;
}

/*
* 函数名称：radix_insert_recoder
* 函数功能：插入字符串到radix树中
* 参数：	str：被插入的字符串
* 返回值：  int，1为成功，0为失败
*/
int radix_insert_recoder(char *str)
{

    if (!str[0]) //字符串为空则直接返回
    {
        return 0;
    }
    //因为首bit有0或1两种情况，所以分两棵树处理，此处判断使用0开头树还是1开头树
    if ((unsigned char)str[0] >> 7)
    {
        point = root_r;
    }
    else
    {
        point = root_l;
    }
    //若还未插入过字符串则直接给树根赋值
    if (point->len == 65535)
    {
        int i = strlen(str);
        int j;
        point->str = (char *)malloc(sizeof(str));
        for (j = 0; j < i; j++)
        {
            point->str[j] = str[j];
        }
        point->is_end = 1;
        point->len = i * 8;
        return 1;
    }
    //若已插入过字符串则需要判断
    int i;
    unsigned short node_p = 0; //指针，指向节点内被遍历到的bit的位置，如node_p为9表示遍历到节点内字符串第2个字节的第1bit
    int len = strlen(str) * 8; //一个需要遍历的比特数
    /*
    整体思路为以bit为单位把插入的字符串遍历一遍，同时从根节点搜索，分一下几种情况：
        1.搜到某节点某bit时与当前遍历的字符串的bit对不上，则分裂此节点生成兄弟节点
        2.某节点被全部遍历，但字符串未遍历完，则在此节点child中寻找能继续遍历的节点（child首bit与当前遍历字符串bit相同）,若找到则开始从此child节点继续遍历，未找到则插入新节点
        3.某节点未被全部遍历，但字符串已遍历完，则分裂此节点，前半部分的is_end置1
        4.某节点被全部遍历，字符串恰好也被全部遍历，则此节点is_end置1
    */
    for (i = 0; i < len; i++) //从字符串首bit开始遍历
    {
        if (node_p > point->len - 1) //判断当前节点是否被遍历完
        {
            //若被遍历完，则为情况2
            int flag = 0; //用来记录状态，值为0为当前节点无子节点，则新节点作为其child插入，值为1为有子节点，则新节点做为子节点的brother插入，值为2则为找到了可以继续遍历的子节点
            Radix_Node *old_point = point;
            Radix_Node *old_point_b = point;
            point = point->child;
            while (point) //开始遍历子节点，寻找能被继续遍历的节点
            {
                //在子节点中寻找可继续遍历的节点
                if (get_bit(point->str, 0) == get_bit(str, i))
                {
                    //若找到，则开始遍历此节点，指针，计数器等复位
                    node_p = 0;
                    i--;
                    flag = 2;
                    break;
                }
                old_point_b = point;
                point = point->brother;
                flag = 1;
            }
            if (flag != 2) //flag为2说明未找到能继续遍历的节点，需要插入新节点
            {
                //创建新节点
                char *new_str = create_new_str(str, i, strlen(str) * 8);
                Radix_Node *temp = create_radix_node(new_str);
                temp->len = len - i;
                temp->is_end = 1;
                //根据flag判断插入位置并插入新节点
                if (flag)
                {
                    old_point_b->brother = temp;
                }
                else
                {
                    old_point->child = temp;
                }
                point = temp;
            }
        }
        else
        {
            //当前节点未被遍历完，对比两个指针所指Bit是否相同
            if (get_bit(point->str, node_p) != get_bit(str, i))
            {
                //若两个指针所指Bit不相同则为情况1，需要分裂当前节点并插入新节点
                Radix_Node *node_l = create_radix_node(' '); //分裂出的新节点，作为当前节点的child
                node_l->len = point->len - node_p;           //新节点内的有效bit长度为当前节点str值从node_p开始到总长度，即当前节点内值的后半段
                node_l->str = create_new_str(point->str, node_p, point->len);

                point->str = create_new_str(point->str, 0, node_p); //更新当前节点内的数据，其值为原值的前半段（后半段在新节点中）
                point->len = node_p;
                point->is_end = 0;

                node_l->child = point->child; //更新分裂出的节点的数据
                node_l->is_end = 1;
                point->child = node_l;
                point = node_l;

                //插入的字符串内还未被遍历的剩余Bit作为新的节点插入当前节点的child队列中（即分裂出的节点的brother）
                char *c_str = create_new_str(str, i, strlen(str) * 8);
                Radix_Node *node_c = create_radix_node(c_str);
                node_c->len = len - i;
                node_c->is_end = 1;
                point->brother = node_c;
                point = node_c;

                return 1;
            }
            else
            {
                //若两个指针所指Bit相同则继续向后对比
                node_p++;
            }
        }
    }
    //执行到此处说明字符串已被遍历完，有情况3，4两种
    if (point->len != node_p) //若遍历节点的指针未指向节点末尾，则说明情况3发生
    {
        //情况3，需要分裂当前节点，前半段的is_end置1，以表明有字符串到此结尾，出现过此字符串

        //创建新节点并赋值
        char *c_str = create_new_str(point->str, node_p, point->len);
        Radix_Node *node_c = create_radix_node(c_str);
        node_c->is_end = 0;
        node_c->len = point->len - node_p;
        node_c->child = point->child;
        //更新原节点的数据
        point->str = create_new_str(point->str, 0, node_p);
        point->len = node_p;
        point->is_end = 1;
        point->child = node_c;
        return 1;
    }
    //执行到此处说明为情况4，则is_end置1，以表明有字符串到此结尾，出现过此字符串
    point->is_end = 1;
    return 1;
}

/*
* 函数名称：radix_query_recoder
* 函数功能：查询字符串是否出现过
* 参数：	str：被查询的字符串
* 返回值：  int，1为出现过，0为未出现过
*/
int radix_query_recoder(char *str)
{
    int i;
    int len = strlen(str) * 8; //需要遍历多少bit
    //因为首bit有0或1两种情况，所以分两棵树处理，此处判断使用0开头树还是1开头树
    if ((unsigned char)str[0] >> 7)
    {
        point = root_r;
    }
    else
    {
        point = root_l;
    }
    unsigned short node_p = 0; //node_p为指针，指向节点内被遍历到的bit的位置
    //从0bit开始遍历
    for (i = 0; i < len; i++)
    {
        if (node_p > point->len - 1)
        {
            //若当前节点被遍历完，则在其子节点中寻找能继续遍历的节点
            point = point->child;
            int m = 1; //标志是否找到可继续遍历的子节点
            while (point)
            {
                if (get_bit(point->str, 0) == get_bit(str, i))
                {
                    //找到能继续遍历的节点，把其设置为被遍历节点，同时指针，计数器等复位
                    node_p = 0;
                    i--;
                    m = 0;
                    break;
                }
                point = point->brother;
            }
            if (m) //若m为1则表明未找到可继续遍历的节点，则返回false
            {
                return 0;
            }
        }
        else //若当前节点未被遍历完，则对比节点内指针与字符串内指针指向的bit，若不同说明树内未找到此字符串
        {
            if (get_bit(point->str, node_p) != get_bit(str, i))
            {
                //指向的bit不同
                return 0;
            }
            else
            {
                //指向的bit相同，指针向前步进，继续对比下一bit
                node_p++;
            }
        }
    }
    //若字符串被遍历完且节点恰好也被遍历完且节点is_end为1，说明此字符串出现过，返回true
    if (point->len == node_p && point->is_end)
    {
        return 1;
    }
    //若不是上述情况则返回false
    return 0;
}
void radix_destroy_tree()
{
    printf("destroy d\n");
}
