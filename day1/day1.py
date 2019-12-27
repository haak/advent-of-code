#fuel for a module  based on mass
# fuel  = mass / 3 Round down subtract 2
# math .floor
import math
import pytest


def day1():
    file = open("in.txt", "r")
    out = open("out.txt","w")
    total_fuel = 0
    for line in file:
        fuel = get_fuel(int(line))
        total_fuel += fuel
    print(total_fuel)



mass = 12
def get_fuel(mass):
    fuel = (math.floor(mass/3))-2
    return fuel


day1()


def get_fuel_recurring(mass):
    while mass_remaining != 0:
        fuel = (math.floor(mass/3))-2
        



#mass =  14
# fuel = 2
#fuel = 


def fuel_for_fuel(mass_of_fuel):
    # use recursion for this. return once the fuel required is 0. if its not 0 
    # call function with the mass.
    return math.floor(mass_of_fuel/3)-2  