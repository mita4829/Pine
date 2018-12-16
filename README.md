![Alt text](https://github.com/mita4829/Pine/blob/master/Pine.jpg "Pine Header")
# Pine
| |**Master**|
|---|:--:|
|**GCC**|Not Tested|
|**Clang**|[![Build Status](https://travis-ci.org/mita4829/Pine.svg?branch=master)](https://travis-ci.org/mita4829/Pine)|





# Abstract
Pine is a compiler and a programming language that's intended for 64-bit machines, with 32-bit backport in the future. Pine depends on `gcc` for the linker. Pine is a statically-typed programming language. A more comprehensive Pine tutorial will be written in the future. 

# Set Up
Run the `Makefile` located in `src` to build Pine.
```
cd src/
make
```
Open `Main.pn`, and replace its contents with:
```

func main() -> Int {
    print("Hello, World from Pine!");
};

```
Compile the program by passing it into Pine, and then run the executable.
```
./Pine Main.pn
./a.out
```
You should see `Hello, World from Pine!` printed to the console. 
# Pine Tutorial
**Syntax:**

Pine is white-space insensitive. All statements must be ended with a semi-colon. 

**Variables:**

This is how you define a new variable named `acorn` that's of type `String` with the value `"Tree"`.
```
let pine:String = "Tree";
```
All variables in Pine are statically-typed. All variables must have unique names in the given scope. Names can be composed of one or more characters, numbers, or symbols, but the first character in the name cannot be a number or symbol. Re-assigning a variable with a value of different type from its original type definition will result in a Pine exception. The type given at the definition site is optional, but it's highly recommendend. Pine will make an attempt to infer the type of the variable at the definition point if no type is given. 

**Numeric Values**

Pine supports integers, floating point numbers, and double precision floating point numbers, `Int`, `Float`, `Double` respectively. `Int` will follow two's complement, and it will have a width of 64-bits on 64-bit Pine compilers, and implementation defined width on 32-bit Pine compilers no less than 32-bits. `Float` and `Double` will follow IEEE 754 standard of precision. Division will result in integer division with integer operands, but regular division with floating or double precision operands. 

**Numeric Operations**

Pine supports the following operators on numeric operands. All numeric operators must have operand(s) that are the same type else a Pine exception will occur. From greatest to least precendence:

| |**Operators**|
|---|:--:|
|Parentheses| (_expression_)|
|Negation| -_expression_|
|Multiplcation, Division| *, \ |
|Addition, Subtraction| +, - |

**Example**
```
let  pineConeCountPerTree:Int = 42;
let         numberOfTrees:Int = 100;
let totalNumberOfPineCone:Int = pineConeCountPerTree * numberOfTrees;
```

**Boolean Values**

Pine supports the boolean type, `Bool`. Any variable that is of type `Bool` must have a value that is of type `Bool` or be evaluable to type `Bool`.

**Logical Operations**
Pine supports the following operators on `Bool` operands. All logical operators must have operand(s) that are the same type else a Pine exception will occur. From greatest to least precendence: 

| |**Operators**|
|---|:--:|
|Parentheses| (_expression_)|
|Logical And| && |
|Logical Or| \|\| |

**Example**
```
let arePineTreeReferencesFun:Bool = True;
let       pineConeTeaIsTasty:Bool = False;
```
