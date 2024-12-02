pub fn part1(inp: String) {
	// println!("{}", inp.chars().rev().collect::<String>());
	let mut depth = 0;
	let mut hpos = 0;
	let inp_list: Vec<String> = inp.lines().map(str::parse).map(Result::unwrap).collect();
	for i in 0..inp_list.len() {
		// println!("{}", inp_list[i]);
		let split = inp_list[i].split(" ");
		let vec = split.collect::<Vec<&str>>();
		// println!("{}", vec[0]);
		if vec[0] == "forward"{
			// println!("{}", vec[1]);
			hpos += vec[1].parse::<i32>().unwrap();
			
		} else if vec[0] == "up" {
			// println!("{}", vec[1]);
			depth -= vec[1].parse::<i32>().unwrap();
			
		} else if vec[0] == "down" {
			depth += vec[1].parse::<i32>().unwrap();
			// println!("{}", vec[1]);
		}
		// println!("hpos: {} depth: {} ", hpos.abs() ,depth.abs());
	    }
	    
	    println!("hpos * depth: {}", hpos * depth);
	}


pub fn part2(inp: String) {
	// let mut inp_list: Vec<String> = inp.lines().map(str::parse).map(Result::unwrap).collect();
	let mut depth = 0;
	let mut hpos = 0;
	let mut aim = 0;
	let inp_list: Vec<String> = inp.lines().map(str::parse).map(Result::unwrap).collect();
	for i in 0..inp_list.len() {
		// println!("{}", inp_list[i]);
		let split = inp_list[i].split(" ");
		let vec = split.collect::<Vec<&str>>();
		// println!("{}", vec[0]);
		if vec[0] == "forward"{
			hpos += vec[1].parse::<i32>().unwrap();
			depth += aim * vec[1].parse::<i32>().unwrap();
			// println!("{}", vec[1]);
			
		} else if vec[0] == "up" {
			aim -= vec[1].parse::<i32>().unwrap();
			// println!("{}", vec[1]);
			// depth -= vec[1].parse::<i32>().unwrap();
			
		} else if vec[0] == "down" {
			aim += vec[1].parse::<i32>().unwrap();
			// depth += vec[1].parse::<i32>().unwrap();
			// println!("{}", vec[1]);
		}
		// println!("hpos: {} depth: {} aim: {} ", hpos.abs() ,depth.abs(), aim);
	    }
	
	    println!("hpos * depth: {}", hpos * depth);
	}
