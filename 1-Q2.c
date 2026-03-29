#include <stdio.h>
#include <stdbool.h>

int calib_map_linear(int x,
                     int x1, int y1,
                     int x2, int y2,
                     bool clamp, int y_min, int y_max,
                     int *out_y)
{
   
    if (out_y == NULL)          
        return -1;
    if (x1 == x2)              
        return -1;
    if (clamp && y_min > y_max) 
        return -1;

    long num = (long)(x  - x1) * (long)(y2 - y1);
    long den = (long)(x2 - x1);

   
    long adj  = (num >= 0) ? (den / 2L) : -(den / 2L);
    long term = (num + adj) / den;

   
    long y_long = (long)y1 + term;

   
    if (clamp) {
        if (y_long < (long)y_min) y_long = (long)y_min;
        if (y_long > (long)y_max) y_long = (long)y_max;
    }

   
    *out_y = (int)y_long;
    return 0;
}


typedef struct {
    int  x, x1, y1, x2, y2;
    bool clamp;
    int  y_min, y_max;
    bool null_ptr;          
    int  exp_ret;
    int  exp_y;            
    const char *note;
} TC;

int main(void)
{
    TC tests[] = {
        /* x   x1  y1   x2   y2  clamp ymin ymax  null exp_r exp_y   note */
        {  75, 50,  0, 100, 100, false,  0,  100, false,  0,  50, "Midpoint → 50"              },
        { 100, 50,  0, 100, 100, false,  0,  100, false,  0, 100, "Exact upper endpoint"        },
        { 120, 50,  0, 100, 100, true,   0,  100, false,  0, 100, "Extrapolate high, clamped"  },
        {  40, 50,  0, 100, 100, true,   0,  100, false,  0,   0, "Extrapolate low, clamped"   },
        {  60,100,200, 200, 400, false,  0,    0, false,  0, 120, "Extrapolation (rounded)"    },
        {  10, 10,500,  10, 900, false,  0,    0, false, -1,  -1, "Degenerate: x1 == x2"       },
        {  75, 50,  0, 100, 100, false,  0,  100, true,  -1,  -1, "NULL out_y pointer"         },
        {  75, 50,  0, 100, 100, true,  100,   0, false, -1,  -1, "Bad clamp: y_min > y_max"   },
        {  50, 50,  0, 100, 100, false,  0,    0, false,  0,   0, "Exact lower endpoint"       },
        {  75, 50, -50,100,  50, false,  0,    0, false,  0,   0, "Negative y values, mid=0"   },
    };

    int n    = (int)(sizeof(tests) / sizeof(tests[0]));
    int pass = 0, fail = 0;

    printf("%-4s %-38s %-8s %-8s %-8s %-8s %s\n",
           "#", "Note", "exp_ret", "got_ret", "exp_y", "got_y", "Result");
    printf("%.100s\n",
           "---------------------------------------------------------------------"
           "---------------------------------");

    for (int i = 0; i < n; i++) {
        TC *t   = &tests[i];
        int y   = 0xDEAD;   
        int *py = t->null_ptr ? NULL : &y;

        int ret = calib_map_linear(t->x,
                                   t->x1, t->y1,
                                   t->x2, t->y2,
                                   t->clamp, t->y_min, t->y_max,
                                   py);

        int ret_ok = (ret == t->exp_ret);
        int y_ok   = (t->exp_ret == -1)
                     ? (y == 0xDEAD)          
                     : (y == t->exp_y);
        int ok     = ret_ok && y_ok;

        ok ? pass++ : fail++;

        char exp_y_s[16], got_y_s[16];
        if (t->exp_ret == -1) { snprintf(exp_y_s, 16, "—"); snprintf(got_y_s, 16, "—"); }
        else { snprintf(exp_y_s, 16, "%d", t->exp_y); snprintf(got_y_s, 16, "%d", y); }

        printf("%-4d %-38s %-8d %-8d %-8s %-8s [%s]\n",
               i + 1, t->note,
               t->exp_ret, ret,
               exp_y_s, got_y_s,
               ok ? "PASS" : "FAIL");
    }

    printf("\n%d / %d tests passed.\n", pass, pass + fail);
    return (fail == 0) ? 0 : 1;
}