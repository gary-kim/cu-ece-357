#include <stdio.h>
#include <stdlib.h>

int initializedStaticGlobal = 153;
int uninitializedStaticGlobal;

int main() {
    int stackVal = 0;
    int* brk = malloc(sizeof(int));
    *brk = 10;
    
    printf("%p\n", &initializedStaticGlobal);
    printf("%p\n", &uninitializedStaticGlobal);
    printf("%p\n", &stackVal);
    printf("%p\n", brk);
}
