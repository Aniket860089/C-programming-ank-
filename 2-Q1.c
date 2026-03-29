#include <stdio.h>
#include <math.h>

int compute_kmpl(int distance_meters, int fuel_milliliters, double *out_kmpl)
{
    if (out_kmpl == NULL)          return -1;
    if (distance_meters < 0)       return -1;
    if (fuel_milliliters <= 0)     return -1;

    *out_kmpl = (distance_meters / 1000.0) / (fuel_milliliters / 1000.0);
    return 0;
}

typedef struct {
    int    distance_meters;
    int    fuel_milliliters;
    int    null_ptr;
    int    exp_ret;
    double exp_kmpl;
    double tolerance;
    const char *note;
} TC;

int main(void)
{
    TC tests[] = {
        {     0,  500, 0,  0,  0.000, 1e-9, "Zero distance → 0 km/L"          },
        { 50000, 2500, 0,  0, 20.000, 1e-9, "50 km / 2.5 L = 20.0"            },
        { 12345,  678, 0,  0, 18.207, 1e-3, "FP precision check ≈ 18.21"      },
        {  1000,    0, 0, -1,  0.000, 0.0,  "Invalid: fuel = 0"               },
        {   -10,  100, 0, -1,  0.000, 0.0,  "Invalid: negative distance"      },
        {  1000, 1000, 1, -1,  0.000, 0.0,  "Invalid: NULL out_kmpl"          },
        {   999, 1000, 0,  0,  0.999, 1e-9, "Sub-km distance FP correctness"  },
    };

    int n = (int)(sizeof(tests) / sizeof(tests[0]));
    int pass = 0, fail = 0;

    printf("%-4s %-34s %-8s %-8s %-10s %-10s %s\n",
           "#", "Note", "exp_ret", "got_ret", "exp_kmpl", "got_kmpl", "Result");
    printf("%.90s\n",
           "------------------------------------------------------------------------------------------");

    for (int i = 0; i < n; i++) {
        double kmpl = -999.0;
        double *ptr = tests[i].null_ptr ? NULL : &kmpl;

        int ret = compute_kmpl(tests[i].distance_meters,
                               tests[i].fuel_milliliters, ptr);

        int ret_ok  = (ret == tests[i].exp_ret);
        int kmpl_ok = (tests[i].exp_ret == -1)
                      ? (kmpl == -999.0)
                      : (fabs(kmpl - tests[i].exp_kmpl) <= tests[i].tolerance);
        int ok = ret_ok && kmpl_ok;

        ok ? pass++ : fail++;

        char exp_s[16], got_s[16];
        if (tests[i].exp_ret == -1) {
            snprintf(exp_s, 16, "—");
            snprintf(got_s, 16, "—");
        } else {
            snprintf(exp_s, 16, "%.3f", tests[i].exp_kmpl);
            snprintf(got_s, 16, "%.3f", kmpl);
        }

        printf("%-4d %-34s %-8d %-8d %-10s %-10s [%s]\n",
               i + 1, tests[i].note,
               tests[i].exp_ret, ret,
               exp_s, got_s,
               ok ? "PASS" : "FAIL");
    }

    printf("\n%d / %d tests passed.\n", pass, pass + fail);
    return (fail == 0) ? 0 : 1;
}