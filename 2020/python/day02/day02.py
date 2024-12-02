def p1():
    file = open("in.txt", "r")
    # file = open("sample.txt", "r")
    #  find 2 entries that add to 2020
    content = file.read()
    numberOfCorrectPasswords = 0
    puzzleIn = content.split("\n")

    for line in puzzleIn:
        status = False
        counter = 0
        lineSplit = line.split(" ")
        # print(lineSplit)
        length = lineSplit[0]
        minNum = int(length.split("-")[0])
        maxNum = int(length.split("-")[1])
        # print("min: " + str(minNum) + " max: " + str(maxNum))

        letter = lineSplit[1][0]
        # print(letter)

        password = lineSplit[2]
        # print(password)
        for char in password:
            if char == letter:
                counter += 1
        # print(counter)

        # print("min " + str(minNum) + " max " + str(maxNum) + " letter " + letter + " " + password)
        print(counter)
        print(minNum)
        print(maxNum)

        if counter <= maxNum:
            if counter >= minNum:
                print(True)
                numberOfCorrectPasswords += 1
                status = True

        print(
            "min "
            + str(minNum)
            + " max "
            + str(maxNum)
            + " letter "
            + letter
            + " "
            + password
            + " "
            + str(status)
            + " "
            + str(counter)
        )
    print(numberOfCorrectPasswords)
    # min = lineSplit[0][0]
    # max =
    # print(line)
    # print(puzzleIn)


def p2():
    file = open("in.txt", "r")
    # file = open("sample.txt", "r")
    #  find 2 entries that add to 2020
    content = file.read()
    numberOfCorrectPasswords = 0
    puzzleIn = content.split("\n")

    for line in puzzleIn:
        firstLoc = True
        secondLoc = True
        status = False
        counter = 0
        lineSplit = line.split(" ")
        # print(lineSplit)
        length = lineSplit[0]
        firstIndex = int(length.split("-")[0])
        secondIndex = int(length.split("-")[1])
        # print("min: " + str(minNum) + " max: " + str(maxNum))

        letter = lineSplit[1][0]
        # print(letter)

        password = lineSplit[2]
        # print(password)
        # for char in password:
        #     if char == letter:
        #         counter += 1
        # print(counter)
        print(password[firstIndex-1])
        if password[firstIndex-1] == letter:
            firstLoc = True
        else:
            firstLoc = False

        print(len(password))
        print(secondIndex+1)
        if len(password) >= secondIndex:
            print(password[secondIndex-1])
            if password[secondIndex-1] == letter:
                secondLoc = True
            else:
                secondLoc = False

        if firstLoc != secondLoc:
            
            numberOfCorrectPasswords += 1
            status = True
        
        print("min " + str(firstIndex) + " max " + str(secondIndex) + " letter " + letter + " " + password + " " + str(status))
    print("***************")
    print(numberOfCorrectPasswords)


if __name__ == "__main__":
    # p1()
    p2()