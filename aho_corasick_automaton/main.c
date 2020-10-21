#include "string_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ac_tree.h"

clock_t start, end;
int main()
{
    char *str = NULL;
    //打开pattern文件
    FileHandler *file_pattern;
    file_pattern = open_file("pattern.txt", "rb");
    start = clock();
    int kk = 0;
    init_tree();
    while (read_line(file_pattern, &str))
    {
        kk++;
        if(kk%100000==0)
            printf("kk=%d\n",kk);
        insert_recoder(str);
       //printf("%s\n", str);
    }
    end = clock();
    close_file(file_pattern);
    printf("runtime: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    
    return 0;
}
