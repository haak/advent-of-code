/* Advent of Code 2025 — Day 6: Trash Compactor
 *
 * Cephalopod math worksheet: columns of numbers, one operator per problem,
 * problems separated by a full column of spaces.
 *
 *   part 1: each worksheet row is a number; apply the operator across rows.
 *   part 2: each column of digits is a number (read top to bottom); apply the
 *           operator across those numbers.
 *
 * Values fit in 64 bits (largest product in the real input is 12 digits).
 *
 * Build:  cc -O2 -Wall -Wextra -o day06 day06.c
 * Run:    ./day06            (reads ./input.txt)
 *         ./day06 other.txt
 *         ./day06 --test     (worked example: 4277556 / 3263827)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 16
#define MAX_COLS 8192

static char rows[MAX_ROWS][MAX_COLS + 1];
static int n_rows;
static int width;

/* Returns 1 if this problem column is blank in every row. */
static int is_separator(int c)
{
    int r;

    for (r = 0; r < n_rows; ++r)
        if (rows[r][c] != ' ')
            return 0;
    return 1;
}

static unsigned long long parse_slice(int row, int from, int to)
{
    char buf[MAX_COLS + 1];
    int len = to - from;

    memcpy(buf, rows[row] + from, (size_t)len);
    buf[len] = '\0';

    /* strtoull skips leading whitespace; trailing whitespace is harmless. */
    return strtoull(buf, NULL, 10);
}

static unsigned long long parse_column(int c, int first, int last)
{
    char buf[MAX_ROWS + 1];
    int n = 0;
    int r;

    /* Numbers are written vertically: top digit first. */
    for (r = first; r < last; ++r)
        if (rows[r][c] >= '0' && rows[r][c] <= '9')
            buf[n++] = rows[r][c];
    buf[n] = '\0';

    return n ? strtoull(buf, NULL, 10) : 0;
}

static unsigned long long combine(char op, unsigned long long a,
                                 unsigned long long b)
{
    return op == '*' ? a * b : a + b;
}

static void solve(unsigned long long *part1, unsigned long long *part2)
{
    int c = 0;
    int numeric_rows = n_rows - 1; /* last row holds the operators */

    *part1 = 0;
    *part2 = 0;

    while (c < width)
    {
        int start, end, r, i;
        char op = 0;
        unsigned long long p1 = 0, p2 = 0;

        if (is_separator(c))
        {
            ++c;
            continue;
        }

        start = c;
        while (c < width && !is_separator(c))
            ++c;
        end = c; /* exclusive */

        for (i = start; i < end; ++i)
            if (rows[n_rows - 1][i] == '+' || rows[n_rows - 1][i] == '*')
            {
                op = rows[n_rows - 1][i];
                break;
            }
        if (!op)
            continue;

        /* part 1: one number per row */
        {
            int first = 1;

            for (r = 0; r < numeric_rows; ++r)
            {
                int has_digit = 0;

                for (i = start; i < end; ++i)
                    if (rows[r][i] >= '0' && rows[r][i] <= '9')
                    {
                        has_digit = 1;
                        break;
                    }
                if (!has_digit)
                    continue;

                {
                    unsigned long long v = parse_slice(r, start, end);

                    p1 = first ? v : combine(op, p1, v);
                    first = 0;
                }
            }
        }

        /* part 2: one number per column */
        {
            int first = 1;

            for (i = start; i < end; ++i)
            {
                unsigned long long v;
                int has_digit = 0;
                int rr;

                for (rr = 0; rr < numeric_rows; ++rr)
                    if (rows[rr][i] >= '0' && rows[rr][i] <= '9')
                    {
                        has_digit = 1;
                        break;
                    }
                if (!has_digit)
                    continue;

                v = parse_column(i, 0, numeric_rows);
                p2 = first ? v : combine(op, p2, v);
                first = 0;
            }
        }

        *part1 += p1;
        *part2 += p2;
    }
}

static int load(FILE *fp)
{
    char line[MAX_COLS + 4];

    n_rows = 0;
    width = 0;
    while (fgets(line, sizeof line, fp))
    {
        size_t len;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        if (n_rows >= MAX_ROWS)
            return -1;

        len = strlen(line);
        if (len > (size_t)MAX_COLS)
            return -1;
        memset(rows[n_rows], ' ', (size_t)MAX_COLS);
        memcpy(rows[n_rows], line, len);
        rows[n_rows][MAX_COLS] = '\0';
        if ((int)len > width)
            width = (int)len;
        ++n_rows;
    }

    /* Pad every row to the same width so column scans are safe. */
    if (n_rows > 0)
    {
        int r;

        for (r = 0; r < n_rows; ++r)
            rows[r][width] = '\0';
    }
    return n_rows >= 2 ? 0 : -1;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "123 328  51 64 \n"
        " 45 64  387 23 \n"
        "  6 98  215 314\n"
        "*   +   *   +  \n";

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        FILE *tmp = tmpfile();
        unsigned long long p1 = 0, p2 = 0;
        int rc = 1;

        if (!tmp)
            return 1;
        fputs(sample, tmp);
        rewind(tmp);
        if (load(tmp) == 0)
        {
            solve(&p1, &p2);
            if (p1 == 4277556ULL && p2 == 3263827ULL)
            {
                printf("sample OK: part1=4277556 part2=3263827\n");
                rc = 0;
            }
            else
            {
                printf("FAIL: expected 4277556 / 3263827, got %llu / %llu\n",
                       p1, p2);
            }
        }
        fclose(tmp);
        return rc;
    }

    {
        const char *path = argc > 1 ? argv[1] : "input.txt";
        FILE *fp = fopen(path, "r");
        unsigned long long p1 = 0, p2 = 0;

        if (!fp)
        {
            perror(path);
            return 1;
        }
        if (load(fp) != 0)
        {
            fclose(fp);
            fprintf(stderr, "bad input\n");
            return 1;
        }
        fclose(fp);
        solve(&p1, &p2);
        printf("grid: %dx%d\n", width, n_rows);
        printf("part 1: %llu\n", p1);
        printf("part 2: %llu\n", p2);
    }
    return 0;
}
