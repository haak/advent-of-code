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
        if int_code[current_loc] == 99:
            return
        
        current_loc += 1
    opcode = int_code[instruction_start]
    loc_1 = int_code[instruction_start+1]
    loc_2 = int_code[instruction_start+2]
    result_loc = int_code[instruction_start+3]
    



if __name__ == "__main__":
    read_input()



