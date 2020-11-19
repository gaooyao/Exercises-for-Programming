#include <stdio.h>
#include <sys/time.h>

#include "config.h"
#include "build_graph.h"
//#include "pagerankyn.h"
#include "page_rank.h"
#include "brandes_between.h"

int main()
{
    struct timeval s, e;
    //s = clock();
    //build_graph(); //输出出链信息到graph.bin
    //e = clock();
    //printf("Build graph.bin time %f s.", (float)(e - s) / CLOCKS_PER_SEC);

    gettimeofday(&s, NULL);
    //build_graph(); //输出出链信息到graph.bin
    //pagerank_func();//
    page_rank_v3(); //输出出链信息到graph.bin

    //brandes_between();
    gettimeofday(&e, NULL);
    printf("ALL time: %.3f s.\n", (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec) / 1000000.0);
    return 0;
}
