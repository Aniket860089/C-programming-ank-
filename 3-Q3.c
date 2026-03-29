#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum Action {
    ACT_NONE = 0,
    ACT_START = 1,
    ACT_STOP = 2,
    ACT_RESET = 3,
    ACT_DIAG = 4,
    ACT_UPDATE = 5,
    ACT_SHUTDOWN = 6
};

int interpret_uart_command(char cmd, bool maintenance_mode, bool locked, enum Action *out_action) {
    if (out_action == NULL) return -1;

    switch (cmd) {
        case 'S':
            if (locked) return -3;
            *out_action = ACT_START;
            return 0;

        case 'T':
            *out_action = ACT_STOP;
            return 0;

        case 'R':
            *out_action = ACT_RESET;
            return 0;

        case 'D':
            if (!maintenance_mode) return -3;
            if (locked) return -3;
            *out_action = ACT_DIAG;
            return 0;

        case 'U':
            if (!maintenance_mode) return -3;
            if (locked) return -3;
            *out_action = ACT_UPDATE;
            return 0;

        case 'Q':
            if (locked) return -3;
            *out_action = ACT_SHUTDOWN;
            return 0;

        default:
            return -2;
    }
}

int main(void) {
    enum Action action;
    int ret;

    const char *action_str[] = {
        "ACT_NONE", "ACT_START", "ACT_STOP", "ACT_RESET",
        "ACT_DIAG", "ACT_UPDATE", "ACT_SHUTDOWN"
    };

    struct { char cmd; bool maint; bool locked; int expect_ret; } tc[] = {
        {'S', false, false,  0},
        {'S', false, true,  -3},
        {'T', false, true,   0},
        {'R', false, true,   0},
        {'D', false, false, -3},
        {'D', true,  false,  0},
        {'U', true,  true,  -3},
        {'Q', false, false,  0},
        {'Q', false, true,  -3},
        {'X', false, false, -2},
    };

    for (int i = 0; i < 10; i++) {
        action = ACT_NONE;
        ret = interpret_uart_command(tc[i].cmd, tc[i].maint, tc[i].locked, &action);
        printf("TC%-2d: cmd='%c' maint=%-5s locked=%-5s → ret=%-2d",
               i + 1, tc[i].cmd,
               tc[i].maint ? "true" : "false",
               tc[i].locked ? "true" : "false",
               ret);
        if (ret == 0)
            printf(" action=%s", action_str[action]);
        printf(" (expect ret=%d)\n", tc[i].expect_ret);
    }

    /* TC11: NULL pointer */
    ret = interpret_uart_command('S', false, false, NULL);
    printf("TC11: out_action=NULL → ret=%d (expect ret=-1)\n", ret);

    return 0;
}