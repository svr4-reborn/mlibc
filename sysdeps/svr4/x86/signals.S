.section .text

.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
	lcall $0xF, $0
	ud2

.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
	lcall $0xF, $0
	ud2

.section .note.GNU-stack,"",%progbits
