#include <stdio.h>
#include <time.h>

#include "config.h"
#include "build_graph.h"

int main()
{
    clock_t s, e;
    s = clock();
    build_graph(); //输出出链信息到graph.bin
    e = clock();
    printf("time %f s.", (float)(e - s) / CLOCKS_PER_SEC);
    return 0;
}
