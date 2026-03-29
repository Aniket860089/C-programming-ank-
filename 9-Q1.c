#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum Mode { MODE0 = 0, MODE1 = 1, MODE2 = 2, MODE3 = 3 };

struct StatusBits {
    int rsv    : 7;
    int fault  : 1;
    int busy   : 1;
    int ready  : 1;
    int mode   : 2;
    int retries: 3;
    int par    : 1;
};

int build_status_reg16(enum Mode mode, int retries, bool ready, bool busy,
                       bool fault, int *out_reg16) {
    if (out_reg16 == NULL) return -1;
    if (retries < 0 || retries > 7) return -1;
    if (mode < MODE0 || mode > MODE3) return -1;

    int reg = 0;
    reg |= (retries & 0x7) << 12;
    reg |= (mode    & 0x3) << 10;
    reg |= (ready  ? 1 : 0) << 9;
    reg |= (busy   ? 1 : 0) << 8;
    reg |= (fault  ? 1 : 0) << 7;

    int tmp = reg & 0x7FFF;
    int parity = 0;
    for (int i = 0; i < 15; i++) {
        parity ^= (tmp >> i) & 1;
    }
    reg |= parity << 15;

    *out_reg16 = reg & 0xFFFF;
    return 0;
}

void print_reg(int reg) {
    printf("  binary: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (reg >> i) & 1);
        if (i == 15 || i == 12 || i == 10 || i == 9 || i == 8 || i == 7 || i == 1)
            printf("|");
    }
    printf("\n");
    printf("  PAR=%d RETRIES=%d MODE=%d READY=%d BUSY=%d FAULT=%d\n",
           (reg >> 15) & 1, (reg >> 12) & 7, (reg >> 10) & 3,
           (reg >> 9) & 1, (reg >> 8) & 1, (reg >> 7) & 1);
}

int main(void) {
    int reg, ret;

    ret = build_status_reg16(MODE0, 0, false, false, false, &reg);
    printf("TC1: ret=%d reg=0x%04X (expect ret=0, PAR=0 all zeros)\n", ret, reg);
    print_reg(reg);

    ret = build_status_reg16(MODE3, 7, true, true, true, &reg);
    printf("TC2: ret=%d reg=0x%04X (expect ret=0, dense case)\n", ret, reg);
    print_reg(reg);

    ret = build_status_reg16(MODE1, 5, true, false, false, &reg);
    printf("TC3: ret=%d reg=0x%04X (expect ret=0, mixed)\n", ret, reg);
    print_reg(reg);

    ret = build_status_reg16(MODE0, -1, false, false, false, &reg);
    printf("TC4: ret=%d (expect ret=-1)\n", ret);

    ret = build_status_reg16((enum Mode)4, 3, false, false, false, &reg);
    printf("TC5: ret=%d (expect ret=-1)\n", ret);

    ret = build_status_reg16(MODE0, 3, false, false, false, NULL);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    return 0;
}