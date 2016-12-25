#include <stdio.h>
#include <stdlib.h>
long *parseLong(char *startPtr){
    char *endPtr;
    long outValue;
    long *output = NULL;
    outValue = strtol(startPtr, &endPtr, 10);
    if(endPtr - startPtr > 0){
        output = (long *)malloc(sizeof(long));
        *output = outValue;
    }
    return output;
}
char *longToString(long l){
    const int n = snprintf(NULL, 0, "%ld", l);
    char *buf = (char *)malloc(n+1);
    int c = snprintf(buf, n+1, "%ld", l);
    return buf;
}
