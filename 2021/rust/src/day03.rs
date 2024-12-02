#[derive(Debug)]
struct Matrix(f32, f32);

pub fn part1(inp: String) {
    // println!("{}", inp.chars().collect::<String>());
    // loop through all the lines
    // for each digit in the line add a tuple of count of 0 or 1
    let mut gamma_bin = "".to_string();
    let mut epsilon_bin = "".to_string();

    let inp_list: Vec<String> = inp.lines().map(str::parse).map(Result::unwrap).collect();
    let vec = bit_list(inp_list);

    for i in 0..vec.len() {
        // println!("{}", vec[i].0 * vec[i].1);
        if vec[i].0 > vec[i].1 {
            gamma_bin.push('1');
            epsilon_bin.push('0');
        } else {
            gamma_bin.push('0');
            epsilon_bin.push('1');
        }
    }
    // let bin_idx = "01110011001";
    let gamma = isize::from_str_radix(&gamma_bin, 2).unwrap();
    let epsilon = isize::from_str_radix(&epsilon_bin, 2).unwrap();
    println!(
        "gamma: {} episilon: {} ans: {} ",
        gamma,
        epsilon,
        gamma * epsilon
    );
}

pub fn bit_list(input: Vec<String>) -> Vec<(f32, f32)> {
    let mut vec: Vec<(f32, f32)> = vec![];
    for i in 0..input.len() {
        if i == 0 {
            for j in 0..input[i].len() {
                if input[i].chars().nth(j).unwrap() == '0' {
                    vec.push((1.0, 0.0));
                } else {
                    vec.push((0.0, 1.0));
                }
            }
        } else {
            for j in 0..input[i].len() {
                // for each digit in the line
                if input[i].chars().nth(j).unwrap() == '0' {
                    // if the digit is 0
                    vec[j] = (vec[j].0 + 1.0, vec[j].1); // add 1 to the count of 0s
                } else {
                    // if the digit is 1
                    vec[j] = (vec[j].0, vec[j].1 + 1.0); // add 1 to the count of 1s
                }
            }

            // create list of tuples that is the length of the line
            // let mut list: Vec<(usize, usize)> = Vec::new();
        }
        // println!("{:?}", vec);
    }
    return vec;
}

pub fn part2(inp: String) {
    let input = inp.lines()
    .map(|l| u32::from_str_radix(l, 2).unwrap())
    .collect::<Vec<_>>();
//   let p1 = part1(&input);
  let p2 = part2help(&input, 1) * part2help(&input, 0);
    println!("{}", p2);
    
}


fn part2help(nums: &[u32], oxygen: u32) -> u32 {
    let mut nums = nums.to_vec();
    for i in (0..12).rev() {
      let keep = max_bit(&nums, i) ^ oxygen;
      nums.retain(|x| (x>>i) & 1 == keep);
      if nums.len() == 1 { break }
    }
    nums[0]
  }

fn max_bit(nums: &[u32], bit: usize) -> u32 {
    let mut c = [0, 0];
    for &x in nums {
        c[(x as usize >> bit) & 1] += 1
    }
    (c[1] >= c[0]) as u32
}
