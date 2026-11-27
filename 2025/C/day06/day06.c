/* Advent of Code 2025 — Day 6: Trash Compactor
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * The cephalopods hand you a maths worksheet like this:
 *
 *     123 328  51 64
 *      45 64  387 23
 *       6 98  215 314
 *     *   +   *   +
 *
 * Each "problem" is a group of columns separated from the next problem by a
 * column that is spaces all the way down. The bottom row says which operator
 * to use: '+' or '*' (one per problem).
 *
 *   part 1: read each worksheet ROW as one number (123, 45, 6), then apply
 *           the operator to those numbers. Sum the answers.
 *
 *   part 2: read each COLUMN as one number, top digit to bottom digit
 *           (the first problem's columns spell 1, 24 and 356), then apply
 *           the operator to those numbers. Sum the answers.
 *
 * Worked example (--test): 4277556 and 3263827.
 *
 * THE MAIN IDEA
 * -------------
 * Store every input line as a fixed-width row of characters, padding shorter
 * rows with spaces. Then a "problem" is just a range of column indexes
 * [start, end). Finding those ranges is the only tricky part: a column is a
 * separator when every row has a space there.
 *
 * Once the ranges are known, both parts are the same operation applied to
 * numbers collected in two different directions (along a row vs down a
 * column).
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - `char rows[MAX_ROWS][MAX_COLS + 1]` is a fixed 2D table of characters.
 *     We keep one spare byte per row for the terminating '\0' that makes it
 *     a valid C string.
 *   - `memset` fills a block of memory with one byte. Here it pads a row
 *     with spaces.
 *   - `memcpy(dest, src, n)` copies exactly n bytes (no '\0' checking).
 *   - `strtoull(text, NULL, 10)` turns digit characters into a number and
 *     skips leading spaces for us.
 *   - `unsigned long long` is needed because a product can be 12 digits,
 *     which overflows a 32-bit int.
 *   - `start`/`end` half-open ranges are a common C convention: `end` is the
 *     first index NOT included, so the length is simply `end - start`.
 *
 * Build:  cc -O2 -Wall -Wextra -o day06 day06.c
 * Run:    ./day06            (reads ./input.txt)
 *         ./day06 other.txt
 *         ./day06 --test     (worked example: 4277556 / 3263827)
 *         ./day06 --viz      (animated walk through the worked example;
 *                             VIZ_FAST=1 skips the delays)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../viz.h"

#define MAX_ROWS 16       /* the real input has 5 rows; this is generous */
#define MAX_COLS 8192     /* the real input is 3734 columns wide          */

/* Every input line, padded with spaces to the same width. */
static char rows[MAX_ROWS][MAX_COLS + 1];
static int n_rows;        /* number of lines read (includes operator row) */
static int width;         /* width of the widest line                     */

/* A column is a separator when every row has a space in that column. */
static int is_separator(int c)
{
    int r;

    for (r = 0; r < n_rows; ++r)
        if (rows[r][c] != ' ')
            return 0;
    return 1;
}

/* Read one horizontal slice rows[row][from..to) as a number.
 * Copying the slice into a small buffer lets strtoull parse it: the buffer
 * needs a '\0' at the end, which the slice in the grid does not have. */
static unsigned long long parse_slice(int row, int from, int to)
{
    char buf[MAX_COLS + 1];
    int len = to - from;

    memcpy(buf, rows[row] + from, (size_t)len);
    buf[len] = '\0';

    /* strtoull skips leading whitespace, so a right-aligned number like
     * "   45" parses as 45. */
    return strtoull(buf, NULL, 10);
}

/* Read the vertical slice rows[first..last) in column c as a number.
 * The top digit is the most significant one: reading "1", "2", "3" from
 * top to bottom gives 123. Spaces are ignored, which also ignores the blank
 * leading rows of short numbers. */
static unsigned long long parse_column(int c, int first, int last)
{
    char buf[MAX_ROWS + 1];
    int n = 0;
    int r;

    for (r = first; r < last; ++r)
        if (rows[r][c] >= '0' && rows[r][c] <= '9')
            buf[n++] = rows[r][c];
    buf[n] = '\0';

    return n ? strtoull(buf, NULL, 10) : 0;
}

/* Apply one operator to two numbers. */
static unsigned long long combine(char op, unsigned long long a,
                                  unsigned long long b)
{
    return op == '*' ? a * b : a + b;
}

