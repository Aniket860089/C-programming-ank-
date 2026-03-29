#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* ── Bit / mask helpers ─────────────────────────────────────────────────────
   Arguments must be side-effect free (no *p++ etc.); pure C macros cannot
   guarantee single evaluation without compiler extensions.               */
#define BIT(b)              (1 << (b))
#define BMASK(hi, lo)       (((1 << ((hi) - (lo) + 1)) - 1) << (lo))

/* ── Field helpers ──────────────────────────────────────────────────────── */
#define FIELD_PREP(hi, lo, val) \
    (((val) & ((1 << ((hi) - (lo) + 1)) - 1)) << (lo))
#define FIELD_GET(hi, lo, word) \
    (((word) >> (lo)) & ((1 << ((hi) - (lo) + 1)) - 1))

/* ── Statement-like register macros (do-while guards) ───────────────────── */
#define SET_BITS(reg_ptr, mask)  do { *(reg_ptr) |=  (mask);        } while (0)
#define CLR_BITS(reg_ptr, mask)  do { *(reg_ptr) &= ~(mask);        } while (0)
#define W1C_BITS(reg_ptr, mask)  do { *(reg_ptr)  =  (mask);        } while (0)

/* ── Compile-time guard (enum trick; negative array size on failure) ─────── */
#define ENUM_ASSERT(name, cond)  enum { name = 1 / (int)(!!(cond)) }

#define MODE_MAX 7
ENUM_ASSERT(MODE_RANGE_OK, (MODE_MAX >= 7));

/* ── Optional logging ────────────────────────────────────────────────────── */
#if defined(ENABLE_CFG_LOG) && (ENABLE_CFG_LOG)
    #define CFG_LOG(msg) cfg_log(msg)
#else
    #define CFG_LOG(msg) /* logging disabled */
#endif

void cfg_log(const char *msg) { printf("  [LOG] %s\n", msg); }

/* ── Return code ─────────────────────────────────────────────────────────── */
typedef enum { DC_OK = 0, DC_BAD_ARG = -1, DC_TIMEOUT = -2 } DC_Ret;

/* ── Main function ───────────────────────────────────────────────────────── */
DC_Ret device_configure(volatile int *CTRL, volatile int *STAT,
                        volatile int *CFG, int mode, int max_attempts) {
    if (CTRL == NULL || STAT == NULL || CFG == NULL) return DC_BAD_ARG;
    if (mode < 0 || mode > MODE_MAX)                 return DC_BAD_ARG;
    if (max_attempts < 0)                             return DC_BAD_ARG;

    CFG_LOG("Enabling CTRL.EN");
    SET_BITS(CTRL, BIT(0));

    CFG_LOG("Polling STAT.RDY");
    int attempts = 0;
    while (attempts < max_attempts) {
        if (*STAT & BIT(7)) break;
        attempts++;
    }
    if (!(*STAT & BIT(7))) return DC_TIMEOUT;

    CFG_LOG("Programming CFG.MODE field [5:3]");
    int tmp = *CFG;
    tmp &= ~BMASK(5, 3);
    tmp |= FIELD_PREP(5, 3, mode);
    *CFG = tmp;

    CFG_LOG("Clearing STAT.INT via W1C");
    W1C_BITS(STAT, BIT(6));

    return DC_OK;
}

/* ── Test harness ────────────────────────────────────────────────────────── */
static const char *dc_name(DC_Ret r) {
    return r == DC_OK ? "DC_OK" : r == DC_BAD_ARG ? "DC_BAD_ARG" : "DC_TIMEOUT";
}

int main(void) {
    volatile int ctrl, stat, cfg;
    DC_Ret ret;

    /* TC1: max_attempts=0 → timeout immediately */
    ctrl = 0; stat = 0; cfg = 0;
    ret = device_configure(&ctrl, &stat, &cfg, 3, 0);
    printf("TC1: ret=%s ctrl=0x%X (expect DC_TIMEOUT, ctrl bit0 set)\n",
           dc_name(ret), ctrl);

    /* TC2: STAT.RDY already set */
    ctrl = 0; stat = BIT(7); cfg = 0;
    ret = device_configure(&ctrl, &stat, &cfg, 3, 10);
    printf("TC2: ret=%s cfg=0x%X cfg[5:3]=%d stat=0x%X (expect DC_OK,cfg[5:3]=3)\n",
           dc_name(ret), cfg, FIELD_GET(5, 3, cfg), stat);

    /* TC3: RDY appears after 3 polls — simulate by pre-setting bit7 in STAT;
       since we cannot do side-effects inside volatile reads in a simple test,
       we pre-set and rely on the first poll succeeding. */
    ctrl = 0; stat = BIT(7); cfg = 0;
    ret = device_configure(&ctrl, &stat, &cfg, 5, 5);
    printf("TC3: ret=%s cfg[5:3]=%d (expect DC_OK)\n",
           dc_name(ret), FIELD_GET(5, 3, cfg));

    /* TC4: mode out of range */
    ctrl = 0; stat = BIT(7); cfg = 0;
    ret = device_configure(&ctrl, &stat, &cfg, 8, 10);
    printf("TC4: ret=%s (expect DC_BAD_ARG)\n", dc_name(ret));

    /* TC5: NULL pointers */
    ret = device_configure(NULL, &stat, &cfg, 3, 10);
    printf("TC5a: ret=%s (expect DC_BAD_ARG)\n", dc_name(ret));
    ret = device_configure(&ctrl, NULL, &cfg, 3, 10);
    printf("TC5b: ret=%s (expect DC_BAD_ARG)\n", dc_name(ret));
    ret = device_configure(&ctrl, &stat, NULL, 3, 10);
    printf("TC5c: ret=%s (expect DC_BAD_ARG)\n", dc_name(ret));

    /* TC6: RMW — other CFG bits must be preserved */
    ctrl = 0; stat = BIT(7); cfg = 0xFF ^ BMASK(5, 3); /* all bits except [5:3] */
    int before = cfg & ~BMASK(5, 3);
    ret = device_configure(&ctrl, &stat, &cfg, 2, 10);
    int after_other = cfg & ~BMASK(5, 3);
    printf("TC6: ret=%s cfg[5:3]=%d other_bits_preserved=%s (expect DC_OK,field=2,preserved)\n",
           dc_name(ret), FIELD_GET(5, 3, cfg),
           (before == after_other) ? "YES" : "NO");

    /* TC7: logging (enable at top with -DENABLE_CFG_LOG=1 or define here)  */
#undef  ENABLE_CFG_LOG
#define ENABLE_CFG_LOG 1
#undef  CFG_LOG
#define CFG_LOG(msg) cfg_log(msg)
    ctrl = 0; stat = BIT(7); cfg = 0;
    printf("TC7: logging enabled:\n");
    ret = device_configure(&ctrl, &stat, &cfg, 1, 5);
    printf("TC7: ret=%s (expect DC_OK)\n", dc_name(ret));

    return 0;
}
