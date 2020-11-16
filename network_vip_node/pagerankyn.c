#include <stdio.h>
#include <stdlib.h>


void pagerank_func(){
    int i,j;
    char str[4096];
    char tem;
    FILE *file_handler = fopen("graph.bin", "rb");
    int page_num;//total number
    int max_num=0;
    int numt;
    int temp;
    
    fread(&page_num, sizeof(int), 1, file_handler);
    for (i = 0; i < page_num;i++){
        if(i==4670){
            printf("   ");
        }
        do{
            fread(&tem, sizeof(char), 1, file_handler);
            if(i==4670){
                printf("%c",tem);
            }
        } while (tem != 0);
        //fscanf(file_handler, "%s", str); //读出数组
        numt = 0;
        fread(&numt, sizeof(int), 1, file_handler);
        if (numt>max_num)
        {
            max_num = numt;
            printf("max_num=%d,i=%d\n",max_num,i);
        }
        for (j = 0; j < numt;j++)
        {
            fread(&temp, sizeof(int), 1, file_handler);
        }
    }
    for (i = 0; i < max_num;i++){

    }
        printf("page=%d\n", page_num);
        //int ELL[][];
}