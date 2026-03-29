#include <stddef.h>
#include <stdio.h>

int find_subsequence_in_slice(const char *buf, int n, const char *needle, int m,
                              char **out_ptr, int *out_offset) {
    if (buf == NULL || needle == NULL || out_ptr == NULL ||
        out_offset == NULL || n < 0 || m <= 0) return -1;

    const char *end = buf + n;

    for (const char *p = buf; p + m <= end; ++p) {
        const char *q = p;
        const char *r = needle;
        while (r < needle + m && *q == *r) { q++; r++; }
        if (r == needle + m) {
            *out_ptr = (char *)p;
            *out_offset = (int)(p - buf);
            return 0;
        }
    }

    *out_ptr = NULL;
    *out_offset = -1;
    return 0;
}

int main(void) {
    char *ptr;
    int offset, ret;

    const char *buf1 = "ABCDEF";
    ret = find_subsequence_in_slice(buf1, 6, "CDE", 3, &ptr, &offset);
    printf("TC1: ret=%d offset=%d match=%s (expect ret=0,offset=2,match=CDE)\n",
           ret, offset, ptr ? ptr : "NULL");

    const char *buf2 = "AAAAA";
    ret = find_subsequence_in_slice(buf2, 5, "AAA", 3, &ptr, &offset);
    printf("TC2: ret=%d offset=%d (expect ret=0,offset=0)\n", ret, offset);

    const char *buf3 = "ABCDE";
    ret = find_subsequence_in_slice(buf3, 5, "DEF", 3, &ptr, &offset);
    printf("TC3: ret=%d offset=%d ptr=%s (expect ret=0,offset=-1,ptr=NULL)\n",
           ret, offset, ptr ? ptr : "NULL");

    const char *buf4 = "XYZ";
    ret = find_subsequence_in_slice(buf4, 3, "XYZ", 3, &ptr, &offset);
    printf("TC4: ret=%d offset=%d (expect ret=0,offset=0)\n", ret, offset);

    const char *buf5 = "HI";
    ret = find_subsequence_in_slice(buf5, 2, "HIK", 3, &ptr, &offset);
    printf("TC5: ret=%d offset=%d ptr=%s (expect ret=0,offset=-1,ptr=NULL)\n",
           ret, offset, ptr ? ptr : "NULL");

    ret = find_subsequence_in_slice(NULL, 5, "AB", 2, &ptr, &offset);
    printf("TC6a: ret=%d (expect ret=-1)\n", ret);

    ret = find_subsequence_in_slice(buf1, 6, NULL, 2, &ptr, &offset);
    printf("TC6b: ret=%d (expect ret=-1)\n", ret);

    ret = find_subsequence_in_slice(buf1, -1, "AB", 2, &ptr, &offset);
    printf("TC7a: ret=%d (expect ret=-1)\n", ret);

    ret = find_subsequence_in_slice(buf1, 6, "AB", 0, &ptr, &offset);
    printf("TC7b: ret=%d (expect ret=-1)\n", ret);

    return 0;
}