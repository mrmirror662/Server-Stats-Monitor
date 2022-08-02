#pragma once
#include <stdio.h>
typedef struct Load
{
    float avg1, avg5, avg15;
    int thread, totalthread;
} Load;
void printLoad(Load l)
{
    printf("avg load 1 min:%f\navg load 5 min:%f\navg load 15 min:%f\nthread:%d/%d\n\n", l.avg1, l.avg5, l.avg15, l.thread, l.totalthread);
}
