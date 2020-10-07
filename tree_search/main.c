#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_file_reader.h"

int main()
{
    char *str = NULL;
    FileHandler *file_a;
    FileHandler *file_b;
    file_a = open_file("dict.txt", "rb");
    file_b = open_file("output.txt", "wb");
    while (read_line(file_a, &str))
    {
        write_line(file_b, str);
    }
    close_file(file_a);
    close_file(file_b);
    getchar();
    return 0;
}