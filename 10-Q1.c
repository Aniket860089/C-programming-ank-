#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int dynint_append(int **buf, int *count, int *capacity, int value) {
    if (buf == NULL || count == NULL || capacity == NULL) return -1;
    if (*count < 0 || *capacity < 0) return -1;
    if (*capacity == 0 && *buf != NULL) return -1;
    if (*count > *capacity) return -1;

    if (*count < *capacity) {
        (*buf)[*count] = value;
        (*count)++;
        return 0;
    }

    int new_cap;
    if (*capacity == 0) {
        new_cap = 4;
    } else {
        if (*capacity > INT_MAX / 2) {
            long safe = (long)INT_MAX / (long)sizeof(int);
            if (safe <= *count) return -3;
            new_cap = (int)safe;
        } else {
            new_cap = *capacity * 2;
        }
    }

    if (new_cap < *count + 1) return -3;

    long alloc_size = (long)new_cap * (long)sizeof(int);
    if (alloc_size > (long)INT_MAX) return -3;

    int *tmp;
    if (*capacity == 0) {
        tmp = malloc((size_t)alloc_size);
    } else {
        tmp = realloc(*buf, (size_t)alloc_size);
    }

    if (tmp == NULL) return -2;

    *buf = tmp;
    *capacity = new_cap;
    (*buf)[*count] = value;
    (*count)++;
    return 0;
}

int main(void) {
    int *buf = NULL;
    int count = 0, capacity = 0;
    int ret;

    ret = dynint_append(&buf, &count, &capacity, 10);
    printf("TC1: ret=%d count=%d capacity=%d buf[0]=%d (expect ret=0,count=1,cap=4,buf[0]=10)\n",
           ret, count, capacity, buf[0]);

    dynint_append(&buf, &count, &capacity, 20);
    dynint_append(&buf, &count, &capacity, 30);
    dynint_append(&buf, &count, &capacity, 40);
    printf("TC2: ret=0 count=%d capacity=%d buf={%d,%d,%d,%d} (expect count=4,cap=4)\n",
           count, capacity, buf[0], buf[1], buf[2], buf[3]);

    ret = dynint_append(&buf, &count, &capacity, 50);
    printf("TC3: ret=%d count=%d capacity=%d buf[4]=%d (expect ret=0,count=5,cap=8,buf[4]=50)\n",
           ret, count, capacity, buf[4]);

    int *buf2 = NULL; int count2 = -1, cap2 = 0;
    ret = dynint_append(&buf2, &count2, &cap2, 1);
    printf("TC4a: ret=%d (expect ret=-1)\n", ret);

    int *buf3 = NULL; int count3 = 0, cap3 = -1;
    ret = dynint_append(&buf3, &count3, &cap3, 1);
    printf("TC4b: ret=%d (expect ret=-1)\n", ret);

    int *buf4 = NULL; int count4 = 0, cap4 = INT_MAX;
    ret = dynint_append(&buf4, &count4, &cap4, 1);
    printf("TC5: ret=%d (expect ret=-3 or -2)\n", ret);

    free(buf);
    return 0;
}