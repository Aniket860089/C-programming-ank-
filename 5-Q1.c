#include <stddef.h>
#include <stdio.h>

int find_min_in_array(const int *a, int n, int *out_min) {
    if (a == NULL || out_min == NULL || n <= 0) return -1;

    int min_value = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] < min_value) min_value = a[i];
    }

    *out_min = min_value;
    return 0;
}

int main(void) {
    int out, ret;

    int a1[] = {5, 2, 8, 1, 9};
    ret = find_min_in_array(a1, 5, &out);
    printf("TC1: ret=%d out=%d (expect ret=0, out=1)\n", ret, out);

    int a2[] = {10};
    ret = find_min_in_array(a2, 1, &out);
    printf("TC2: ret=%d out=%d (expect ret=0, out=10)\n", ret, out);

    int a3[] = {-5, -2, -9, -1};
    ret = find_min_in_array(a3, 4, &out);
    printf("TC3: ret=%d out=%d (expect ret=0, out=-9)\n", ret, out);

    int a4[] = {100, 100, 100};
    ret = find_min_in_array(a4, 3, &out);
    printf("TC4: ret=%d out=%d (expect ret=0, out=100)\n", ret, out);

    ret = find_min_in_array(a1, 0, &out);
    printf("TC5: ret=%d (expect ret=-1)\n", ret);

    ret = find_min_in_array(NULL, 5, &out);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    ret = find_min_in_array(a1, 5, NULL);
    printf("TC7: ret=%d (expect ret=-1)\n", ret);

    return 0;
}