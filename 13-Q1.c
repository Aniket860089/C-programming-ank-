#include <stddef.h>
#include <stdio.h>

static void insertion_sort_range(int *a, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= left && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

static void merge_sort(int *a, int left, int right, int *scratch) {
    int m = right - left + 1;
    if (m <= 16) {
        insertion_sort_range(a, left, right);
        return;
    }

    int mid = left + (right - left) / 2;
    merge_sort(a, left, mid, scratch);
    merge_sort(a, mid + 1, right, scratch);

    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (a[i] <= a[j]) scratch[k++] = a[i++];
        else               scratch[k++] = a[j++];
    }
    while (i <= mid)  scratch[k++] = a[i++];
    while (j <= right) scratch[k++] = a[j++];

    for (int x = 0; x < m; x++) a[left + x] = scratch[x];
}

int stable_sort_range_with_scratch(int *a, int n, int left, int right,
                                   int *scratch, int scratch_capacity,
                                   int *out_sorted_count) {
    if (a == NULL || scratch == NULL || out_sorted_count == NULL) return -1;
    if (n < 0) return -1;

    if (n == 0) {
        *out_sorted_count = 0;
        return 0;
    }

    if (left < 0 || right < left || right >= n) return -1;

    int m = right - left + 1;
    if (scratch_capacity < m) return -1;

    merge_sort(a, left, right, scratch);

    *out_sorted_count = m;
    return 0;
}

static void print_array(const int *a, int n) {
    printf("[");
    for (int i = 0; i < n; i++) printf("%d%s", a[i], i < n-1 ? "," : "");
    printf("]");
}

int main(void) {
    int scratch[16];
    int sorted_count, ret;

    int a1[] = {5, 3, 3, 2, 9};
    ret = stable_sort_range_with_scratch(a1, 5, 0, 4, scratch, 5, &sorted_count);
    printf("TC1: ret=%d count=%d a=", ret, sorted_count);
    print_array(a1, 5);
    printf(" (expect ret=0,count=5,[2,3,3,5,9])\n");

    int a2[] = {4, 1, 4, 2, 4};
    ret = stable_sort_range_with_scratch(a2, 5, 1, 3, scratch, 3, &sorted_count);
    printf("TC2: ret=%d count=%d a=", ret, sorted_count);
    print_array(a2, 5);
    printf(" (expect ret=0,count=3,[4,1,2,4,4])\n");

    int a3[] = {3, 3, 3};
    ret = stable_sort_range_with_scratch(a3, 3, 0, 2, scratch, 3, &sorted_count);
    printf("TC3: ret=%d count=%d a=", ret, sorted_count);
    print_array(a3, 3);
    printf(" (expect ret=0,count=3,[3,3,3] stable)\n");

    int a4[] = {1, 2, 3, 4};
    ret = stable_sort_range_with_scratch(a4, 4, 0, 3, scratch, 4, &sorted_count);
    printf("TC4: ret=%d count=%d a=", ret, sorted_count);
    print_array(a4, 4);
    printf(" (expect ret=0,count=4,[1,2,3,4])\n");

    int a5[] = {4, 3, 2, 1};
    ret = stable_sort_range_with_scratch(a5, 4, 0, 3, scratch, 4, &sorted_count);
    printf("TC5: ret=%d count=%d a=", ret, sorted_count);
    print_array(a5, 4);
    printf(" (expect ret=0,count=4,[1,2,3,4])\n");

    int a6[] = {4, 3, 2, 1};
    ret = stable_sort_range_with_scratch(a6, 4, 0, 3, scratch, 2, &sorted_count);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    ret = stable_sort_range_with_scratch(NULL, 4, 0, 3, scratch, 4, &sorted_count);
    printf("TC7a: ret=%d (expect ret=-1)\n", ret);

    ret = stable_sort_range_with_scratch(a5, 4, 0, 3, NULL, 4, &sorted_count);
    printf("TC7b: ret=%d (expect ret=-1)\n", ret);

    ret = stable_sort_range_with_scratch(a5, 4, 0, 3, scratch, 4, NULL);
    printf("TC7c: ret=%d (expect ret=-1)\n", ret);

    ret = stable_sort_range_with_scratch(a5, 0, 0, 0, scratch, 4, &sorted_count);
    printf("TC8: ret=%d count=%d (expect ret=0,count=0)\n", ret, sorted_count);

    ret = stable_sort_range_with_scratch(a5, 4, 2, 1, scratch, 4, &sorted_count);
    printf("TC9a: ret=%d (expect ret=-1)\n", ret);

    ret = stable_sort_range_with_scratch(a5, 4, 0, 4, scratch, 4, &sorted_count);
    printf("TC9b: ret=%d (expect ret=-1)\n", ret);

    return 0;
}