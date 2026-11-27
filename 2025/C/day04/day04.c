/* Advent of Code 2025 — Day 4: Printing Department
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * The input is a grid of characters:
 *
 *     '@'  a roll of paper
 *     '.'  empty floor
 *
 * Eight positions surround each cell (the four sides plus the four corners).
 * A roll can be reached by the forklift when FEWER THAN FOUR of those eight
 * neighbours are rolls.
 *
 *   part 1: how many rolls are reachable right now?
 *   part 2: remove every reachable roll, then repeat with the new grid,
 *           until nothing more can be removed. Count every roll removed.
 *
 * Worked example (--test): 13 and 43.
 *
 * THE MAIN IDEA
 * -------------
 * Part 1 is one pass over the grid. Part 2 is repetition: each removal can
 * make other rolls reachable, so we keep sweeping until a sweep removes
 * nothing.
 *
 * The removals in one sweep must all happen "at the same time": if two rolls
 * are next to each other and both were reachable at the start of the sweep,
 * both are removed. So the sweep marks rolls with 'x' and only turns them
 * into '.' in a second pass — otherwise a roll removed early would change
 * the neighbour counts of rolls later in the same sweep.
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - A 2D array `char grid[ROWS][COLS]` is just ROWS rows laid out one
 *     after another in memory. grid[r][c] is row r, column c.
 *   - Nested `for` loops walk every cell; the bounds checks in `neighbours`
 *     stop us from reading outside the grid.
 *   - `world` arrays are declared `static` at file scope. That gives them a
 *     fixed size at compile time and zero-initialised storage, and avoids
 *     putting a large array on the small call stack.
 *   - `do { ... } while (condition);` runs the body at least once and then
 *     repeats while the condition holds — ideal for "keep going until
 *     nothing changes".
 *
 * Build:  cc -O2 -Wall -Wextra -o day04 day04.c
 * Run:    ./day04            (reads ./input.txt)
 *         ./day04 other.txt
 *         ./day04 --test     (worked example: 13 / 43)
 *         ./day04 --viz      (animated walk through the worked example;
 *                             VIZ_FAST=1 skips the delays)
 */

#include <stdio.h>
#include <string.h>

#include "../viz.h"

#define MAXDIM 600      /* the real grid is smaller than 600 x 600 */

static char grid[MAXDIM][MAXDIM];   /* the whole map, one char per cell */
static int rows;                    /* how many rows the input actually had */
static int cols;                    /* width of the widest row */

/* Read the grid. Returns 0 on success, -1 if the file is empty or too big. */
static int load(FILE *fp)
{
    char line[MAXDIM + 2];   /* +2 leaves room for the newline and '\0' */
    int r = 0;

    rows = cols = 0;
    while (fgets(line, sizeof line, fp))
    {
        int len;

        /* Cut the line at the first '\r' or '\n' by writing '\0' there.
         * strcspn returns the index of that character. */
        line[strcspn(line, "\r\n")] = '\0';
        len = (int)strlen(line);
        if (len == 0)
            continue;
        if (r >= MAXDIM || len >= MAXDIM)
            return -1;              /* would not fit in our array */

        /* Copy this line into row r. memcpy needs a byte count, not a
         * string, so it copies exactly `len` characters. */
        memcpy(grid[r], line, (size_t)len);
        grid[r][len] = '\0';        /* keep each row a valid string */
        if (len > cols)
            cols = len;             /* remember the widest row */
        ++r;
    }
    rows = r;
    return rows > 0 ? 0 : -1;
}

/* Count how many of the 8 surrounding cells contain a roll ('@'). */
static int neighbours(int r, int c)
{
    int dr, dc;
    int count = 0;

    /* dr and dc each take -1, 0, +1, giving all 9 offsets around (r, c).
     * The (0, 0) offset is the cell itself, which we skip. */
    for (dr = -1; dr <= 1; ++dr)
        for (dc = -1; dc <= 1; ++dc)
        {
            int rr, cc;

            if (dr == 0 && dc == 0)
                continue;
            rr = r + dr;
            cc = c + dc;

            /* Do not read outside the grid: rows 0..rows-1, cols 0..cols-1. */
            if (rr < 0 || rr >= rows || cc < 0 || cc >= cols)
                continue;
            if (grid[rr][cc] == '@')
                ++count;
        }
    return count;
}

/* Part 1: one pass, counting rolls that have fewer than 4 roll neighbours. */
static long count_accessible(void)
{
    long n = 0;
    int r, c;

    for (r = 0; r < rows; ++r)
        for (c = 0; c < cols; ++c)
            if (grid[r][c] == '@' && neighbours(r, c) < 4)
                ++n;
    return n;
}

/* Part 2: repeatedly remove reachable rolls, counting the total removed.
 * This MODIFIES the grid, so call it after count_accessible(). */
