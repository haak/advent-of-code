def read_input():
    instruction_start = 0
    opcode = 0
    loc_1 = 0
    loc_2 = 0
    result_loc = 0
    current_loc = 0
    test_input  = "1,0,0,0,99"
    test_input  = "2,3,0,3,99"
    test_out    = "2,0,0,0,99"
    test_input  = "2,4,4,5,99,0"
    test_input  = "1,12,2,3,1,1,2,3,1,3,4,3,1,5,0,3,2,1,10,19,1,19,5,23,1,6,23,27,1,27,5,31,2,31,10,35,2,35,6,39,1,39,5,43,2,43,9,47,1,47,6,51,1,13,51,55,2,9,55,59,1,59,13,63,1,6,63,67,2,67,10,71,1,9,71,75,2,75,6,79,1,79,5,83,1,83,5,87,2,9,87,91,2,9,91,95,1,95,10,99,1,9,99,103,2,103,6,107,2,9,107,111,1,111,5,115,2,6,115,119,1,5,119,123,1,123,2,127,1,127,9,0,99,2,0,14,0"

    
    int_code = test_input.split(",")
    int_code = list(map(int, int_code))
    print(int_code)
    while current_loc < len(int_code):
        print("current loc: " + str(current_loc))
        if int_code[current_loc] == 99:
            print("hello")
            print(int_code)
            return
    
        
        opcode = int_code[current_loc]
        print("opcode " + str(opcode))
        print(current_loc)
        loc_1 = int_code[current_loc+1]
        print("location_1: "+ str(loc_1))
        print("location_2: "+ str(loc_2))

        loc_2 = int_code[current_loc+2]
        result_loc = int_code[current_loc+3]
        if opcode == 2:
            result = int_code[loc_1] * int_code[loc_2]
            print("result " + str(result))
            int_code[result_loc] = result
        if opcode == 1:
            result = int_code[loc_1] + int_code[loc_2]
            print("result " + str(result))
            int_code[result_loc] = result
        
        current_loc += 4
    print(int_code)




if __name__ == "__main__":
    read_input()




memory = ""
address = []
opcodes = []
instruction = []
