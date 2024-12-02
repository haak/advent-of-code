def p1():
    file = open("in.txt", "r")
    # file = open("sample.txt", "r")

    # as you move across you need to reset the left right to 0
    # store as a list of lists
    # content = file.read()
    a = []
    content = file.read()
    puzzleIn = content.split("\n")
    print(puzzleIn)
    depth = 0
    distanceRight = 0
    treesEncountered = 0
    # print(puzzleIn[0][0])
    while depth < len(puzzleIn):
        for i in range(3):
            print(distanceRight)
            print(len(puzzleIn[depth]))

            # print("right")
            distanceRight += 1
            if distanceRight == len(puzzleIn[depth]):
                distanceRight = 0

        for i in range(1):
            # print("down")
            depth += 1
            print(" distance right " + str(distanceRight) + " depth " + str(depth))
            if depth < len(puzzleIn):

                if puzzleIn[depth][distanceRight] == "#":
                    print("hit a tree")
                    treesEncountered += 1

    #     a.append( [ x for x in line.split(' ') ] )
    # # l = [[num for num in line.split(' ')] for line in file]
    # print(a)
    # return
    print("trees Encountered: " + " " + str(treesEncountered))
    return


def p2():
    file = open("in.txt", "r")
    # file = open("sample.txt", "r")
    tuples = [(1, 1), (3, 1), (5, 1), (7, 1), (1, 2)]
    totalTrees = 1
    content = file.read()
    puzzleIn = content.split("\n")
    print(puzzleIn)

    for test in tuples:
        depth = 0
        distanceRight = 0
        print("******** running test ******** " + str(test))
        treesEncountered = 0

        while depth < len(puzzleIn):
            for i in range(test[0]):
                # print(distanceRight)
                # print(len(puzzleIn[depth]))

                # print("right")
                distanceRight += 1
                if distanceRight == len(puzzleIn[depth]):
                    distanceRight = 0

            for i in range(test[1]):
                # print("down")
                depth += 1
            print(" distance right " + str(distanceRight) + " depth " + str(depth))
            if depth < len(puzzleIn):

                if puzzleIn[depth][distanceRight] == "#":
                    print("hit a tree")
                    treesEncountered += 1
        totalTrees = totalTrees * treesEncountered
        print("temp trees = " + str(treesEncountered))

    print("trees Encountered: " + " " + str(totalTrees))
    return


if __name__ == "__main__":
    # p1()
    p2()