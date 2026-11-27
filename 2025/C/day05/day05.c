/* Advent of Code 2025 — Day 5: Cafeteria
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * The input has two sections separated by a blank line:
 *
 *     3-5
 *     10-14
 *     ...
 *     <blank line>
 *     1
 *     5
 *     ...
 *
 * The first section lists ranges of "fresh" ingredient IDs: "3-5" means
 * IDs 3, 4 and 5 are all fresh. The second section lists the ingredient IDs
 * that are actually available.
 *
 *   part 1: how many of the available IDs fall in at least one range?
 *   part 2: how many IDs are fresh in total? That is the size of the UNION
 *           of all the ranges — each ID counted once even if several ranges
 *           cover it.
 *
 * Worked example (--test): 3 and 14.
 *
 * THE MAIN IDEA
 * -------------
 * Part 1 is a simple lookup: for each available ID, check the ranges.
 *
 * Part 2 must not loop over every ID: the ranges reach about 5 * 10^14, so
 * counting one by one would take days. Instead, sort the ranges by their
 * start, then walk through them merging any that overlap or touch. The
 * merged ranges are all disjoint, so the answer is just the sum of their
 * lengths (hi - lo + 1).
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - `typedef struct { ... } Range;` creates a new type that bundles two
 *     numbers. `ranges[i].lo` reads a field.
 *   - `qsort` sorts an array using a comparison function we supply. The
 *     function receives `const void *` pointers, which we cast back to
 *     `Range *`.
 *   - `sscanf(line, "%llu-%llu", &lo, &hi)` parses two unsigned long long
 *     values out of text; `strtoull` would work too.
 *   - `FILE *`, `fgets`, `tmpfile` and `rewind` work as in the other days.
 *   - Integer types matter: an `unsigned long long` holds up to about
 *     1.8 * 10^19, which covers these IDs; a 32-bit `int` would not.
 *
 * Build:  cc -O2 -Wall -Wextra -o day05 day05.c
 * Run:    ./day05            (reads ./input.txt)
 *         ./day05 other.txt
 *         ./day05 --test     (worked example: 3 / 14)
 *         ./day05 --viz      (animated walk through the worked example;
 *                             VIZ_FAST=1 skips the delays)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../viz.h"

#define MAX_RANGES 4096
#define MAX_IDS 100000

/* A fresh-ID range: every number from lo to hi, inclusive. */
typedef struct
{
    unsigned long long lo;
    unsigned long long hi;
} Range;

static Range ranges[MAX_RANGES];
static int n_ranges;
static unsigned long long ids[MAX_IDS];
static int n_ids;

/* Read both sections. Returns 0 on success, -1 on bad input. */
static int load(FILE *fp)
{
    char line[256];
    int in_ids = 0;      /* 0 = reading ranges, 1 = reading IDs */

    n_ranges = n_ids = 0;
    while (fgets(line, sizeof line, fp))
    {
        unsigned long long lo, hi;

        /* Cut off the trailing newline. */
        line[strcspn(line, "\r\n")] = '\0';

        /* A blank line ends the range section and starts the ID section. */
        if (line[0] == '\0')
        {
            in_ids = 1;
            continue;
        }

        if (!in_ids)
        {
            /* "%llu-%llu" means: an unsigned long long, a literal '-', and
             * another unsigned long long. sscanf returns how many values it
             * managed to read. */
            if (sscanf(line, "%llu-%llu", &lo, &hi) != 2)
                return -1;
            if (lo > hi || n_ranges >= MAX_RANGES)
                return -1;
            ranges[n_ranges].lo = lo;
            ranges[n_ranges].hi = hi;
            ++n_ranges;
        }
        else
        {
            if (n_ids >= MAX_IDS)
                return -1;
            /* strtoull parses a decimal number; NULL means "we do not care
             * where parsing stopped". */
            ids[n_ids++] = strtoull(line, NULL, 10);
        }
    }
    return n_ranges > 0 ? 0 : -1;
}

/* qsort comparison for ranges: order by start, lowest first.
 * Returning negative / zero / positive means a < b / a == b / a > b. */
static int by_start(const void *a, const void *b)
{
    const Range *ra = a;
    const Range *rb = b;

    if (ra->lo < rb->lo)
        return -1;
    if (ra->lo > rb->lo)
        return 1;
    return 0;
}

/* Part 1: count IDs that are inside at least one range. */
static long count_fresh_ids(void)
{
    long count = 0;
    int i, j;

    for (i = 0; i < n_ids; ++i)
        for (j = 0; j < n_ranges; ++j)
            if (ids[i] >= ranges[j].lo && ids[i] <= ranges[j].hi)
            {
                ++count;      /* count this ID once */
                break;        /* no need to look at further ranges */
            }
    return count;
}

