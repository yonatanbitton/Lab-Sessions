#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: task2

# Tool invocations
# Executable "hello" depends on the files hello.o and task2.o.
task2: task2.o JobControl.o LineParser.o
	gcc -g -m32 -Wall -o task2 task2.o JobControl.o LineParser.o

task2.o: task2.c
	gcc -m32 -g -Wall -c -o task2.o task2.c

# Depends on the source and header files
JobControl.o: JobControl.c JobControl.h
	gcc -m32 -g -Wall -c -o JobControl.o JobControl.c 

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o task2