PROG = Pine
CC = g++
FLAGS = -std=c++14
OBJS = Foundation.o Interpreter.o Lexer.o Parser.o Memory.o Compiler.o Register.o main.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) 

Foundation.o : Foundation.cpp 
	$(CC) $(FLAGS) -c Foundation.cpp

Interpreter.o : Interpreter.cpp 
	$(CC) $(FLAGS) -c Interpreter.cpp

Parser.o : Parser.cpp
	$(CC) $(FLAGS) -c Parser.cpp

Memory.o : Memory.cpp
	$(CC) $(FLAGS) -c Memory.cpp

Lexer.o : Lexer.cpp 
	$(CC) $(FLAGS) -c Lexer.cpp

Compiler.o : Compiler.cpp 
	$(CC) $(FLAGS) -c Compiler.cpp

Register.o : Register.cpp 
	$(CC) $(FLAGS) -c Register.cpp

main.o : main.cpp 
	$(CC) $(FLAGS) -c main.cpp

clean:
	rm $(PROG) $(OBJS)

