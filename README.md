# Advent of Code

All my Advent of Code work in one repo, organised by year, then language.

Puzzles: <https://adventofcode.com/> — inputs are personal and mostly gitignored;
where a `input.txt` / `puzzle_input.txt` is tracked, it came in with the original
folder.

## Layout

```
2019/  go, python, rust
2020/  python, rust
2021/  rust, python
2022/  rust, python
2023/  rust (fspoettel template), gleam, zig
2024/  C, python, zig, rust
2025/  python (days 01-02), C (days 03-06)
```

| Year | Folders | Notes |
| --- | --- | --- |
| 2019 | `2019/go`, `2019/python`, `2019/rust` | Go days 02–05, Python days 1–5, one Rust file |
| 2020 | `2020/python` | days 01–08 sketches |
| 2021 | `2021/rust`, `2021/python` | Rust has the most complete set |
| 2022 | `2022/rust`, `2022/python` | Rust days, Python notes |
| 2023 | `2023/rust`, `2023/gleam`, `2023/zig/day01` | Rust is the [fspoettel template](https://github.com/fspoettel/advent-of-code-rust) with day 01 solved; Gleam and Zig are day 01 only |
| 2024 | `2024/C`, `2024/python`, `2024/zig`, `2024/rust` | C days 03–04, Python days 03–05, Zig bit-torrent experiments, Rust days 01–02 |
| 2025 | `2025/python/day01`, `2025/python/day02`, `2025/C/day03`..`2025/C/day06` | Day 1 dial; Day 2 repeated-block ranges; Day 3 battery banks (greedy 2/12 digits); Day 4 paper-roll grid; Day 5 ID-range union; Day 6 cephalopod worksheet (rows + vertical columns) |

## Running things

**Python** — each day is standalone:

```sh
cd 2022/python && python3 day01.py
```

**Rust** — one crate per folder:

```sh
cd 2022/rust && cargo run --release
cd 2023/rust && cargo run --release -- 1    # template CLI: download/read/scaffold/solve
cd 2024/rust && cargo run --release
```

**Zig** (0.14+):

```sh
cd 2024/zig && zig build run
cd 2023/zig/day01 && zig build run
```

**Gleam**:

```sh
cd 2023/gleam && gleam test
```

**Go**:

```sh
cd 2019/go/day02 && go run .
```

**C**:

```sh
cd 2024/C && cc -O2 day03.c -o day03 && ./day03
```

## Ignored on purpose

Build output (`target/`, `.zig-cache/`, `zig-out/`, `*.o`, `*.a`, `a.out`, `*.beam`,
`__pycache__`) and most personal puzzle inputs. If you want everything, remove the
matching lines from `.gitignore`.

## History

This repo started life as `haak/advent-of-code-2019` and has been the catch-all
since. On 2026-11-26 it absorbed:

- `~/projects/advent-of-code-2023` (Zig day 01) → `2023/zig/day01`
- `~/projects/github.com/rust-aoc-2023` (Rust template + day 01) → `2023/rust`
- `~/projects/go/advent-of-code` (empty scaffold dirs, nothing to merge)

Gleam and Rust 2024 were moved into the year folders with `git mv`, so their
history is intact. The old `aoc23rust` folder in this repo was a hello-world
stub and was removed (still recoverable from history: `git log -- aoc23rust`).
