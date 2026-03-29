#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int debounce_and_fire(const int *samples, int n, int k,
                      void (*on_press)(int event_code), bool *out_invoked) {
    if (samples == NULL || on_press == NULL || out_invoked == NULL ||
        n < 0 || k < 1) return -1;

    if (n < k) {
        *out_invoked = false;
        return 0;
    }

    bool all_one = true;
    for (int i = n - k; i < n; i++) {
        if (samples[i] != 1) { all_one = false; break; }
    }

    if (all_one) {
        on_press(1);
        *out_invoked = true;
    } else {
        *out_invoked = false;
    }

    return 0;
}

void my_callback(int event_code) {
    printf("  [callback fired: event_code=%d]\n", event_code);
}

int main(void) {
    bool invoked;
    int ret;

    int a1[] = {0, 1, 1, 1};
    ret = debounce_and_fire(a1, 4, 3, my_callback, &invoked);
    printf("TC1: ret=%d invoked=%s (expect ret=0,invoked=true)\n",
           ret, invoked ? "true" : "false");

    int a2[] = {1, 1, 0, 1};
    ret = debounce_and_fire(a2, 4, 2, my_callback, &invoked);
    printf("TC2: ret=%d invoked=%s (expect ret=0,invoked=false)\n",
           ret, invoked ? "true" : "false");

    int a3[] = {1, 1};
    ret = debounce_and_fire(a3, 2, 3, my_callback, &invoked);
    printf("TC3: ret=%d invoked=%s (expect ret=0,invoked=false)\n",
           ret, invoked ? "true" : "false");

    int a4[] = {1, 1, 1, 1};
    ret = debounce_and_fire(a4, 4, 4, my_callback, &invoked);
    printf("TC4: ret=%d invoked=%s (expect ret=0,invoked=true)\n",
           ret, invoked ? "true" : "false");

    int a5[] = {0, 0, 0};
    ret = debounce_and_fire(a5, 3, 1, my_callback, &invoked);
    printf("TC5: ret=%d invoked=%s (expect ret=0,invoked=false)\n",
           ret, invoked ? "true" : "false");

    ret = debounce_and_fire(NULL, 4, 3, my_callback, &invoked);
    printf("TC6a: ret=%d (expect ret=-1)\n", ret);

    ret = debounce_and_fire(a1, 4, 3, NULL, &invoked);
    printf("TC6b: ret=%d (expect ret=-1)\n", ret);

    ret = debounce_and_fire(a1, 4, 3, my_callback, NULL);
    printf("TC6c: ret=%d (expect ret=-1)\n", ret);

    ret = debounce_and_fire(a1, -1, 3, my_callback, &invoked);
    printf("TC7a: ret=%d (expect ret=-1)\n", ret);

    ret = debounce_and_fire(a1, 4, 0, my_callback, &invoked);
    printf("TC7b: ret=%d (expect ret=-1)\n", ret);

    return 0;
}