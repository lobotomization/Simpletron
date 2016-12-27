#ifndef UTIL_H_HEADER
#define UTIL_H_HEADER
/*
* parseLong - Converts a string to a long, returns a pointer to the long.
*             Returns NULL if startPtr doesn't start with a long.
*
* longToString - Converts a long to a string
*/
long *parseLong(char *startPtr);
char *longToString(long l);
#endif //UTIL_H_HEADER
