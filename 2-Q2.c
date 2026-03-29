#include <stdio.h>
#include <stdbool.h>

int consume_below_threshold(const int *a, int n, int *index, int threshold, int limit, int *out_sum, int *out_count) {
    if (a == NULL || n < 0 || index == NULL || out_sum == NULL || out_count == NULL || limit < 0) {
        return -1;
    }

    if (*index < 0 || *index > n) {
        return -1;
    }

    int sum = 0;
    int cnt = 0;

    while (cnt < limit && *index < n && a[*index] < threshold) {
        sum += a[*index];
        (*index)++;
        cnt++;
    }

    *out_sum = sum;
    *out_count = cnt;

    return 0;
}

int main() {
    int arr1[] = {1, 2, 3, 9, 1};
    int idx1 = 0, sum1, cnt1;
    int r1 = consume_below_threshold(arr1, 5, &idx1, 5, 10, &sum1, &cnt1);
    printf("Test 1: ret=%d, sum=%d, cnt=%d, idx=%d\n", r1, sum1, cnt1, idx1);

    int arr2[] = {-1, -2, -3};
    int idx2 = 0, sum2, cnt2;
    int r2 = consume_below_threshold(arr2, 3, &idx2, 0, 2, &sum2, &cnt2);
    printf("Test 2: ret=%d, sum=%d, cnt=%d, idx=%d\n", r2, sum2, cnt2, idx2);

    int arr3[] = {1, 2, 3};
    int idx3 = 3, sum3, cnt3;
    int r3 = consume_below_threshold(arr3, 3, &idx3, 10, 5, &sum3, &cnt3);
    printf("Test 3: ret=%d, sum=%d, cnt=%d, idx=%d\n", r3, sum3, cnt3, idx3);

    int arr4[] = {4, 4, 4};
    int idx4 = 0, sum4, cnt4;
    int r4 = consume_below_threshold(arr4, 3, &idx4, 4, 5, &sum4, &cnt4);
    printf("Test 4: ret=%d, sum=%d, cnt=%d, idx=%d\n", r4, sum4, cnt4, idx4);

    int arr5[] = {1, 100, 2};
    int idx5 = 0, sum5, cnt5;
    int r5 = consume_below_threshold(arr5, 3, &idx5, 50, 5, &sum5, &cnt5);
    printf("Test 5: ret=%d, sum=%d, cnt=%d, idx=%d\n", r5, sum5, cnt5, idx5);

    int arr6[] = {1, 2, 3};
    int idx6 = 0, sum6, cnt6;
    int r6 = consume_below_threshold(arr6, 3, &idx6, 5, 0, &sum6, &cnt6);
    printf("Test 6: ret=%d, sum=%d, cnt=%d, idx=%d\n", r6, sum6, cnt6, idx6);

    int r7 = consume_below_threshold(NULL, 5, &idx6, 5, 5, &sum6, &cnt6);
    printf("Test 7: ret=%d\n", r7);

    int arr8[] = {1};
    int idx8 = 0, sum8, cnt8;
    int r8 = consume_below_threshold(arr8, -1, &idx8, 5, 1, &sum8, &cnt8);
    printf("Test 8: ret=%d\n", r8);

    return 0;
}