package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
)

func main() {

	file, err := os.Open("in.txt")
	if err != nil {
		log.Fatal(err)
	}
	scanner := bufio.NewScanner(file)

	// Default scanner is bufio.ScanLines. Lets use ScanWords.
	scanner.Split(bufio.ScanWords)

	success := scanner.Scan()
	if success == false {
		// False on error or EOF. Check error
		err = scanner.Err()
		if err == nil {
			// reached EOF
			//log.Println("Scan completed and reached EOF")
			log.Fatal("Didn't find a line in input.txt")
		} else {
			log.Fatal(err)
		}
	}
	var line = scanner.Text()
	fmt.Println("line = ", line)
	fmt.Println("\n")

	// Split on comma.
	slice := strings.Split(line, ",")

	// slice := strings.Split(scanner.Text(), ",")

	// fmt.Println(slice)

	var input []int
	// fmt.Println("num", num)
	// fmt.Println("\n")

	// loop through list of strings
	for _, s := range slice {
		// converting to numbers
		n, _ := strconv.Atoi(s)
		// append number to list
		input = append(input, n)
	}

	for j := 0; j < 99; j++ {
		for k := 0; k < 99; k++ {
			num := make([]int, len(input))
			copy(num, input)

			// OPCODES:
			// 99 	STOP
			// 1	ADD
			// 2	MUL

			num[1] = j
			num[2] = k

			// loop through numbers
			for i := 0; ; i += 4 {

				a := num[num[i+1]] // num[loc 1] = num 1
				b := num[num[i+2]] // num[loc 2] = num 2

				// check opcode
				if num[i] == 1 {
					num[num[i+3]] = a + b // num[dest] = num 1 + num 2
				} else if num[i] == 2 {
					num[num[i+3]] = a * b
				} else if num[i] == 99 {
					break
				} else {
					fmt.Println("ERROR")
					fmt.Println(i)
					break
				}
				if num[0] > 19690720 {
					continue
				} else if num[0] == 19690720 {
					fmt.Println(j, k)
					return
				}
			}
			// fmt.Println(num[0])
			// fmt.Println("\n")
			// Display all elements.
			// for true {
			// 	//fmt.Println(code[codeIndex])
			// }

		}
	}
}