static long remove_accessible(void)
{
    long total = 0;
    int changed;

    do
    {
        int r, c;

        changed = 0;

        /* First pass: mark every currently-reachable roll with 'x'.
         * We do not turn them into '.' yet, because removals in one sweep
         * are simultaneous; erasing one early would change the neighbour
         * counts for rolls examined later in the same sweep. */
        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == '@' && neighbours(r, c) < 4)
                {
                    grid[r][c] = 'x';
                    ++changed;
                }

        /* Second pass: the marks become empty floor. */
        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == 'x')
                    grid[r][c] = '.';

        total += changed;
    } while (changed > 0);   /* stop when a whole sweep removed nothing */

    return total;
}

static int run(FILE *fp)
{
    long part1, part2;

    if (load(fp) != 0)
        return 1;

    /* Order matters: part 2 destroys the grid, so count part 1 first. */
    part1 = count_accessible();
    part2 = remove_accessible();

    printf("grid: %dx%d\n", rows, cols);
    printf("part 1: %ld\n", part1);
    printf("part 2: %ld\n", part2);
    return 0;
}

/* ---------- animated walk-through (--viz) ------------------------------ */

/* Draw the current grid. 'o' is a roll that became reachable in this sweep
 * (shown inverted); '@' is still a roll; '.' is empty floor. */
static void draw_grid(const char *phase, long reachable, long sweep,
                      long removed)
{
    int r, c;

    viz_clear();
    printf(VIZ_BOLD "Day 4 - Printing Department" VIZ_RESET "\n\n");
    printf(VIZ_CYAN "%s" VIZ_RESET "\n\n", phase);

    for (r = 0; r < rows; ++r)
    {
        printf("  ");
        for (c = 0; c < cols; ++c)
        {
            char ch = grid[r][c];

            if (ch == 'o')
                printf(VIZ_INVERSE VIZ_YELLOW "@" VIZ_RESET);
            else if (ch == '.')
                printf(VIZ_DIM "." VIZ_RESET);
            else
                putchar(ch);
        }
        putchar('\n');
    }

    printf("\n  reachable at the start: " VIZ_CYAN "%ld" VIZ_RESET "\n",
           reachable);
    printf("  sweep: " VIZ_BOLD "%ld" VIZ_RESET
           "   removed so far: " VIZ_BOLD "%ld" VIZ_RESET "\n",
           sweep, removed);
    fflush(stdout);
}

/* Load the sample and replay the removal sweeps, showing each wave. */
static int viz(FILE *fp)
{
    long reachable;
    long sweep = 0;
    long removed = 0;

    if (load(fp) != 0)
        return 1;

    reachable = count_accessible();

    viz_begin();
    draw_grid("initial grid", reachable, 0, 0);
    viz_delay(900);

    for (;;)
    {
        long found = 0;
        int r, c;

        /* Mark this sweep's reachable rolls. */
        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == '@' && neighbours(r, c) < 4)
                {
                    grid[r][c] = 'o';
                    ++found;
                }

        if (found == 0)
        {
            draw_grid("stable: nothing left to remove", reachable, sweep,
                      removed);
            viz_delay(800);
            break;
        }

        ++sweep;
        draw_grid("reachable in this sweep (yellow)", reachable, sweep,
                  removed);
        viz_delay(700);

        /* Remove them all at once, exactly like remove_accessible(). */
        for (r = 0; r < rows; ++r)
            for (c = 0; c < cols; ++c)
                if (grid[r][c] == 'o')
                    grid[r][c] = '.';

        removed += found;
        draw_grid("after the sweep", reachable, sweep, removed);
        viz_delay(700);
    }

    printf(VIZ_BOLD "\n  final: part 1 = %ld, part 2 = %ld\n"
           VIZ_RESET, reachable, removed);
    viz_end();
    return 0;
}

int main(int argc, char **argv)
{
    static const char *sample =
        "..@@.@@@@.\n"
        "@@@.@.@.@@\n"
        "@@@@@.@.@@\n"
        "@.@@@@..@.\n"
        "@@.@@@@.@@\n"
        ".@@@@@@@.@\n"
        ".@.@.@.@@@\n"
        "@.@@@.@@@@\n"
        ".@@@@@@@@.\n"
        "@.@.@@@.@.\n";

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
        long p1, p2;
        int rc;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
        fputs(sample, tmp);
        rewind(tmp);
        rc = load(tmp) == 0 ? 0 : 1;
        if (rc == 0)
        {
            p1 = count_accessible();
            p2 = remove_accessible();
            if (p1 != 13 || p2 != 43)
            {
                printf("FAIL: expected 13 / 43, got %ld / %ld\n", p1, p2);
                rc = 1;
            }
            else
            {
                printf("sample OK: part1=13 part2=43\n");
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
