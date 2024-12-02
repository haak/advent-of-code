pub fn part1(inp: String) {
    let mut count = 0;
    // println!("{}", count);
    // parse inp as list of ints separate on new lines
    let inp_list: Vec<i32> = inp.lines().map(str::parse).map(Result::unwrap).collect();
    // println!("{:?}", inp_list);
    //loop through list and compare each one to the next one if bigger add 1 to count
    for i in 0..inp_list.len() - 1 {
        if inp_list[i + 1] > inp_list[i] {
            count += 1;
        }
    }
    println!("{}", count);
}

pub fn part2(inp: String) {
    let inp_list: Vec<i32> = inp.lines().map(str::parse).map(Result::unwrap).collect();
    let mut count = 0;
    for i in 0..inp_list.len() - 3 {
        let a = inp_list[i];
        let b = inp_list[i + 1];
        let c = inp_list[i + 2];
        let d = inp_list[i + 3];
        if b + c + d > a + b + c {
            // println!("{} : {}", a + b + c, b + c + d);
            count += 1;
        }
        
        // if inp_list[i+1] > inp_list[i] {
        //     count += 1;
        // }
    }
    println!("{}", count);
}

