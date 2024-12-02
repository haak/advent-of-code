import re


def p1():
    # file = open("in.txt", "r")
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day04/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    # print(puzzleIn)
    tmpString = ""
    passportFields = ["byr:", "iyr:", "eyr:", "hgt:", "hcl:", "ecl:", "pid:"]
    passports = []
    validPassports = 0
    for puzzle in puzzleIn:
        # print(puzzle)
        if puzzle == "":
            # print("********")
            passports.append(tmpString)

            # passports += tmpString
            tmpString = ""
        else:
            # print(puzzle)
            tmpString += puzzle
            tmpString += " "
    # print(passports)
    for passport in passports:
        valid = True
        for field in passportFields:
            if field in passport:
                # print(True)
                pass
                # print(field + " : " + passport)
            else:
                # print("not valid")
                valid = False

        if valid == True:
            validPassports += 1

    print(validPassports)


def p2():
    # file = open("in.txt", "r")
    file = open(
        "/home/alex/Documents/projects/go/advent-of-code-2019/2020/day04/in.txt",
        "r",
    )
    content = file.read()
    puzzleIn = content.split("\n")
    # print(puzzleIn)
    tmpString = ""
    passportFields = ["byr:", "iyr:", "eyr:", "hgt:", "hcl:", "ecl:", "pid:"]

    # regex = []

    passports = []
    validPassports = 0
    for puzzle in puzzleIn:
        # print(puzzle)
        if puzzle == "":
            # print("********")
            passports.append(tmpString)

            # passports += tmpString
            tmpString = ""
        else:
            # print(puzzle)
            tmpString += puzzle
            tmpString += " "
    # print(passports)
    # print(len(passports))

    for passport in passports:
        passcheck = passport.split(" ")
        # passcheck = filter(None, passcheck)
        passcheck = list(filter(None, passcheck))
        # print(passcheck)
        # print(len(passcheck))

        valid = True

        if len(passcheck) < 7:
            valid = False

        # if len(passport) < 7:
        #     valid  = False
        # if len(passport) > 8:
        #     valid = False

        for field in passportFields:
            if field in passport:
                # print(True)
                pass
                # print(field + " : " + passport)
            else:
                # print("not valid")
                valid = False

        match = re.findall("byr:\d{4}", passport)
        # print(match)
        if match:
            # print("check if match is valid")
            # print(match[0])

            birthYear = match[0]
            year = int(birthYear.split(":")[1])
            if year >= 1920:
                if year <= 2002:
                    pass

                else:
                    valid = False

        else:
            valid = False

        match = re.findall("iyr:\d{4}", passport)
        # print(match)
        if match:
            # print("check if match is valid")
            # print(match[0])
            issueYear = match[0]
            print(issueYear)
            year2 = int(issueYear.split(":")[1])
            print(year2)
            print(year2>=2010)
            print(year2 <= 2020)
            if year2 >= 2010:
                if year2 <= 2020:
                    # print("valid year")
                    pass
                else:
                    valid = False
            else:
                valid = False
            print(valid)
        else:
            valid = False

        match = re.findall("eyr:\d{4}", passport)
        # print(match)
        if match:
            # print("check if match is valid")
            # print(match[0])
            expirationYear = match[0]
            print(expirationYear)
            year3 = int(expirationYear.split(":")[1])
            if year3 >= 2020:
                if year3 <= 2030:
                    # print("valid year")
                    pass
                else:
                    valid = False
            else:
                    valid = False

        else:
            valid = False

        match = re.findall("(hgt:\d{3}cm|hgt:\d{2}in)", passport)
        # print(match)
        if match:
            # print("check if height is valid")
            # print(match[0])
            fullHeight = match[0]
            height = fullHeight.split(":")[1]
            # print(height)
            if len(height) == 5:
                # print("cm")
                actualHeight = int(height[0:3])
                if actualHeight >= 150:
                    if actualHeight <= 193:
                        pass
                    else:
                        valid = False
                # print(actualHeight)
            elif len(height) == 4:
                # print("in")
                actualHeight = int(height[0:2])
                if actualHeight <= 76:
                    if actualHeight > 59:
                        pass
                    else:
                        valid = False

        else:
            valid = False

        match = re.findall("hcl:#[a-f0-9]{6}", passport)
        # print(match)
        if match:
            pass

        else:
            valid = False

        match = re.findall("ecl:(amb|blu|brn|gry|grn|hzl|oth)", passport)
        # print(match)
        if match:
            pass

        else:
            valid = False

        match = re.findall("pid:\d{9}\s", passport)
        # print(match)
        if match:
            pass

        else:
            valid = False

        if valid == True:
            print("valid passport " + passport)
            validPassports += 1

    print(validPassports)


if __name__ == "__main__":
    # p1()
    p2()