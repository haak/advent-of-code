package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strings"
)

func main() {
	// parse sample.txt and break into lines and then split on commas
	// then parse the strings into ints
	// then add them up
	// then print the result
	//
	var sum int

	file, err := os.Open("sample.txt")
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		ints := strings.Split(line, ",")
        print(ints)
		// for _, i := range ints {
		// 	sum += ints
		// }
	}

	fmt.Println(sum)

}
