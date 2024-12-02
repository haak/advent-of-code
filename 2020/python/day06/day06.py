def p1():
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day06/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    print(puzzleIn)
    totalAnswers = 0
    forms = []
    tmpString = ""
    for puzzle in puzzleIn:
        # print(puzzle)
        if puzzle == "":
            # print("********")
            forms.append(tmpString)

            # passports += tmpString
            tmpString = ""
        else:
            # print(puzzle)
            tmpString += puzzle
            tmpString += " "

    print(forms)
    for group in forms:
        groupTrimmed = group.replace(" ", "")
        if groupTrimmed != " ":
            s = set(groupTrimmed)
            totalAnswers += len(s)
            print(str(s) + str(len(s)))
        # remove dupes from strings
    print(totalAnswers)
    return


def p2():
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day06/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    print(puzzleIn)
    totalAnswers = 0
    forms = []
    tmpString = ""
    for puzzle in puzzleIn:
        # print(puzzle)
        if puzzle == "":
            # print("********")
            forms.append(tmpString)

            # passports += tmpString
            tmpString = ""
        else:
            # print(puzzle)
            tmpString += puzzle
            tmpString += " "

    print(forms)
    for group in forms:
        # print("group: " + group)
        # print(group.split(" "))
        people = group.split(" ")
        allAnswered = []
        # groupTrimmed = group.replace(" ", "")
        # print(groupTrimmed)
        everyoneAnswer = group.split(" ")[0]

        for answer in people:

            res = ""
            print("everyoneAnswer: " + everyoneAnswer)
            print("answer : " + answer)
            if answer == "":

                # print("end")
                totalAnswers += len(everyoneAnswer)
            else:
                # print("everyoneAnswer: " + everyoneAnswer)
                for letter in answer:
                    if letter in everyoneAnswer and not letter in res:
                        res += letter
                everyoneAnswer = res

            print(totalAnswers)
            # print(everyoneAnswer + " : " + answer)
            # print('weord')

    # print("help")
    print(totalAnswers)


if __name__ == "__main__":
    # p1()
    p2()