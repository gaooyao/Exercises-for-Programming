#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"

/*
typedef struct Lines
{
    int L;              //出现的行数
    struct Lines *next; //下一个行号
} Lines;
*/

static Link *root;
/*
void initList(Link *L)
{
    *InitLink(Link * L);
}

Lines *InitLines()
{
    Lines *head = (Lines *)malloc(sizeof(Lines));
    int L = -1; //初始化为-1；
    Lines *temp = head;
}
*/

//Link *InitLink()
void init()
{
    root = (Link *)malloc(sizeof(Link));
    root->next = NULL;
    root->str[0] = 0;
    root->num = 0;
    root->line_num[0] = 0;
}

void insert(char *str, int line_num)
{
    Link *temp = root;
    while (temp)
    {
        //传入的str已存在
        if (!strcmp(str, temp->str))
        {
            temp->num++;
            if (temp->num < 4)
            {
                temp->line_num[temp->num - 1] = line_num;
            }
            break; //找到了相应的str,不再继续往下遍历
        }
        if (temp->next)
        {
            temp = temp->next;
        }
        else
        {
            //当传入的字符串尚未出现在链表中，则生成新的节点，因为temp已经不存在
            Link *p = (Link *)malloc(sizeof(Link));
            strcpy(p->str, str);
            p->next = NULL;
            p->num = 1;
            if (p->num < 4)
            {
                p->line_num[p->num - 1] = line_num; //
            };
            temp->next = p;
            //temp = temp->next;
            return;
        }
    }
}

void output()
{
    FILE *file_handler;//指向文件的指针
    file_handler = fopen("result.txt","w");//

    Link *temp = root->next;

    while(temp){
        fprintf(file_handler,"%s %d",temp->str, temp->num);
        int n;
        for (n = 0; n < temp->num&&n<3;n++)
        {
            fprintf(file_handler," %d",temp->line_num[n]);
            
        }
        fprintf(file_handler,"\n");
        temp = temp->next;
    }
    fclose(file_handler);
    
    while (temp)
    {
        printf("%s %d", temp->str, temp->num);
        int n = 0;
        while (n < 3)
        {
            printf("%d", temp->line_num[n]);
        }
        temp = temp->next;
    }
}
