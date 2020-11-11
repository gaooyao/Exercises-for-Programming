
#include "config.h"
#include "build_graph.h"
#include <time.h>
int main(){
    clock_t s=clock();
    build_graph();
    clock_t e=clock();
    printf("%d",e-s/1000);
    return 0;
}