/* Advent of Code 2025 — Day 1: Secret Entrance
 *
 * WHAT THE PUZZLE ASKS
 * --------------------
 * A safe dial has the numbers 0..99 on it and starts pointing at 50.
 * The input is a list of rotations, one per line, such as
 *
 *     L68      turn left 68 clicks
 *     R48      turn right 48 clicks
 *
 * The dial wraps around: after 99 comes 0 (turning right), and before 0
 * comes 99 (turning left).
 *
 *   part 1: after how many whole rotations does the dial END on 0?
 *   part 2: how many times does the dial POINT AT 0 at any moment while
 *           turning, including the last click of a rotation?
 *
 * Worked example (the --test data): answers 3 and 6.
 *
 * THE MAIN IDEA
 * -------------
 * Simulating every single click is fine here (the input is small), but we
 * can do it with arithmetic instead: from position `pos`, the first click
 * in a direction that lands on 0 is a known distance away, and after that
 * a zero appears every 100 clicks. One division tells us how many zeros a
 * rotation passes through. We still move the dial by the whole rotation at
 * the end to get its new position.
 *
 * A rotation can pass through 0 more than once (for example R250), so
 * part 2 is not just "did it land on 0" — that is part 1.
 *
 * C IDEAS YOU WILL SEE IN THIS FILE
 * ---------------------------------
 *   - `static` on a function or global variable means "private to this
 *     file"; other .c files linked with this one cannot see it.
 *   - `FILE *` is C's handle for an open file. `fgets` reads one line.
 *   - Strings are arrays of `char` ending in a '\0' byte. `strcspn` finds
 *     the first character from a set, which is a neat way to cut off the
 *     newline that `fgets` leaves at the end of a line.
 *   - `strtol` turns the text "68" into the number 68 and tells us where
 *     it stopped reading.
 *   - We pass `&landings` (the address of a variable) to a function so the
 *     function can change the caller's variable. That is what the `*` in
 *     `*landings` means: "the value stored at this address".
 *   - C's `%` operator keeps the sign of the left-hand side: `-5 % 100` is
 *     -5, not 95. `mod()` below fixes that for negative numbers.
 *
 * Build:  cc -O2 -Wall -Wextra -o day01 day01.c
 * Run:    ./day01            (reads ./input.txt)
 *         ./day01 other.txt
 *         ./day01 --test     (worked example: 3 / 6)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START 50        /* the dial's starting position                 */
#define SIZE 100        /* how many numbers are on the dial             */
#define MAX_LINE 256    /* comfortably longer than any rotation line    */

/* Remove the newline (and any stray '\r' from Windows files) at the end
 * of a line read by fgets, so the text is just "L68" and not "L68\n". */
static void strip(char *s)
{
    size_t n = strlen(s);

    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
        s[--n] = '\0';
}

/* Mathematical modulo: the result is always 0..m-1, even for negative a.
 * C's built-in % can return a negative value, which would break the dial. */
static long mod(long a, long m)
{
    long r = a % m;

    return r < 0 ? r + m : r;
}

/* Parse one line such as "L68" into a direction and a distance.
 * Returns 0 on success, -1 if the line does not look like a rotation. */
static int parse_rotation(const char *line, char *dir, long *dist)
{
    char *end;

    if (line[0] != 'L' && line[0] != 'R')
        return -1;
    *dir = line[0];

    /* strtol(text, &end, 10) reads a base-10 number and stores in `end`
     * the address of the first character it did not use. */
    *dist = strtol(line + 1, &end, 10);
    if (end == line + 1 || *dist < 0)
        return -1;             /* no digits, or a negative distance */

    return 0;
}

/* Apply one rotation and update the running counters.
 *
 * `pos` points at the dial position, `landings` counts rotations that end
 * on 0 (part 1), and `crossings` counts every click that lands on 0
 * (part 2). The function changes the caller's variables through pointers. */
static void apply_rotation(long *pos, char dir, long dist,
                           long *landings, long *crossings)
{
    long first;   /* distance from here to the first 0 in this direction */

    if (dir == 'R')
    {
        /* Turning right from 50 reaches 0 after 50 clicks; from 0 it takes
         * a full 100 clicks (the "if (first == 0)" case below).
         * mod(-pos, SIZE) gives exactly that distance. */
        first = mod(-*pos, SIZE);
        if (first == 0)
            first = SIZE;
    }
    else
    {
        /* Turning left from 50 reaches 0 after 50 clicks; from 0 it takes
         * a full 100 clicks. */
        first = *pos % SIZE;
        if (first == 0)
            first = SIZE;
    }

    /* Did this rotation reach at least the first zero? If so we cross it
     * once, then once more every SIZE clicks after that. Integer division
     * counts those extra laps for us. */
    if (first <= dist)
        *crossings += 1 + (dist - first) / SIZE;

    /* Move the dial by the whole rotation, wrapping around the 0..99 ring. */
    if (dir == 'R')
        *pos = mod(*pos + dist, SIZE);
    else
        *pos = mod(*pos - dist, SIZE);

    /* Part 1 only cares about where the rotation finished. */
    if (*pos == 0)
        ++*landings;
}

/* Read rotations from `fp`, compute both answers, and print them.
 * `test` is 1 when we are running the worked example: then we check the
 * expected answers instead of printing puzzle output. */
static int run(FILE *fp, int test)
{
    char line[MAX_LINE];
    long pos = START;
    long landings = 0;
    long crossings = 0;
    long rotations = 0;

    while (fgets(line, sizeof line, fp))
    {
        char dir;
        long dist;

        strip(line);
        if (line[0] == '\0')
            continue;              /* skip blank lines */

        if (parse_rotation(line, &dir, &dist) != 0)
        {
            fprintf(stderr, "bad rotation line: %s\n", line);
            return 1;
        }

        apply_rotation(&pos, dir, dist, &landings, &crossings);
        ++rotations;
    }

    if (test)
    {
        if (landings != 3 || crossings != 6)
        {
            printf("FAIL: expected 3 / 6, got %ld / %ld\n",
                   landings, crossings);
            return 1;
        }
        printf("sample OK: part1=3 part2=6\n");
        return 0;
    }

    printf("rotations: %ld\n", rotations);
    printf("part 1: %ld\n", landings);
    printf("part 2: %ld\n", crossings);
    return 0;
}

int main(int argc, char **argv)
{
    /* The worked example from the puzzle description. */
    static const char *sample =
        "L68\n"
        "L30\n"
        "R48\n"
        "L5\n"
        "R60\n"
        "L55\n"
        "L1\n"
        "L99\n"
        "R14\n"
        "L82\n";

    /* argc is how many command-line words there are; argv[i] is each word.
     * strcmp returns 0 when two strings are equal. */
    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        /* tmpfile() gives us a real FILE * that disappears when closed.
         * Writing the sample there lets --test use exactly the same parser
         * and solver as the real input, so the test exercises real code. */
        FILE *tmp = tmpfile();
        int rc;

        if (!tmp)
        {
            perror("tmpfile");
            return 1;
        }
        fputs(sample, tmp);
        rewind(tmp);               /* move back to the start before reading */
        rc = run(tmp, 1);
        fclose(tmp);
        return rc;
    }

    {
        const char *path = argc > 1 ? argv[1] : "input.txt";
        FILE *fp = fopen(path, "r");
        int rc;

        if (!fp)
        {
            perror(path);          /* prints why fopen failed */
            return 1;
        }
        rc = run(fp, 0);
        fclose(fp);                /* always release files you open */
        return rc;
    }
}
