#include <stdio.h>
#include <stdbool.h>

enum Mode {
    MODE_SLEEP = 0,
    MODE_IDLE  = 1,
    MODE_RUN   = 2,
    MODE_BOOST = 3
};

int mode_to_cpu_mhz(enum Mode mode, bool power_save)
{
    int freq;

    switch (mode) {
        case MODE_SLEEP: return 0;
        case MODE_IDLE:  freq = 24;  break;
        case MODE_RUN:   freq = 48;  break;
        case MODE_BOOST: freq = 96;  break;
        default:         return -1;
    }

    if (power_save)
        freq /= 2;

    return freq;
}

typedef struct {
    enum Mode mode;
    bool      power_save;
    int       expected;
    const char *note;
} TC;

int main(void)
{
    TC tests[] = {
        { MODE_SLEEP,        false,  0, "Sleep, no power-save"         },
        { MODE_IDLE,         false, 24, "Idle base frequency"           },
        { MODE_RUN,          true,  24, "Run + power-save: 48/2 = 24"  },
        { MODE_BOOST,        true,  48, "Boost + power-save: 96/2 = 48"},
        { (enum Mode)99,     false, -1, "Invalid enum value"            },
        { MODE_SLEEP,        true,   0, "Sleep + power-save stays 0"   },
    };

    int n = (int)(sizeof(tests) / sizeof(tests[0]));
    int pass = 0, fail = 0;

    printf("%-4s %-34s %-10s %-10s %s\n", "#", "Note", "Expected", "Got", "Result");
    printf("%.75s\n", "------------------------------------------------------------------------");

    for (int i = 0; i < n; i++) {
        int got = mode_to_cpu_mhz(tests[i].mode, tests[i].power_save);
        int ok  = (got == tests[i].expected);
        ok ? pass++ : fail++;
        printf("%-4d %-34s %-10d %-10d [%s]\n",
               i + 1, tests[i].note, tests[i].expected, got, ok ? "PASS" : "FAIL");
    }

    printf("\n%d / %d tests passed.\n", pass, pass + fail);
    return (fail == 0) ? 0 : 1;
}