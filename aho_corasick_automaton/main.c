#include "string_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

clock_t start, end;
int main()
{
    char *str = NULL;
    //打开pattern文件
    FileHandler *file_pattern;
    file_pattern = open_file("string.txt", "rb");
    file_pattern->end_flag_len = 2;
    start = clock();
    int kk = 0;
    long sum = 0;
    while (read_line(file_pattern, &str))
    {
        sum += strlen(str);
        kk++;
    }
    end = clock();
    close_file(file_pattern);
    printf("runtime: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    
    return 0;
}
