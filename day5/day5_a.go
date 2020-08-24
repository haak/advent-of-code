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

	// Putting temp input here for TESTING

	// line := "1,225,6,6"
	line := readInput()
	// fmt.Println("line = ", line)
	// fmt.Println("\n")

	// Split on comma.
	slice := strings.Split(line, ",")

	// slice := strings.Split(scanner.Text(), ",")

	// fmt.Println(slice)

	var num []int

	var programCounter int
	// fmt.Println("num", num)
	// fmt.Println("\n")

	// loop through list of strings
	for _, s := range slice {
		// converting to numbers
		n, _ := strconv.Atoi(s)
		// append number to list
		num = append(num, n)
	}

	num = append(num, 0, 0)
	// OPCODES:
	// 99 	STOP
	// 1	ADD
	// 2	MUL

	// num[1] = 12
	// num[2] = 2

	// loop through numbers
	for programCounter > -1 {
		// takes the input and parses it then calls runInstruction based on the input

		instruction := num[programCounter]
		// this could be up to 4 numbers

		opCode := instruction % 100
		fmt.Println(instruction)
		fmt.Println(opCode)

		parameters := fmt.Sprintf("%.2d", num[programCounter]/100)
		// fmt.Println(parameters)

		if len(parameters) == 2 {
			paramMode3 := 0
			if paramMode3 == 0 {

			}

			// set modeOf3rdParam to be position mode
		}

		paramMode1 := parameters[1:]
		// fmt.Println("paramMode1", paramMode1)

		paramMode2 := parameters[:1]
		// fmt.Println("paramMode2", paramMode2)

		a := num[programCounter+1]
		if paramMode1 == "0" && (opCode == 1 || opCode == 2) {
			a = num[a]
		}
		b := num[programCounter+2]
		if paramMode2 == "0" && (opCode == 1 || opCode == 2) {
			b = num[b]
		}
		fmt.Println("programCounter: ", programCounter)
		switch opCode {

		case 1:
			
			fmt.Println("case 1:", a," ", b, " mode1: ", paramMode1, " mode2: ", paramMode2, " location ", num[num[programCounter+3]])
			fmt.Println(num)
			num[num[programCounter+3]] = a + b // num[dest] = num 1 + num 2
			programCounter += 4
		case 2:
			fmt.Println("case 2")
			num[num[programCounter+3]] = a * b
			programCounter += 4

		case 3:
			fmt.Println("case 3")

			num[num[programCounter+1]] = num[programCounter+1]
			fmt.Println(num[programCounter+1])
			fmt.Println(num[num[programCounter+1]])
			programCounter += 2
		case 4:
			fmt.Println("case 4")
			fmt.Println(num[programCounter+1])
			programCounter += 2

		case 99:
			programCounter = -1

		default:
			fmt.Println("Error at instruction number ", programCounter)
			os.Exit(3)
			

		}
		// break

		// 1002 ==

		// parse
		// 1004 %100 = 4
		// 4 % 100 = 4
		// fmt.Println(num[i])

		// a := num[num[i+1]] // num[loc 1] = num 1
		// b := num[num[i+2]] // num[loc 2] = num 2

		// // check opcode
		// if num[i] == 1 {
		// 	// run func 1

		// 	num[num[i+3]] = a + b // num[dest] = num 1 + num 2
		// } else if num[i] == 2 {
		// 	// run func 2
		// 	num[num[i+3]] = a * b
		// } else if num[i] == 3 {
		// 	// opcode 3
		// 	// should move the number 2 back because we only use 2 not 4
		// 	// takes one input and saves it at the the address
		// 	num[num[i+1]] = num[i+1]
		// 	i-=2

		// } else if num[i] == 4 {
		// 	fmt.Println( num[i+1])
		// 	return
		// 	// should move the number
		// 	// opcode 4

		// } else if num[i] == 99 {

		// 	break
		// } else {
		// 	fmt.Println("ERROR")
		// 	fmt.Println(i)
		// 	break
		// }

	}
	fmt.Println(num[0])
	// fmt.Println("\n")
	// Display all elements.
	// for true {
	// 	//fmt.Println(code[codeIndex])
	// }

}

func runInstruction() {
	// position in memory == address
	// value add
	//OPCODES: indicates what to do
	// 1: 3 inputs
	// 2: 3 inputs
	//  99: no inputs
	// 3, 4
	//VERBS
	//NOUNS
	//PARAMETER MODES:
	// 0,1
	// mode 0: 50 means address 50 - go to 50 then take number
	// mode 1: 50 means 50
}

func readInput() (line string) {

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
			log.Fatal("Didn't find a line in in.txt")
		} else {
			log.Fatal(err)
		}
	}
	line = scanner.Text()
	return line

}
