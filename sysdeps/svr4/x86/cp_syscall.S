.section .text

.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, @function
__mlibc_do_asm_cp_syscall:
	mov 8(%esp), %eax
	mov 12(%esp), %edx
	mov %edx, 8(%esp)
	mov 16(%esp), %edx
	mov %edx, 12(%esp)
	mov 20(%esp), %edx
	mov %edx, 16(%esp)
	mov 24(%esp), %edx
	mov %edx, 20(%esp)
	mov 28(%esp), %edx
	mov %edx, 24(%esp)
	mov 32(%esp), %edx
	mov %edx, 28(%esp)
__mlibc_syscall_begin:
	lcall $0x7, $0
__mlibc_syscall_end:
	ret

.section .note.GNU-stack,"",%progbits
