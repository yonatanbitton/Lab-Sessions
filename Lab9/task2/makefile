#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: skeleton

# Tool invocations
# Executable "task0" depends on the files numbers.o and skeleton.o.
skeleton:  skeleton.o 
	ld -g -melf_i386 skeleton.o -o skeleton

# Depends on the source and header files
skeleton.o: skeleton.s
	nasm -g -f elf skeleton.s -o skeleton.o
 
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o skeleton
