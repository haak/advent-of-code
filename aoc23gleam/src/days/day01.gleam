import gleam/io
import gleam/string
import gleam/int
import gleam/list
import simplifile

// import ext/resultx as resx

pub fn run() {
  let filepath = "./src/days/01demo.txt"
  //  io.println()
  let input = read_file(filepath)
  solvep1(input)
  |> io.debug
}

fn solvep1(input: String) -> Int {
  // for loop through each line and combine first and last number to a number then add to existing number
  // input
  let out =
    string.split(input, on: "\n")
    |> io.debug
    |> get_int_from_list
  // |> io.debug

  0
}

fn get_int_from_list(list: List(String)) -> List(String) {
  case list {
    [x, ..rest] -> string.to_graphemes(x)

    [] -> []
  }
  // for each item in list, take the first and last number and combine them to a number
  // then add to a all together.
  // case list {
  //   [] -> []
  //   [x] -> string.to_graphemes(string.trim(x))
  // }
}

pub fn read_file(filepath: String) -> String {
  let assert Ok(result) = simplifile.read(filepath)
  result
}
