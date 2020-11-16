#include <stdio.h>
#include <time.h>

#include "config.h"
#include "build_graph.h"
#include "page_rank.h"

int main()
{
    clock_t s, e;
    //s = clock();
    //build_graph(); //输出出链信息到graph.bin
    //e = clock();
    //printf("Build graph.bin time %f s.", (float)(e - s) / CLOCKS_PER_SEC);

    s = clock();
    page_rank_v2(); //输出出链信息到graph.bin
    e = clock();
    printf("ALL time: %f s.\n", (float)(e - s) / CLOCKS_PER_SEC);
    return 0;
}
