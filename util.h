#pragma once
#include <string.h>
#include <stdlib.h>
int binSearch(char **strings, int N, char *key)
{
    int low = 0;
    int high = N - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        int cmp = strcmp(key, strings[mid]);

        if (cmp == 0)
            return mid;
        if (cmp < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return -1;
}
static int myCompare(const void *a, const void *b)
{

    return strcmp(*(const char **)a, *(const char **)b);
}

void sortStr(const char *arr[], int n)
{

    qsort(arr, n, sizeof(const char *), myCompare);
}