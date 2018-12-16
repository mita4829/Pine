# Script to run Pine tests
import os
from sys import exit
import filecmp

RED = "\033[00;31m"
GREEN = "\033[00;32m"
NORMAL = "\033[0m"

print(GREEN+"Building Pine"+NORMAL)
os.system("cd src/; make; cd ..")
print(GREEN+"Building PineRuntime"+NORMAL)
os.system("cd src/; gcc -std=c11 PineRuntime.c -S; cp PineRuntime.s ../Tests")


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
        os.system("diff "+file+" ../Test_Expected/"+ans)
        # Clean up files from tests
        os.system("rm Pine.s")
        os.system("rm log.txt");
        os.system("rm output");
        exit()

print(GREEN + "All tests passed!" + NORMAL)
