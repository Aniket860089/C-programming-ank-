#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int count_lines_filtered(const char *path, bool ignore_blank,
                         bool ignore_comment_hash, int *out_count) {
    if (path == NULL || out_count == NULL) return -1;

    FILE *f = fopen(path, "rb");
    if (f == NULL) return -1;

    int count = 0;
    char buf[4096];
    bool at_line_start = true;
    bool line_is_blank = true;
    bool line_is_comment = false;
    bool first_nonspace_seen = false;

    while (fgets(buf, sizeof(buf), f) != NULL) {
        int i = 0;
        while (buf[i] != '\0') {
            char ch = buf[i];

            if (ch == '\n') {
                /* strip trailing \r if present */
                /* already handled: we check line state */
                if (!first_nonspace_seen) line_is_blank = true;

                bool skip = false;
                if (ignore_blank && line_is_blank) skip = true;
                if (ignore_comment_hash && line_is_comment) skip = true;
                if (!skip) count++;

                /* reset line state */
                at_line_start = true;
                line_is_blank = true;
                line_is_comment = false;
                first_nonspace_seen = false;
            } else {
                if (ch == '\r') { i++; continue; }

                if (!first_nonspace_seen) {
                    if (ch != ' ' && ch != '\t') {
                        first_nonspace_seen = true;
                        line_is_blank = false;
                        if (ch == '#') line_is_comment = true;
                    }
                }
                at_line_start = false;
            }
            i++;
        }
    }

    /* handle file not ending with newline */
    if (first_nonspace_seen || !at_line_start) {
        bool skip = false;
        if (ignore_blank && line_is_blank) skip = true;
        if (ignore_comment_hash && line_is_comment) skip = true;
        if (!skip) count++;
    }

    fclose(f);
    *out_count = count;
    return 0;
}

static void write_test_file(const char *path, const char *content) {
    FILE *f = fopen(path, "wb");
    if (f) { fputs(content, f); fclose(f); }
}

int main(void) {
    int count, ret;
    const char *tmp = "test_lines.txt";

    write_test_file(tmp, "A\nB\nC\n");
    ret = count_lines_filtered(tmp, false, false, &count);
    printf("TC1: ret=%d count=%d (expect ret=0, count=3)\n", ret, count);

    write_test_file(tmp, "A\r\n\r\n#x\r\nB\r\n");
    ret = count_lines_filtered(tmp, true, true, &count);
    printf("TC2: ret=%d count=%d (expect ret=0, count=2)\n", ret, count);

    write_test_file(tmp, " # comment\n data \n");
    ret = count_lines_filtered(tmp, false, true, &count);
    printf("TC3: ret=%d count=%d (expect ret=0, count=1)\n", ret, count);

    /* TC4: very long line */
    {
        FILE *f = fopen(tmp, "wb");
        if (f) {
            for (int i = 0; i < 9000; i++) fputc('X', f);
            fputc('\n', f);
            for (int i = 0; i < 9000; i++) fputc('Y', f);
            fputc('\n', f);
            fclose(f);
        }
    }
    ret = count_lines_filtered(tmp, false, false, &count);
    printf("TC4: ret=%d count=%d (expect ret=0, count=2)\n", ret, count);

    ret = count_lines_filtered("no_such_file.txt", false, false, &count);
    printf("TC5: ret=%d (expect ret=-1)\n", ret);

    ret = count_lines_filtered(tmp, false, false, NULL);
    printf("TC6: ret=%d (expect ret=-1)\n", ret);

    remove(tmp);
    return 0;
}