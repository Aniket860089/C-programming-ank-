#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum { RC_NOFAULT = 0, RC_SEEN_ONLY = 1, RC_CLEARED = 2, RC_BAD_ARG = -1 } RetCode;

extern long g_ms_tick;
long g_ms_tick = 0;

RetCode handle_w1c_fault(volatile int *reg, int fault_bit, bool allow_clear,
                         long cooldown_ms, int *out_seen_count) {
    if (reg == NULL || out_seen_count == NULL) return RC_BAD_ARG;
    if (fault_bit < 0 || fault_bit > 30) return RC_BAD_ARG;
    if (cooldown_ms < 0) return RC_BAD_ARG;

    static int seen_count = 0;
    static long last_clear_ms = -1;

    int mask = (1 << fault_bit);
    int val = *reg;

    if (!(val & mask)) {
        *out_seen_count = seen_count;
        return RC_NOFAULT;
    }

    seen_count++;

    if (allow_clear && (last_clear_ms < 0 || g_ms_tick - last_clear_ms >= cooldown_ms)) {
        *reg = mask;
        last_clear_ms = g_ms_tick;
        *out_seen_count = seen_count;
        return RC_CLEARED;
    }

    *out_seen_count = seen_count;
    return RC_SEEN_ONLY;
}

int main(void) {
    int seen;
    RetCode ret;
    volatile int reg;

    const char *rc_str[] = {"RC_NOFAULT", "RC_SEEN_ONLY", "RC_CLEARED", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "RC_BAD_ARG"};
    #define RC_NAME(r) ((r) == RC_BAD_ARG ? "RC_BAD_ARG" : \
                        (r) == RC_NOFAULT ? "RC_NOFAULT" : \
                        (r) == RC_SEEN_ONLY ? "RC_SEEN_ONLY" : "RC_CLEARED")

    /* TC1 */
    g_ms_tick = 0; reg = 0x00;
    ret = handle_w1c_fault(&reg, 3, true, 0, &seen);
    printf("TC1: ret=%s seen=%d (expect RC_NOFAULT, seen=0)\n", RC_NAME(ret), seen);

    /* TC2 */
    g_ms_tick = 0; reg = 0x08;
    ret = handle_w1c_fault(&reg, 3, false, 0, &seen);
    printf("TC2: ret=%s seen=%d reg=0x%02X (expect RC_SEEN_ONLY, seen=1, reg=0x08)\n",
           RC_NAME(ret), seen, reg);

    /* TC3 */
    g_ms_tick = 100; reg = 0x08;
    ret = handle_w1c_fault(&reg, 3, true, 0, &seen);
    printf("TC3: ret=%s seen=%d reg=0x%02X (expect RC_CLEARED, seen=2, reg=0x08 written)\n",
           RC_NAME(ret), seen, reg);

    /* TC4 */
    g_ms_tick = 120; reg = 0x08;
    ret = handle_w1c_fault(&reg, 3, true, 50, &seen);
    printf("TC4: ret=%s seen=%d (expect RC_SEEN_ONLY, seen=3)\n", RC_NAME(ret), seen);

    /* TC5 */
    g_ms_tick = 180; reg = 0x08;
    ret = handle_w1c_fault(&reg, 3, true, 50, &seen);
    printf("TC5: ret=%s seen=%d (expect RC_CLEARED, seen=4)\n", RC_NAME(ret), seen);

    /* TC6 */
    ret = handle_w1c_fault(NULL, 3, true, 0, &seen);
    printf("TC6a: ret=%s (expect RC_BAD_ARG)\n", RC_NAME(ret));

    ret = handle_w1c_fault(&reg, 3, true, 0, NULL);
    printf("TC6b: ret=%s (expect RC_BAD_ARG)\n", RC_NAME(ret));

    ret = handle_w1c_fault(&reg, 31, true, 0, &seen);
    printf("TC6c: ret=%s (expect RC_BAD_ARG)\n", RC_NAME(ret));

    return 0;
}