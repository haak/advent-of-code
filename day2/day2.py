def read_input():
    instruction_start = 0
    opcode = 0
    loc_1 = 0
    loc_2 = 0
    result_loc = 0
    current_loc = 0
    test_input  = "1,0,0,0,99"
    test_out    = "2,0,0,0,99"
    int_code = test_input.split(",")
    print(int_code)
    while current_loc < len(int_code):
        print("current loc: " + str(current_loc))
        if int_code[current_loc] == "99":
            print("hello")
            return
        
        opcode = int_code[current_loc]
        print("opcode " + opcode)
        print(current_loc)
        loc_1 = int_code[current_loc+1]
        loc_2 = int_code[current_loc+2]
        result_loc = int_code[current_loc+3]
        if opcode == 2:
            return
        if opcode == "1":
            result = int_code[loc_1] + int_code[loc_2]
            print("result" + result)
            int_code[result_loc] = result
            return
        current_loc += 4
    print(int_code)




if __name__ == "__main__":
    read_input()



