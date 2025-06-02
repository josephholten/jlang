.global _start

.section .text
# strlen(rdi = pointer to string)
# returns: rax = length
.type strlen, @function
strlen:
  xor %rax, %rax            # length counter = 0
.loop:
  movb (%rdi,%rax,1), %dl   # load byte at rdi + rax
  test %dl, %dl
  je .done
  inc %rax
  jmp .loop
.done:
  ret

.global _start
_start:
  lea str(%rip), %rdi    # %rdi = address of "hello world"
  call strlen
  mov %rax, %rdi
  call exit

.section .rodata
str:
  .asciz "hello world"

.section .text

exit:
  mov $60, %rax  # exit syscall
  # pass on rdi
  syscall
