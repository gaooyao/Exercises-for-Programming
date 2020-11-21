#include <stdio.h>
#include <sys/time.h>

#include "build_graph.h"
#include "page_rank.h"

int main()
{
    struct timeval s, e;
    gettimeofday(&s, NULL);

    build_graph(); //输出出链信息到graph.bin
    page_rank();
    gettimeofday(&e, NULL);
    printf("ALL time: %.3f s.\n", (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec) / 1000000.0);
    return 0;
}