static void solve(unsigned long long *part1, unsigned long long *part2)
{
    int c = 0;
    /* All rows except the last hold numbers; the last holds the operators. */
    int numeric_rows = n_rows - 1;

    *part1 = 0;
    *part2 = 0;

    while (c < width)
    {
        int start, end, r, i;
        char op = 0;
        unsigned long long p1 = 0, p2 = 0;

        /* Skip separator columns until a problem starts. */
        if (is_separator(c))
        {
            ++c;
            continue;
        }

        /* Now [start, end) is one problem's block of columns. */
        start = c;
        while (c < width && !is_separator(c))
            ++c;
        end = c;

        /* The operator sits somewhere in the bottom row of the block. */
        for (i = start; i < end; ++i)
            if (rows[n_rows - 1][i] == '+' || rows[n_rows - 1][i] == '*')
            {
                op = rows[n_rows - 1][i];
                break;
            }
        if (!op)
            continue;          /* no operator: not a real problem block */

        /* Part 1: one number per row. `first` tells us whether we are
         * starting the running result or combining into it. Without it, a
         * legitimately zero value would look like "nothing yet". */
        {
            int first = 1;

            for (r = 0; r < numeric_rows; ++r)
            {
                int has_digit = 0;

                /* Skip a row that is entirely spaces in this block: some
                 * worksheets have fewer numbers than others. */
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

        /* Part 2: one number per column, read top to bottom. */
        {
            int first = 1;

            for (i = start; i < end; ++i)
            {
                unsigned long long v;
                int has_digit = 0;
                int rr;

                /* A column with no digits anywhere is a gap inside the
                 * block; skip it. */
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

        /* The puzzle wants the sum of the individual problem answers. */
        *part1 += p1;
        *part2 += p2;
    }
}

/* Read every non-empty line into rows[], padding all rows to one width. */
static int load(FILE *fp)
{
    char line[MAX_COLS + 4];

    n_rows = 0;
    width = 0;
    while (fgets(line, sizeof line, fp))
    {
        size_t len;

        /* Drop the newline. */
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        if (n_rows >= MAX_ROWS)
            return -1;

        len = strlen(line);
        if (len > (size_t)MAX_COLS)
            return -1;

        /* Fill the whole row with spaces, then overwrite the start with the
         * actual text. This makes every row the same width, so column scans
         * never run past the end of a short line. */
        memset(rows[n_rows], ' ', (size_t)MAX_COLS);
        memcpy(rows[n_rows], line, len);
        rows[n_rows][MAX_COLS] = '\0';

        if ((int)len > width)
            width = (int)len;      /* remember the widest line */
        ++n_rows;
    }

    if (n_rows == 0)
        return -1;

    /* Only `width` columns are meaningful; end every row there so the rows
     * are proper C strings if someone prints them while debugging. */
    {
        int r;

        for (r = 0; r < n_rows; ++r)
            rows[r][width] = '\0';
    }
    return n_rows >= 2 ? 0 : -1;   /* need at least one number row + operators */
}

/* ---------- animated walk-through (--viz) ------------------------------ */

#define VIZ_MAX_BLOCKS 256
#define VIZ_DELAY_MS 220

static int viz_bstart[VIZ_MAX_BLOCKS];
static int viz_bend[VIZ_MAX_BLOCKS];
static char viz_bop[VIZ_MAX_BLOCKS];
static int viz_nblocks;

/* Same block detection as solve(): a block is a run of non-separator
 * columns, and its operator is the '+' or '*' in the bottom row. */
static void viz_find_blocks(void)
{
    int c = 0;
    int numeric_rows = n_rows - 1;

    viz_nblocks = 0;
    while (c < width && viz_nblocks < VIZ_MAX_BLOCKS)
    {
        int start, end, i;

        if (is_separator(c))
        {
            ++c;
            continue;
        }

        start = c;
        while (c < width && !is_separator(c))
            ++c;
        end = c;

        viz_bstart[viz_nblocks] = start;
        viz_bend[viz_nblocks] = end;
        viz_bop[viz_nblocks] = 0;
        for (i = start; i < end; ++i)
            if (rows[numeric_rows][i] == '+' || rows[numeric_rows][i] == '*')
            {
                viz_bop[viz_nblocks] = rows[numeric_rows][i];
                break;
            }

        if (viz_bop[viz_nblocks] != 0)
            ++viz_nblocks;
    }
}

/* Draw the worksheet with one problem highlighted.
 *   mode 0: highlight the whole problem's columns
 *   mode 1: also highlight row `idx` (part 1 is reading it)
 *   mode 2: also highlight column `idx` (part 2 is reading it)       */
static void draw_ws(int bs, int be, int mode, int idx,
                    unsigned long long total1, unsigned long long total2,
                    unsigned long long block_ans, char op, int block_no,
                    int nblocks, const char *phase)
{
    int r, c;

    viz_clear();
    printf(VIZ_BOLD "Day 6 - Trash Compactor" VIZ_RESET "\n\n");
    printf(VIZ_CYAN "%s" VIZ_RESET "\n\n", phase);

    for (r = 0; r < n_rows; ++r)
    {
        printf("  ");
        for (c = 0; c < width; ++c)
        {
            char ch = rows[r][c];
            int in_block = c >= bs && c < be;
            int hi = in_block &&
                     ((mode == 1 && r == idx) || (mode == 2 && c == idx));

            if (hi)
                printf(VIZ_INVERSE VIZ_YELLOW "%c" VIZ_RESET, ch);
            else if (ch == '+' || ch == '*')
                printf(VIZ_MAGENTA "%c" VIZ_RESET, ch);
            else if (in_block)
                putchar(ch);
            else
                printf(VIZ_DIM "%c" VIZ_RESET, ch);
        }
        putchar('\n');
    }

    printf("\n  problem %d/%d   operator '" VIZ_MAGENTA "%c" VIZ_RESET
           "'   columns %d..%d\n", block_no + 1, nblocks, op, bs, be - 1);

    if (mode == 1)
        printf("  reading row %d    -> " VIZ_BOLD "%llu" VIZ_RESET "\n",
               idx, parse_slice(idx, bs, be));
    else if (mode == 2)
        printf("  reading column %d -> " VIZ_BOLD "%llu" VIZ_RESET "\n",
               idx, parse_column(idx, 0, n_rows - 1));

    printf("  this problem: " VIZ_BOLD "%llu" VIZ_RESET "\n", block_ans);
    printf("  totals: part 1 = " VIZ_BOLD "%llu" VIZ_RESET
           "   part 2 = " VIZ_BOLD "%llu" VIZ_RESET "\n", total1, total2);
    fflush(stdout);
}

/* Replay both interpretations of every problem, one number at a time. */
static int viz(FILE *fp)
{
    unsigned long long total1 = 0, total2 = 0;
    int b;

    if (load(fp) != 0)
        return 1;
    viz_find_blocks();

    viz_begin();

    for (b = 0; b < viz_nblocks; ++b)
    {
        int bs = viz_bstart[b];
        int be = viz_bend[b];
        char op = viz_bop[b];
        unsigned long long p1 = 0, p2 = 0;
        int r, i, first;

        /* Part 1: one number per row. */
        draw_ws(bs, be, 0, -1, total1, total2, 0, op, b, viz_nblocks,
                "part 1 - one number per row");
        viz_delay(400);

        first = 1;
        for (r = 0; r < n_rows - 1; ++r)
        {
            int has = 0;

            for (i = bs; i < be; ++i)
                if (rows[r][i] >= '0' && rows[r][i] <= '9')
                {
                    has = 1;
                    break;
                }
            if (!has)
                continue;

            {
                unsigned long long v = parse_slice(r, bs, be);

                p1 = first ? v : combine(op, p1, v);
                first = 0;
            }

            draw_ws(bs, be, 1, r, total1, total2, p1, op, b, viz_nblocks,
                    "part 1 - one number per row");
            viz_delay(VIZ_DELAY_MS);
        }

        /* Part 2: one number per column, top to bottom. */
        draw_ws(bs, be, 0, -1, total1, total2, p2, op, b, viz_nblocks,
                "part 2 - one number per column");
        viz_delay(400);

        first = 1;
        for (i = bs; i < be; ++i)
        {
            int has = 0;

            for (r = 0; r < n_rows - 1; ++r)
                if (rows[r][i] >= '0' && rows[r][i] <= '9')
                {
                    has = 1;
                    break;
                }
            if (!has)
                continue;

            {
                unsigned long long v = parse_column(i, 0, n_rows - 1);

                p2 = first ? v : combine(op, p2, v);
                first = 0;
            }

            draw_ws(bs, be, 2, i, total1, total2, p2, op, b, viz_nblocks,
                    "part 2 - one number per column");
            viz_delay(VIZ_DELAY_MS);
        }

        total1 += p1;
        total2 += p2;
        draw_ws(bs, be, 0, -1, total1, total2, p1, op, b, viz_nblocks,
                "problem finished");
        viz_delay(500);
    }

    printf(VIZ_BOLD "\n  final: part 1 = %llu, part 2 = %llu\n"
           VIZ_RESET, total1, total2);
    viz_end();
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "123 328  51 64 \n"
        " 45 64  387 23 \n"
        "  6 98  215 314\n"
        "*   +   *   +  \n";

    if (argc > 1 && strcmp(argv[1], "--viz") == 0)
    {
        FILE *tmp = tmpfile();
        int rc;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
        fputs(sample, tmp);
        rewind(tmp);
        rc = viz(tmp);
        fclose(tmp);
        return rc;
    }

    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        FILE *tmp = tmpfile();
        unsigned long long p1 = 0, p2 = 0;
        int rc = 1;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
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
