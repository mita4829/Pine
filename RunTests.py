# Script to run Pine tests
import os
from sys import exit
import filecmp

RED = "\033[00;31m"
GREEN = "\033[00;32m"
NORMAL = "\033[0m"

os.chdir("Tests")
files = os.listdir(os.getcwd())
for file in files:
    if(file == "PineRuntime.s"):
        continue
    os.system("../src/Pine "+file)
    os.system("./a.out > output")
    ans = file[:len(file)-3]+".ans"
    print(GREEN+"Testing: "+NORMAL+file)
    same = filecmp.cmp("output", "../Test_Expected/"+ans)
    os.system("rm output Pine.s a.out")
    if(not same):
        print(RED + "Failed: "+ NORMAL + file)
        exit()

print(GREEN + "All tests passed!" + NORMAL)
