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



def get_fuel(mass):
    fuel = (math.floor(mass/3))-2
    return fuel


# day1()


def get_fuel_recurring(mass):
    mass_remaining = 0
    total_fuel = 0
    while mass_remaining > 0:
        fuel = (math.floor(mass/3))-2
        


def day1p2():
    file = open("in.txt", "r")
    # out = open("out.txt","w")
    total_fuel = 0
    for line in file:
        fuel = get_fuel(int(line))
        total_fuel += fuel
        # print(fuel)
        while fuel > 0:
            fuel = get_fuel(int(fuel))
            if fuel > 0:
                total_fuel += fuel
            # print(fuel)
    print("total fuel: " + str(total_fuel))
        
day1p2()