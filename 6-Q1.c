#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int safe_copy_label(const char *src, char *dst, int dst_capacity,
                    bool to_lower, int *out_copied, bool *out_truncated) {
    if (src == NULL || dst == NULL || out_copied == NULL ||
        out_truncated == NULL || dst_capacity < 0) return -1;

    int limit = (dst_capacity > 0) ? (dst_capacity - 1) : 0;

    int i = 0;
    for (i = 0; i < limit && src[i] != '\0'; i++) {
        char ch = src[i];
        if (to_lower && ch >= 'A' && ch <= 'Z') ch = ch + ('a' - 'A');
        dst[i] = ch;
    }

    if (dst_capacity > 0) dst[i] = '\0';

    *out_copied = i;
    *out_truncated = (src[i] != '\0');
    return 0;
}

int main(void) {
    char buf[32];
    int copied, ret;
    bool truncated;

    ret = safe_copy_label("Hello", buf, 10, false, &copied, &truncated);
    printf("TC1: ret=%d copied=%d truncated=%s dst=\"%s\" (expect ret=0,copied=5,truncated=false,dst=\"Hello\")\n",
           ret, copied, truncated ? "true" : "false", buf);

    ret = safe_copy_label("Hello", buf, 6, true, &copied, &truncated);
    printf("TC2: ret=%d copied=%d truncated=%s dst=\"%s\" (expect ret=0,copied=5,truncated=false,dst=\"hello\")\n",
           ret, copied, truncated ? "true" : "false", buf);

    ret = safe_copy_label("FirmwareV1", buf, 5, false, &copied, &truncated);
    printf("TC3: ret=%d copied=%d truncated=%s dst=\"%s\" (expect ret=0,copied=4,truncated=true,dst=\"Firm\")\n",
           ret, copied, truncated ? "true" : "false", buf);

    ret = safe_copy_label("", buf, 3, true, &copied, &truncated);
    printf("TC4: ret=%d copied=%d truncated=%s dst=\"%s\" (expect ret=0,copied=0,truncated=false,dst=\"\")\n",
           ret, copied, truncated ? "true" : "false", buf);

    ret = safe_copy_label("ABC", buf, 0, false, &copied, &truncated);
    printf("TC5: ret=%d copied=%d truncated=%s (expect ret=0,copied=0,truncated=true)\n",
           ret, copied, truncated ? "true" : "false");

    ret = safe_copy_label(NULL, buf, 10, false, &copied, &truncated);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    ret = safe_copy_label("Hello", NULL, 10, false, &copied, &truncated);
    printf("TC7: ret=%d (expect ret=-1)\n", ret);

    ret = safe_copy_label("Hello", buf, -1, false, &copied, &truncated);
    printf("TC8: ret=%d (expect ret=-1)\n", ret);

    return 0;
}