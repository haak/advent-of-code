use std::fs::File;
use std::io::{BufRead, BufReader};
use std::path::Path;

fn main() {
    part1().unwrap();
    // println!("Hello, world!");
}

fn part1() -> Result<(), Box<dyn std::error::Error>> {
    // load text from file as two lists, left items in one list and right items in another list
    // sort both lists
    // compare each item from both lists, subtract one from the other and add to a total
    // print total

    // Read items from file
    let path = Path::new("test.txt");
    let file = File::open(path)?;
    let reader = BufReader::new(file);

    // Parse lines into left and right lists
    let mut left_items: Vec<i32> = Vec::new();
    let mut right_items: Vec<i32> = Vec::new();

    for line in reader.lines() {
        let line = line?;
        let parts: Vec<&str> = line.split('\t').collect();

        if parts.len() == 2 {
            left_items.push(parts[0].trim().parse()?);
            right_items.push(parts[1].trim().parse()?);
        } else {
            eprintln!("Skipping invalid line: {}", line);
        }
    }

    // Sort both lists
    left_items.sort();
    right_items.sort();

    // Compare and calculate total difference
    let total_difference: i32 = left_items
        .iter()
        .zip(right_items.iter())
        .map(|(left, right)| left - right)
        .sum();

    // Print total difference
    println!("Total difference: {}", total_difference);

    Ok(())
}
