#include "string_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ac_tree.h"

clock_t start, end;
int main()
{

    printf("输入文件要求：编码格式全部为GB2312，pattern.txt换行符为\\n，string.txt换行符为\\r\\n。若搜索结果只有英文则说明文件编码不相同。\n");

    char *str = NULL;
    //打开pattern文件，建树
    FileHandler *file_pattern;
    file_pattern = open_file("pattern.txt", "rb");
    init_tree();
    start = clock();
    while (read_line(file_pattern, &str))
    {
        insert_recoder(str);
    }
    end = clock();

    printf("build tree finished, use %f seconds\n", (float)(end - start) / CLOCKS_PER_SEC);

    close_file(file_pattern);

    //构建失效指针
    start = clock();
    make_turn();
    end = clock();

    printf("build fail point finished, use %f seconds\n", (float)(end - start) / CLOCKS_PER_SEC);


    //打开string文件，查找
    int line_num = 0;
    FileHandler *file_string;
    file_string = open_file("string.txt", "rb");
    file_string->end_flag_len = 2;
    start = clock();
    while (read_line(file_string, &str))
    {
        line_num++;
       //if (line_num % 20000 == 0)
        //{
            //break;
             //printf("%d\n", line_num);
        //}
        query_recoder(str, line_num);
    }
    end = clock();

    printf("search finished, use %f seconds\n", (float)(end - start) / CLOCKS_PER_SEC);

    close_file(file_string);
    printf("sorting now...");
    //输出到文件
    start = clock();
    out_to_result();
    end = clock();

    printf("output to file finished, use %f seconds\n", (float)(end - start) / CLOCKS_PER_SEC);
    sleep(3);


    return 0;
}
