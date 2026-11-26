use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::path::Path;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<BufReader<File>>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename)?;
    Ok(BufReader::new(file).lines())
}

fn main() -> io::Result<()> {
    // Read lines from the file
    let file_path = "day01.txt";
    let lines = read_lines(file_path)?;

    // Parse the input into left and right lists
    let mut left_numbers: Vec<i32> = Vec::new();
    let mut right_numbers: Vec<i32> = Vec::new();

    // Parse each line
    for line in lines {
        if let Ok(ip) = line {
            // Split the line by whitespace and collect into a vector
            let parts: Vec<&str> = ip.split_whitespace().collect();

            // Parse the first and second parts as i32
            if parts.len() == 2 {
                if let (Ok(left), Ok(right)) = (parts[0].parse(), parts[1].parse()) {
                    left_numbers.push(left);
                    right_numbers.push(right);
                }
            }
        }
    }

    // Sort both lists
    left_numbers.sort();
    right_numbers.sort();

    // Calculate differences and store them in a list
    let differences: Vec<i32> = left_numbers
        .iter()
        .zip(&right_numbers)
        .map(|(left, right)| (*right as i32 - *left as i32).abs())
        .collect();

    // Print sorted lists
    println!("Left numbers: {:?}", left_numbers);
    println!("Right numbers: {:?}", right_numbers);

    // Print differences with calculations
    println!("\nDifferences:");
    for (i, (left, right)) in left_numbers.iter().zip(&right_numbers).enumerate() {
        let diff = differences[i];
        println!("{} - {} = {}", right, left, diff);
    }

    // Print the list of differences
    println!("\nList of Differences: {:?}", differences);

    // Calculate and print the total of differences
    let total_differences: i32 = differences.iter().sum();
    println!("Total of Differences: {}", total_differences);

    Ok(())
}
