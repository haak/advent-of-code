use std::collections::HashMap;
use std::error::Error;
use std::fs::File;
use std::io::{BufRead, BufReader};

fn read_lists_from_file(filename: &str) -> Result<(Vec<i64>, Vec<i64>), Box<dyn Error>> {
    let file = File::open(filename)?;
    let reader = BufReader::new(file);

    let mut left_list = Vec::new();
    let mut right_list = Vec::new();

    for line in reader.lines() {
        let line = line?;
        let parts: Vec<&str> = line.split_whitespace().collect();

        if parts.len() == 2 {
            left_list.push(parts[0].parse()?);
            right_list.push(parts[1].parse()?);
        }
    }

    Ok((left_list, right_list))
}

fn calculate_similarity_score(left_list: &[i64], right_list: &[i64]) -> i64 {
    // Count occurrences of each number in the right list
    let mut right_count = HashMap::new();
    for &num in right_list {
        *right_count.entry(num).or_insert(0) += 1;
    }

    // Calculate similarity score
    let mut similarity_score = 0;
    for &num in left_list {
        // Get the number of times this number appears in the right list
        let count = *right_count.get(&num).unwrap_or(&0);

        // Increase similarity score by num * count
        similarity_score += num * count;
    }

    similarity_score
}

fn main() -> Result<(), Box<dyn Error>> {
    // Read lists from file
    let (left_list, right_list) = read_lists_from_file("day01.txt")?;

    let score = calculate_similarity_score(&left_list, &right_list);
    println!("Similarity Score: {}", score);

    Ok(())
}

// Optional: Add some tests to verify the calculation
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_read_lists() -> Result<(), Box<dyn Error>> {
        let (left_list, right_list) = read_lists_from_file("day01.txt")?;

        assert_eq!(left_list, vec![3, 4, 2, 1, 3, 3]);
        assert_eq!(right_list, vec![3, 3, 5, 3, 9, 3]);

        Ok(())
    }

    #[test]
    fn test_similarity_score() -> Result<(), Box<dyn Error>> {
        let (left_list, right_list) = read_lists_from_file("day01.txt")?;

        assert_eq!(calculate_similarity_score(&left_list, &right_list), 31);

        Ok(())
    }

    #[test]
    fn test_large_numbers() {
        let left_list = vec![1000000, 500000, 250000];
        let right_list = vec![1000000, 1000000, 500000];

        assert_eq!(calculate_similarity_score(&left_list, &right_list), 2000000);
    }
}
