import math


def p1():
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day05/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    print(puzzleIn)
    maxRow = 127
    minRow = 0
    seatIDs = []
    for binarySpacePartition in puzzleIn:
        currentHigh = 127
        currentLow = 0
        row = 0
        column = 0
        print(binarySpacePartition)
        print(str(currentLow) + " : " + str(currentHigh))
        for letter in binarySpacePartition[:7]:

            if letter == "F":

                currentHigh = (
                    currentHigh - math.floor((currentHigh - currentLow) / 2) - 1
                )
            if letter == "B":
                currentLow = currentLow + math.ceil((currentHigh - currentLow) / 2)
            print(letter)
            print(str(currentLow) + " : " + str(currentHigh))
            if currentHigh == currentLow:
                row = currentHigh
                print(row)
        currentHigh = 7
        currentLow = 0
        print(str(currentLow) + " : " + str(currentHigh))
        for letter in binarySpacePartition[7:]:
            # print(letter)

            if letter == "R":
                print("R")
                currentLow = currentLow + math.ceil((currentHigh - currentLow) / 2)

            if letter == "L":
                print("L")
                currentHigh = (
                    currentHigh - math.floor((currentHigh - currentLow) / 2) - 1
                )
            print(str(currentLow) + " : " + str(currentHigh))
        if currentHigh == currentLow:
            column = currentHigh

        seatID = row * 8 + column
        seatIDs.append(seatID)

    print(seatIDs)
    print(max(seatIDs))


"""
0 127
0 63
128 rows

F = lower = move top to half
B = upper = move lower to half upper

"""


def p2():
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day05/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    # print(puzzleIn)
    maxRow = 127
    minRow = 0
    seatIDs = []
    for binarySpacePartition in puzzleIn:
        currentHigh = 127
        currentLow = 0
        row = 0
        column = 0
        # print(binarySpacePartition)
        # print(str(currentLow) + " : " + str(currentHigh))
        for letter in binarySpacePartition[:7]:

            if letter == "F":

                currentHigh = (
                    currentHigh - math.floor((currentHigh - currentLow) / 2) - 1
                )
            if letter == "B":
                currentLow = currentLow + math.ceil((currentHigh - currentLow) / 2)
            # print(letter)
            # print(str(currentLow) + " : " + str(currentHigh))
            if currentHigh == currentLow:
                row = currentHigh
                # print(row)
        currentHigh = 7
        currentLow = 0
        # print(str(currentLow) + " : " + str(currentHigh))
        for letter in binarySpacePartition[7:]:
            # print(letter)

            if letter == "R":
                # print("R")
                currentLow = currentLow + math.ceil((currentHigh - currentLow) / 2)

            if letter == "L":
                # print("L")
                currentHigh = (
                    currentHigh - math.floor((currentHigh - currentLow) / 2) - 1
                )
            # print(str(currentLow) + " : " + str(currentHigh))
        if currentHigh == currentLow:
            column = currentHigh

        seatID = row * 8 + column
        seatIDs.append(seatID)

    seatIDs = sorted(seatIDs)
    print(seatIDs)
    for seatNumberIndex in range(len(seatIDs)-1):
        # print(seatNumberIndex)
        # print(str(seatIDs[seatNumberIndex]) + " : " + str(seatIDs[seatNumberIndex+1]))
        if seatIDs[seatNumberIndex+1] - seatIDs[seatNumberIndex] > 1:
            print("gap")
            print(str(seatIDs[seatNumberIndex]) + " : " + str(seatIDs[seatNumberIndex+1]))
            print(seatIDs[seatNumberIndex]+1)



if __name__ == "__main__":
    # p1()
    p2()