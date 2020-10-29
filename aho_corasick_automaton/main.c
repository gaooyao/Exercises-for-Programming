#include "string_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ac_tree.h"
#include "output.h"

clock_t start, end;
int main()
{
    char *str = NULL;
    //打开pattern文件，建树
    FileHandler *file_pattern;
    file_pattern = open_file("pattern.txt", "rb");
    int kk = 0;
    init_tree();
    init();
    start = clock();
    while (read_line(file_pattern, &str))
    {
        kk++;
        insert_recoder(str);
    }
    end = clock();
    printf("build tree finished: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    close_file(file_pattern);

    start = clock();
    make_turn();
    end = clock();
    printf("build fail point finished: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    



    //打开string文件，查找
    kk = 0;
    FileHandler *file_string;
    file_string = open_file("string.txt", "rb");
    file_string->end_flag_len = 2;
    start = clock();
    while (read_line(file_string, &str))
    {
        kk++;
        if(kk%10000==0){
            printf("%d\n",kk);

        }
        query_recoder(str,kk);
    }
    end = clock();
    printf("search finished: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    close_file(file_string);

    printf("runtime: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    output();
    getchar();
    getchar();
    return 0;
}
