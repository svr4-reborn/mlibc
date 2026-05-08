.section .text

.hidden __do_syscall0
.hidden __do_syscall1
.hidden __do_syscall2
.hidden __do_syscall3
.hidden __do_syscall4
.hidden __do_syscall5
.hidden __do_syscall6
.hidden __do_syscall7

.macro svr4_shift_and_call nargs
	mov 4(%esp), %eax
	.if \nargs >= 1
		mov 8(%esp), %edx
		mov %edx, 4(%esp)
	.endif
	.if \nargs >= 2
		mov 12(%esp), %edx
		mov %edx, 8(%esp)
	.endif
	.if \nargs >= 3
		mov 16(%esp), %edx
		mov %edx, 12(%esp)
	.endif
	.if \nargs >= 4
		mov 20(%esp), %edx
		mov %edx, 16(%esp)
	.endif
	.if \nargs >= 5
		mov 24(%esp), %edx
		mov %edx, 20(%esp)
	.endif
	.if \nargs >= 6
		mov 28(%esp), %edx
		mov %edx, 24(%esp)
	.endif
	.if \nargs >= 7
		mov 32(%esp), %edx
		mov %edx, 28(%esp)
	.endif
	lcall $0x7, $0
	ret
.endm

.global __do_syscall0
.type __do_syscall0, @function
__do_syscall0:
	mov 4(%esp), %eax
	lcall $0x7, $0
	ret

.global __do_syscall1
.type __do_syscall1, @function
__do_syscall1:
	svr4_shift_and_call 1

.global __do_syscall2
.type __do_syscall2, @function
__do_syscall2:
	svr4_shift_and_call 2

.global __do_syscall3
.type __do_syscall3, @function
__do_syscall3:
	svr4_shift_and_call 3

.global __do_syscall4
.type __do_syscall4, @function
__do_syscall4:
	svr4_shift_and_call 4

.global __do_syscall5
.type __do_syscall5, @function
__do_syscall5:
	svr4_shift_and_call 5

.global __do_syscall6
.type __do_syscall6, @function
__do_syscall6:
	svr4_shift_and_call 6

.global __do_syscall7
.type __do_syscall7, @function
__do_syscall7:
	svr4_shift_and_call 7

.section .note.GNU-stack,"",%progbits
