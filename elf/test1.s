.section .text
.global _start

_start:
  mov $60, %rax  # exit syscall
  mov $69, %rdi
  syscall
