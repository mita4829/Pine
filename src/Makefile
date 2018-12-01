PROG = Pine
CC = g++
FLAGS = -std=c++14
OBJS = Foundation.o Lexer.o Parser.o Static.o Compiler.o Register.o main.o 

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) 

Foundation.o : Foundation.cpp 
	$(CC) $(FLAGS) -c Foundation.cpp

Lexer.o : Lexer.cpp 
	$(CC) $(FLAGS) -c Lexer.cpp

Static.o : Static.cpp 
	$(CC) $(FLAGS) -c Static.cpp

main.o : main.cpp 
	$(CC) $(FLAGS) -c main.cpp

Parser.o : Parser.cpp
	$(CC) $(FLAGS) -c Parser.cpp

Compiler.o : Compiler.cpp
	$(CC) $(FLAGS) -c Compiler.cpp

Register.o : Register.cpp
	$(CC) $(FLAGS) -c Register.cpp
clean:
	rm $(PROG) $(OBJS)

