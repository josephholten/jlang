.section .text
.global _start

exit:
  mov $60, %rax  # exit syscall
  # pass on rdi
  syscall

_start:
  mov $1, %rdi
  call exit
