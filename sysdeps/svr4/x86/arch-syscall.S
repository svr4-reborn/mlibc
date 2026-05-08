.section .text

.hidden __do_syscall0
.hidden __do_syscall0_dual
.hidden __do_syscall1
.hidden __do_syscall1_dual
.hidden __do_syscall2
.hidden __do_syscall2_dual
.hidden __do_syscall3
.hidden __do_syscall3_dual
.hidden __do_syscall4
.hidden __do_syscall4_dual
.hidden __do_syscall5
.hidden __do_syscall5_dual
.hidden __do_syscall6
.hidden __do_syscall6_dual
.hidden __do_syscall7
.hidden __do_syscall7_dual

.macro svr4_shift_and_capture_carry nargs
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
	setc %dl
	movzbl %dl, %edx
	ret
.endm

.macro svr4_shift_and_capture_carry_dual nargs
	mov (4 * (\nargs + 2))(%esp), %ecx
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
	jc 1f
	mov %edx, (%ecx)
	jmp 2f
1:
	movl $0, (%ecx)
2:
	setc %dl
	movzbl %dl, %edx
	ret
.endm

.global __do_syscall0
.type __do_syscall0, @function
__do_syscall0:
	mov 4(%esp), %eax
	lcall $0x7, $0
	setc %dl
	movzbl %dl, %edx
	ret

.global __do_syscall0_dual
.type __do_syscall0_dual, @function
__do_syscall0_dual:
	mov 8(%esp), %ecx
	mov 4(%esp), %eax
	lcall $0x7, $0
	jc 1f
	mov %edx, (%ecx)
	jmp 2f
1:
	movl $0, (%ecx)
2:
	setc %dl
	movzbl %dl, %edx
	ret

.global __do_syscall1
.type __do_syscall1, @function
__do_syscall1:
	svr4_shift_and_capture_carry 1

.global __do_syscall1_dual
.type __do_syscall1_dual, @function
__do_syscall1_dual:
	svr4_shift_and_capture_carry_dual 1

.global __do_syscall2
.type __do_syscall2, @function
__do_syscall2:
	svr4_shift_and_capture_carry 2

.global __do_syscall2_dual
.type __do_syscall2_dual, @function
__do_syscall2_dual:
	svr4_shift_and_capture_carry_dual 2

.global __do_syscall3
.type __do_syscall3, @function
__do_syscall3:
	svr4_shift_and_capture_carry 3

.global __do_syscall3_dual
.type __do_syscall3_dual, @function
__do_syscall3_dual:
	svr4_shift_and_capture_carry_dual 3

.global __do_syscall4
.type __do_syscall4, @function
__do_syscall4:
	svr4_shift_and_capture_carry 4

.global __do_syscall4_dual
.type __do_syscall4_dual, @function
__do_syscall4_dual:
	svr4_shift_and_capture_carry_dual 4

.global __do_syscall5
.type __do_syscall5, @function
__do_syscall5:
	svr4_shift_and_capture_carry 5

.global __do_syscall5_dual
.type __do_syscall5_dual, @function
__do_syscall5_dual:
	svr4_shift_and_capture_carry_dual 5

.global __do_syscall6
.type __do_syscall6, @function
__do_syscall6:
	svr4_shift_and_capture_carry 6

.global __do_syscall6_dual
.type __do_syscall6_dual, @function
__do_syscall6_dual:
	svr4_shift_and_capture_carry_dual 6

.global __do_syscall7
.type __do_syscall7, @function
__do_syscall7:
	svr4_shift_and_capture_carry 7

.global __do_syscall7_dual
.type __do_syscall7_dual, @function
__do_syscall7_dual:
	svr4_shift_and_capture_carry_dual 7

.section .note.GNU-stack,"",%progbits
