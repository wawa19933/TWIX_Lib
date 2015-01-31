#ifndef MYUTILS
#define MYUTILS

#include <string.h>

template <typename T>
void pushData(T &source, void *destination)
{
    memcpy(destination, &source, sizeof(source));
}

template <typename T>
void popData(const void *source, T &destination)
{
    memcpy(&destination, source, sizeof(destination));
}

template <typename T>
void clearMemory(T &data)
{
    memset(&data, 0, sizeof(data));
}

#endif // MYUTILS

