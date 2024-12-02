use itertools::Itertools;

pub fn part1(inp: String) {
    let totals = get_total(inp);
    println!("Part 1: {}", totals[0]);
}

pub fn part2(inp: String) {
    let totals = get_total(inp);
    println!("Part 2: {}", totals.iter().take(3).sum::<u64>());

}

pub fn get_total(inp: String) -> Vec<u64> {
    let totals: Vec<u64> = inp
        .split("\n\n")
        .map(|elf| {
            elf.split('\n')
                .map(|food| food.parse::<u64>().unwrap_or(0)).sum()
        })
        .sorted().rev().collect();
    return totals;

}