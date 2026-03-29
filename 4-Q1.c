#include <stddef.h>
#include <stdio.h>

int count_above_threshold(const int *a, int n, int threshold, int *out_count) {
    if (a == NULL || out_count == NULL || n < 0) return -1;

    int count = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] > threshold) count++;
    }

    *out_count = count;
    return 0;
}

int main(void) {
    int out, ret;

    int a1[] = {1, 5, 9};
    ret = count_above_threshold(a1, 3, 4, &out);
    printf("TC1: ret=%d out=%d (expect ret=0, out=2)\n", ret, out);

    int a2[] = {10, 10, 10};
    ret = count_above_threshold(a2, 3, 10, &out);
    printf("TC2: ret=%d out=%d (expect ret=0, out=0)\n", ret, out);

    int a3[] = {-1, -5, -3};
    ret = count_above_threshold(a3, 3, -4, &out);
    printf("TC3: ret=%d out=%d (expect ret=0, out=2)\n", ret, out);

    ret = count_above_threshold(a1, 0, 5, &out);
    printf("TC4: ret=%d out=%d (expect ret=0, out=0)\n", ret, out);

    ret = count_above_threshold(NULL, 3, 5, &out);
    printf("TC5: ret=%d (expect ret=-1)\n", ret);

    ret = count_above_threshold(a1, 3, 5, NULL);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    ret = count_above_threshold(a1, -1, 5, &out);
    printf("TC7: ret=%d (expect ret=-1)\n", ret);

    return 0;
}