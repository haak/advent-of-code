def main():
    f1 = open("in.txt","r")
    wireA = f1.readline()
    wireB = f1.readline()
    # print(wireA)
    x,y = 0
    locs = {}
    # direction = wire[0]
    for step in wireA:
        direction = step[0]
        delta = step[1:]
        if direction == "U":
            return
        elif direction == "D":
            return
        elif direction == "L":
            return
        elif direction == "R":
            return
    



    return

if __name__ == "__main__":
    main()