/* Part 2: total size of the union of all ranges. */
static unsigned long long count_union(void)
{
    unsigned long long total = 0;
    unsigned long long cur_lo, cur_hi;
    int i;

    /* Sorting by start means a range can only overlap the current merged
     * block, never one we have already finished. */
    qsort(ranges, (size_t)n_ranges, sizeof ranges[0], by_start);

    cur_lo = ranges[0].lo;
    cur_hi = ranges[0].hi;

    for (i = 1; i < n_ranges; ++i)
    {
        /* Merge overlapping AND adjacent ranges: 3-5 followed by 6-8
         * covers 3..8 with no gap, so it is one block.
         * `cur_hi + 1` is safe because the IDs are far below the maximum
         * value of unsigned long long, so this cannot wrap around. */
        if (ranges[i].lo <= cur_hi + 1)
        {
            if (ranges[i].hi > cur_hi)
                cur_hi = ranges[i].hi;
        }
        else
        {
            /* The next range starts after a genuine gap: close the current
             * block and start a new one. hi - lo + 1 counts inclusive IDs. */
            total += cur_hi - cur_lo + 1;
            cur_lo = ranges[i].lo;
            cur_hi = ranges[i].hi;
        }
    }
    /* The last block is closed after the loop. */
    total += cur_hi - cur_lo + 1;
    return total;
}

static int run(FILE *fp)
{
    if (load(fp) != 0)
        return 1;
    printf("ranges: %d  ids: %d\n", n_ranges, n_ids);
    printf("part 1: %ld\n", count_fresh_ids());
    printf("part 2: %llu\n", count_union());
    return 0;
}

/* ---------- animated walk-through (--viz) ------------------------------ */

#define VIZ_W 50
#define VIZ_DELAY_MS 420

/* Map an ID onto a column of the display. The sample IDs are small, so a
 * plain linear scale is readable here. `maxv` is the largest number shown. */
static int viz_x5(unsigned long long v, unsigned long long maxv)
{
    unsigned long long x;

    if (maxv == 0)
        return 0;
    x = v * (VIZ_W - 1) / maxv;
    if (x >= VIZ_W)
        x = VIZ_W - 1;
    return (int)x;
}

/* Draw the sorted ranges, the block currently being merged, the union size
 * so far, and a row of marks for the available IDs. */
static void draw_cafe(Range *rs, int n, int upto,
                      unsigned long long cur_lo, unsigned long long cur_hi,
                      unsigned long long maxv, unsigned long long total,
                      const char *marks, long fresh, long checked,
                      const char *phase)
{
    char bar[VIZ_W + 1];
    int i, j;

    viz_clear();
    printf(VIZ_BOLD "Day 5 - Cafeteria" VIZ_RESET "\n");
    printf(VIZ_CYAN "%s" VIZ_RESET "\n\n", phase);

    for (i = 0; i < n; ++i)
    {
        int a = viz_x5(rs[i].lo, maxv);
        int b = viz_x5(rs[i].hi, maxv);

        if (b - a < 2)
        {
            b = a + 2;
            if (b >= VIZ_W)
            {
                b = VIZ_W - 1;
                a = b - 2;
            }
        }

        for (j = 0; j < VIZ_W; ++j)
            bar[j] = ' ';
        bar[VIZ_W] = '\0';
        for (j = a; j <= b; ++j)
            bar[j] = '=';
        bar[a] = '[';
        bar[b] = ']';

        printf("%2d %8llu-%-8llu %s%s" VIZ_RESET "\n", i, rs[i].lo,
               rs[i].hi, i <= upto ? VIZ_GREEN : VIZ_DIM, bar);
    }

    if (cur_hi >= cur_lo)
        printf("\n  merging: %llu-%llu  (size %llu)\n", cur_lo, cur_hi,
               cur_hi - cur_lo + 1);
    else
        printf("\n  merging: (none)\n");
    printf("  union so far: " VIZ_BOLD "%llu" VIZ_RESET "\n", total);

    printf("\n%25s", "");
    for (j = 0; j < VIZ_W; ++j)
    {
        if (marks[j] == 'o')
            printf(VIZ_GREEN "o");
        else if (marks[j] == 'x')
            printf(VIZ_RED "x");
        else
            putchar(' ');
    }
    printf(VIZ_RESET "\n  available IDs checked: " VIZ_BOLD "%ld" VIZ_RESET
           "   fresh: " VIZ_BOLD "%ld" VIZ_RESET "\n", checked, fresh);
    fflush(stdout);
}

