#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myshell

# Tool invocations
# Executable "hello" depends on the files hello.o and myshell.o.
myshell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -m32 -g -Wall -c -o myshell.o myshell.c

# Depends on the source and header files
LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o myshell