pub fn part1(inp: String) {

    let mut lines = inp.lines();


    let mut answer: u64 = 0;
    for line in lines {
        let p1 = line.chars().nth(0).unwrap();
        let p2 = line.chars().nth(2).unwrap();
        answer += match p1 {
            'A' => match p2 {
                'X' => 4,
                'Y' => 8,
                'Z' => 3,
                _ => panic!(),
            },
            'B' => match p2 {
                'X' => 1,
                'Y' => 5,
                'Z' => 9,
                _ => panic!(),
            },
            'C' => match p2 {
                'X' => 7,
                'Y' => 2,
                'Z' => 6,
                _ => panic!(),
            },
            _ => panic!(),
        };
    }

    println!("PART 1: {answer}");


}

pub fn part2(inp: String) {

    let mut lines = inp.lines();

    let mut answer: u64 = 0;
    for line in lines {
        let p1 = line.chars().nth(0).unwrap();
        let p2 = line.chars().nth(2).unwrap();
        answer += match p1 {
            'A' => match p2 {
                'X' => 3,
                'Y' => 4,
                'Z' => 8,
                _ => panic!(),
            },
            'B' => match p2 {
                'X' => 1,
                'Y' => 5,
                'Z' => 9,
                _ => panic!(),
            },
            'C' => match p2 {
                'X' => 2,
                'Y' => 6,
                'Z' => 7,
                _ => panic!(),
            },
            _ => panic!(),
        };
    }

    println!("PART 2: {answer}");
}