/* Load the sample, animate the merge, then test each available ID. */
static int viz(FILE *fp)
{
    unsigned long long maxv = 0;
    unsigned long long total = 0;
    unsigned long long cur_lo, cur_hi;
    char marks[VIZ_W + 1];
    long fresh = 0;
    int i, j;

    if (load(fp) != 0)
        return 1;

    for (i = 0; i < n_ranges; ++i)
        if (ranges[i].hi > maxv)
            maxv = ranges[i].hi;
    for (i = 0; i < n_ids; ++i)
        if (ids[i] > maxv)
            maxv = ids[i];

    viz_begin();

    /* Part 2: sort, then merge one range at a time. */
    qsort(ranges, (size_t)n_ranges, sizeof ranges[0], by_start);
    memset(marks, ' ', VIZ_W);
    marks[VIZ_W] = '\0';

    cur_lo = ranges[0].lo;
    cur_hi = ranges[0].hi;
    draw_cafe(ranges, n_ranges, 0, cur_lo, cur_hi, maxv, total,
              marks, fresh, 0, "part 2 - sort the ranges, then merge");
    viz_delay(VIZ_DELAY_MS);

    for (i = 1; i < n_ranges; ++i)
    {
        char phase[128];
        int joins = ranges[i].lo <= cur_hi + 1;

        if (joins)
        {
            if (ranges[i].hi > cur_hi)
                cur_hi = ranges[i].hi;
        }
        else
        {
            total += cur_hi - cur_lo + 1;
            cur_lo = ranges[i].lo;
            cur_hi = ranges[i].hi;
        }

        snprintf(phase, sizeof phase, "%llu-%llu %s the current block",
                 ranges[i].lo, ranges[i].hi,
                 joins ? "joins" : "starts a new block");
        draw_cafe(ranges, n_ranges, i, cur_lo, cur_hi, maxv, total,
                  marks, fresh, 0, phase);
        viz_delay(VIZ_DELAY_MS);
    }

    total += cur_hi - cur_lo + 1;
    draw_cafe(ranges, n_ranges, n_ranges - 1, cur_lo, cur_hi, maxv, total,
              marks, fresh, 0, "part 2 - all ranges merged");
    viz_delay(800);

    /* Part 1: check every available ID against the ranges. */
    draw_cafe(ranges, n_ranges, n_ranges - 1, 1, 0, maxv, total,
              marks, fresh, 0, "part 1 - check each available ID");
    viz_delay(600);

    for (i = 0; i < n_ids; ++i)
    {
        char phase[128];
        int is_fresh = 0;

        for (j = 0; j < n_ranges; ++j)
            if (ids[i] >= ranges[j].lo && ids[i] <= ranges[j].hi)
            {
                is_fresh = 1;
                break;
            }

        marks[viz_x5(ids[i], maxv)] = is_fresh ? 'o' : 'x';
        if (is_fresh)
            ++fresh;

        snprintf(phase, sizeof phase, "ID %llu is %s", ids[i],
                 is_fresh ? "fresh (green)" : "spoiled (red)");
        draw_cafe(ranges, n_ranges, n_ranges - 1, 1, 0, maxv, total,
                  marks, fresh, i + 1, phase);
        viz_delay(VIZ_DELAY_MS);
    }

    printf(VIZ_BOLD "\n  final: part 1 = %ld, part 2 = %llu\n"
           VIZ_RESET, fresh, total);
    viz_end();
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "3-5\n"
        "10-14\n"
        "16-20\n"
        "12-18\n"
        "\n"
        "1\n"
        "5\n"
        "8\n"
        "11\n"
        "17\n"
        "32\n";

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
        /* Feed the sample through a temporary file so load() runs too. */
        FILE *tmp = tmpfile();
        long p1;
        unsigned long long p2;
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
            p1 = count_fresh_ids();
            p2 = count_union();
            if (p1 == 3 && p2 == 14)
            {
                printf("sample OK: part1=3 part2=14\n");
                rc = 0;
            }
            else
            {
                printf("FAIL: expected 3 / 14, got %ld / %llu\n", p1, p2);
            }
        }
        fclose(tmp);
        return rc;
    }

    {
        const char *path = argc > 1 ? argv[1] : "input.txt";
        FILE *fp = fopen(path, "r");
        int rc;

        if (!fp)
        {
            perror(path);
            return 1;
        }
        rc = run(fp);
        fclose(fp);
        return rc;
    }
}
