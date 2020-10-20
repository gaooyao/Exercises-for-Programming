#include "string_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

clock_t start, end;
int main()
{
    char *str = NULL;
    //打开文件
    FileHandler *file_dict;
    file_dict = open_file("string.txt", "rb");
    start = clock();
    int kk = 0;
    while (read_line(file_dict, &str))
    {
        kk++;
    }
    end = clock();
    printf("runtime: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    close_file(file_dict);

    start = clock();
    FILE *f;
    kk = 0;
    char c[10000];
    f = fopen("string.txt", "r");
    while (fgets(c, 10000, f))
    {
        kk++;
    }
    end = clock();
    printf("runtime: %f, string_match:%d\n", (float)(end - start) / CLOCKS_PER_SEC, kk);
    fclose(f);
     getchar();
     getchar();
    return 0;
}
