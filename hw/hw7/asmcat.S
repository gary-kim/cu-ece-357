.bss
.comm buffer, 4096, 4
.text
main:
    mov $0, %rax
    mov $0, %rdi
    mov $buffer, %rsi
    mov $4096, %rdx
    syscall
    cmpl $0, (%rax)
    jl exit_error
    je exit
    mov %rax, %rdx
    mov $1, %rax
    mov $1, %rdi
    mov $buffer, %rsi
    syscall
    jmp main
exit_error:
    mov $60, %rax
    mov $1, %rdi
    syscall
exit:
    mov $60, %rax
    mov $0, %rdi
    syscall



