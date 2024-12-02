use itertools::Itertools;

pub fn part1(inp: String) {
    println!("{}", inp.chars().collect::<String>());
    // convert inp to &[i32]
    let inp = inp
        .chars()
        .map(|c| c.to_digit(10).unwrap() as i32)
        .collect::<Vec<i32>>();

    let (&min,&max) = inp.iter()
    .minmax()
    .into_option()
    .unwrap();
    println!("{}", min);
    println!("{}", max);
}

pub fn part2(inp: String) {
    println!("{}", inp.chars().collect::<String>());
}

fn gauss(n: i32) -> i32 {
    n * (n + 1) / 2
}
