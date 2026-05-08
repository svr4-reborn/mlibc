.section .text

.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, @function
__mlibc_spawn_thread:
	mov $-38, %eax
	ret

.section .note.GNU-stack,"",%progbits
