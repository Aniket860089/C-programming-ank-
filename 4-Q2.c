#include <stddef.h>
#include <stdio.h>

int drain_ring_snapshot(const char *buf, int capacity, int read_index_snapshot,
                        int write_index_snapshot, int max_to_copy,
                        char *out, int *out_count) {
    if (buf == NULL || out == NULL || out_count == NULL) return -1;
    if (capacity <= 0 || max_to_copy < 0) return -1;
    if (read_index_snapshot < 0 || read_index_snapshot >= capacity) return -1;
    if (write_index_snapshot < 0 || write_index_snapshot >= capacity) return -1;

    int i, idx;
    for (i = 0, idx = read_index_snapshot;
         i < max_to_copy && idx != write_index_snapshot;
         i++, idx = (idx + 1) % capacity) {
        out[i] = buf[idx];
    }

    *out_count = i;
    return 0;
}

int main(void) {
    char out[32];
    int count, ret;

    const char *buf = "ABCDEFGH";

    ret = drain_ring_snapshot(buf, 8, 2, 6, 10, out, &count);
    out[count] = '\0';
    printf("TC1:  ret=%d count=%d out=\"%s\" (expect ret=0, count=4, out=\"CDEF\")\n", ret, count, out);

    ret = drain_ring_snapshot(buf, 8, 6, 2, 10, out, &count);
    out[count] = '\0';
    printf("TC2:  ret=%d count=%d out=\"%s\" (expect ret=0, count=4, out=\"GHAB\")\n", ret, count, out);

    ret = drain_ring_snapshot(buf, 8, 3, 3, 5, out, &count);
    printf("TC3:  ret=%d count=%d (expect ret=0, count=0)\n", ret, count);

    ret = drain_ring_snapshot(buf, 8, 1, 5, 2, out, &count);
    out[count] = '\0';
    printf("TC4:  ret=%d count=%d out=\"%s\" (expect ret=0, count=2, out=\"BC\")\n", ret, count, out);

    ret = drain_ring_snapshot(buf, 8, 7, 0, 5, out, &count);
    out[count] = '\0';
    printf("TC5:  ret=%d count=%d out=\"%s\" (expect ret=0, count=1, out=\"H\")\n", ret, count, out);

    ret = drain_ring_snapshot(buf, 8, 0, 5, 0, out, &count);
    printf("TC6:  ret=%d count=%d (expect ret=0, count=0)\n", ret, count);

    ret = drain_ring_snapshot("X", 1, 0, 0, 3, out, &count);
    printf("TC7:  ret=%d count=%d (expect ret=0, count=0)\n", ret, count);

    ret = drain_ring_snapshot(buf, 8, 8, 0, 3, out, &count);
    printf("TC8:  ret=%d (expect ret=-1)\n", ret);

    ret = drain_ring_snapshot(buf, 0, 0, 0, 3, out, &count);
    printf("TC9:  ret=%d (expect ret=-1)\n", ret);

    ret = drain_ring_snapshot(NULL, 8, 0, 5, 3, out, &count);
    printf("TC10: ret=%d (expect ret=-1)\n", ret);

    return 0;
}