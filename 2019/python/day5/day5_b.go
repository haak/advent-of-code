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

	line := "3,9,8,9,10,9,4,9,99,-1,8"
	// line := readInput()
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
		// fmt.Println(instruction)
		// fmt.Println(opCode)

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
		// fmt.Println("programCounter: ", programCounter)
		switch opCode {

		case 1:

			// fmt.Println("case 1:", a," ", b, " mode1: ", paramMode1, " mode2: ", paramMode2, " location ", num[num[programCounter+3]])
			// fmt.Println(num)
			num[num[programCounter+3]] = a + b // num[dest] = num 1 + num 2
			programCounter += 4
		case 2:
			// fmt.Println("case 2")
			num[num[programCounter+3]] = a * b
			programCounter += 4

		case 3:
			// fmt.Println("case 3")
			fmt.Scanf("%d", &num[num[programCounter+1]])
			// num[num[programCounter+1]] = num[programCounter+1]
			// fmt.Println(num[programCounter+1])
			// fmt.Println(num[num[programCounter+1]])
			programCounter += 2
		case 4:

			fmt.Println(num[num[programCounter+1]])
			programCounter += 2

		case 5: // Jump if true
			fmt.Println("case 5")
			// if the first param is non-zero 
			// set instruction pointer to the value from the 2nd param
			if num[programCounter+1] != 0{
				programCounter = num[num[programCounter+2]]
			}

		case 6: // jump if false
			fmt.Println("case 6")
			// if the first param is zero
			// set instruction pointer to the value at the 2nd param 
			if num[programCounter+1] == 0{
				programCounter = num[num[programCounter+2]]
			}


		case 7: // less than 
			fmt.Println("case 7")
			// if the first param is less than second param 
			// set 1 in the position given by the third param 
			// otherwise store 0
			if num[programCounter+1] < num[programCounter+2]{
				num[programCounter+3] = 1
			}else {
				num[programCounter+3] = 0
			}

		case 8: // equals 
			fmt.Println("case 8")
			// if the first param is == second param 
			// store 1 in the position given by third param 
			// otherwise store 0 
			if num[programCounter+1] == num[programCounter+2]{
				num[programCounter+3] = 1
			}else {
				num[programCounter+3] = 0
			}

		case 99:
			programCounter = -1

		default:
			fmt.Println("Error at instruction number ", programCounter)
			os.Exit(3)

		}

	}

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
