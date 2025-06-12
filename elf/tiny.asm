; tiny.asm
BITS 64
ORG 0x400000 ; Virtual address of the executable

ehdr: ; ELF Header (64 bytes)
    db 0x7F, "ELF", 2, 1, 1, 0 ; e_ident: magic, class (64-bit), data (little-endian), version, OS/ABI (System V), ABI version (0), padding
    times 8 db 0

    dw 2         ; e_type (ET_EXEC = executable)
    dw 62        ; e_machine (EM_X86_64)
    dd 1         ; e_version
    dq _start    ; e_entry (entry point address)
    dq phdr - $$ ; e_phoff (program header table offset)
    dq 0         ; e_shoff (section header table offset - set to 0 to omit)
    dd 0         ; e_flags
    dw ehdrsize  ; e_ehsize (size of ELF header)
    dw phentsize ; e_phentsize (size of program header entry)
    dw 1         ; e_phnum (number of program header entries)
    dw 0         ; e_shentsize (size of section header entry - set to 0)
    dw 0         ; e_shnum (number of section header entries - set to 0)
    dw 0         ; e_shstrndx (section header string table index - set to 0)

ehdrsize equ $ - ehdr

phdr: ; Program Header (56 bytes for ELF64)
    dd 1         ; p_type (PT_LOAD = loadable segment)
    dd 5         ; p_flags (PF_X | PF_W | PF_R = execute, write, read)
    dq 0         ; p_offset (offset in file)
    dq 0x400000  ; p_vaddr (virtual address in memory)
    dq 0x400000  ; p_paddr (physical address - typically same as vaddr for Linux)
    dq filesize  ; p_filesz (size of segment in file)
    dq filesize  ; p_memsz (size of segment in memory)
    dq 0x1000    ; p_align (alignment, typically page size)

phentsize equ $ - phdr

_start:
    ; Exit syscall (exit_group, syscall number 231)
    ; This is slightly shorter than `exit` (60) because it uses `mov al, 231` (2 bytes)
    ; vs `mov rax, 60` (5 bytes)
    mov al, 231 ; syscall number for exit_group
    mov edi, 69 ; exit code 0
    syscall

filesize equ $ - ehdr
