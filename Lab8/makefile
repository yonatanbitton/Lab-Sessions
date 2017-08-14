#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: menu

# Tool invocations
# Executable "task0" depends on the files numbers.o and menu.o.
menu:  menu.o  
	gcc -m32 -g menu.o -o menu

# Depends on the source and header files

menu.o: menu.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector menu.c -o menu.o
	

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o menu