def p1():
    file = open("in.txt", "r")
    #  find 2 entries that add to 2020
    content = file.read()
    # print(content)
    puzzleIn = content.split("\n")
    # print(content)
    for number in range(len(puzzleIn)):
        for number2 in range(number,len(puzzleIn)):
            # print(number ,number2)
            # print(puzzleIn[number] + puzzleIn[number2])
            if int(puzzleIn[number]) + int(puzzleIn[number2]) == 2020:
                print(int(puzzleIn[number])* int(puzzleIn[number2])) 
        


def p2():
    file = open("in.txt", "r")
    tmpList = []
    content = file.read()
    # print(content)
    puzzleIn = content.split("\n")
    for number in range(len(puzzleIn)):
        for number1 in range(number, len(puzzleIn)):
            for number2 in range(number1, len(puzzleIn)):
                if int(puzzleIn[number]) + int(puzzleIn[number1]) + int(puzzleIn[number2]) == 2020:
                    print(int(puzzleIn[number])* int(puzzleIn[number1])*int(puzzleIn[number2])) 



if __name__ == "__main__":
    # p1()
    p2()