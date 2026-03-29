#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum AdmissionReason {
    REASON_ACCEPTED = 0,
    REASON_MAINTENANCE = 1,
    REASON_COOLDOWN = 2,
    REASON_MIN_GAP = 3,
    REASON_WINDOW_LIMIT = 4
};

int decide_admission(
    long now_ms,
    long *window_start_ms,
    int window_ms,
    int max_in_window,
    long min_gap_ms,
    long cooldown_ms,
    long *last_accept_ms,
    long *cooldown_until_ms,
    int *count_in_window,
    bool maintenance_mode,
    bool emergency_override,
    int *out_decision,
    enum AdmissionReason *out_reason)
{
    if (window_start_ms == NULL || last_accept_ms == NULL ||
        cooldown_until_ms == NULL || count_in_window == NULL ||
        out_decision == NULL || out_reason == NULL)
        return -1;

    if (now_ms < 0 || *window_start_ms < 0 || window_ms <= 0 ||
        max_in_window < 1 || min_gap_ms < 0 || cooldown_ms < 0 ||
        *count_in_window < 0)
        return -1;

    int decision;
    enum AdmissionReason reason;

    if (now_ms >= *window_start_ms + window_ms) {
        *window_start_ms = now_ms;
        *count_in_window = 0;
    }

    if (maintenance_mode && !emergency_override) {
        decision = 0;
        reason = REASON_MAINTENANCE;
    } else if (now_ms < *cooldown_until_ms) {
        decision = 0;
        reason = REASON_COOLDOWN;
    } else if (*last_accept_ms >= 0 && (now_ms - *last_accept_ms) < min_gap_ms) {
        decision = 0;
        reason = REASON_MIN_GAP;
    } else if (*count_in_window >= max_in_window) {
        decision = 0;
        reason = REASON_WINDOW_LIMIT;
        *cooldown_until_ms = now_ms + cooldown_ms;
    } else {
        *last_accept_ms = now_ms;
        (*count_in_window)++;
        decision = 1;
        reason = REASON_ACCEPTED;
    }

    *out_decision = decision;
    *out_reason = reason;
    return 0;
}

int main(void) {
    long window_start, last_accept, cooldown_until;
    int count, decision;
    enum AdmissionReason reason;
    int ret;

    const char *reason_str[] = {
        "ACCEPTED", "MAINTENANCE", "COOLDOWN", "MIN_GAP", "WINDOW_LIMIT"
    };

    /* TC1 */
    window_start = 1000; last_accept = -1; cooldown_until = 0; count = 0;
    ret = decide_admission(1000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC1:  ret=%d decision=%d reason=%s | last=%ld count=%d\n",
           ret, decision, reason_str[reason], last_accept, count);

    /* TC2 */
    ret = decide_admission(1200, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC2:  ret=%d decision=%d reason=%s | last=%ld count=%d\n",
           ret, decision, reason_str[reason], last_accept, count);

    /* TC3 */
    ret = decide_admission(1700, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC3:  ret=%d decision=%d reason=%s | last=%ld count=%d\n",
           ret, decision, reason_str[reason], last_accept, count);

    /* TC4 */
    ret = decide_admission(2300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC4:  ret=%d decision=%d reason=%s | last=%ld count=%d\n",
           ret, decision, reason_str[reason], last_accept, count);

    /* TC5 */
    ret = decide_admission(2400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC5:  ret=%d decision=%d reason=%s | cooldown_until=%ld\n",
           ret, decision, reason_str[reason], cooldown_until);

    /* TC6 */
    ret = decide_admission(5300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC6:  ret=%d decision=%d reason=%s\n",
           ret, decision, reason_str[reason]);

    /* TC7 */
    ret = decide_admission(5400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC7:  ret=%d decision=%d reason=%s\n",
           ret, decision, reason_str[reason]);

    /* TC8 */
    ret = decide_admission(11000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC8:  ret=%d decision=%d reason=%s | window_start=%ld count=%d last=%ld\n",
           ret, decision, reason_str[reason], window_start, count, last_accept);

    /* TC9 */
    ret = decide_admission(12000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, false, &decision, &reason);
    printf("TC9:  ret=%d decision=%d reason=%s\n",
           ret, decision, reason_str[reason]);

    /* TC10 */
    ret = decide_admission(12550, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, true, &decision, &reason);
    printf("TC10: ret=%d decision=%d reason=%s\n",
           ret, decision, reason_str[reason]);

    /* TC11 */
    ret = decide_admission(1000, &window_start, 0, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC11: ret=%d (expect -1)\n", ret);

    ret = decide_admission(1000, NULL, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC11b: ret=%d (expect -1)\n", ret);

    return 0;
}