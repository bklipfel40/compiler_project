			.text
			.globl			__start
__start:
			jal			_main
			li			$v0, 			10
			syscall
_main:
			addiu			$sp, 			$sp, 			-32
			sw			$ra, 			0($sp)
			sw			$fp, 			4($sp)
			sw			$s7, 			8($sp)
			sw			$t0, 			12($sp)
			sw			$t1, 			16($sp)
			sw			$t2, 			20($sp)
			sw			$t3, 			24($sp)
			sw			$t4, 			28($sp)
			addiu			$fp, 			$sp, 			28
			addiu			$s7, 			$sp, 			-4

			li			$v0, 			4
			la			$a0, 			L1
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_p
L9:
			lw			$t0, 			_p
			li			$t1, 			1
			sgt			$t2, 			$t0, 			$t1
			beqz			$t2, 			L10
			li			$v0, 			4
			la			$a0, 			L2
			syscall
			lw			$t0, 			_p
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$v0, 			4
			la			$a0, 			L3
			syscall
			li			$t0, 			2
			sw			$t0, 			_d
L6:
			lw			$t0, 			_d
			lw			$t1, 			_p
			li			$t3, 			2
			div			$t4, 			$t1, 			$t3
			slt			$t1, 			$t0, 			$t4
			beqz			$t1, 			L7
			lw			$t0, 			_p
			lw			$t3, 			_d
			div			$t4, 			$t0, 			$t3
			sw			$t4, 			_q
			lw			$t0, 			_q
			lw			$t3, 			_d
			mul			$t4, 			$t0, 			$t3
			lw			$t0, 			_p
			seq			$t3, 			$t4, 			$t0
			beqz			$t3, 			L5
			lw			$t0, 			_d
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$v0, 			4
			la			$a0, 			L4
			syscall
L5:
			lw			$t0, 			_d
			li			$t3, 			1
			add			$t4, 			$t0, 			$t3
			sw			$t4, 			_d
			b			L6
L7:
			li			$v0, 			4
			la			$a0, 			L8
			syscall
			li			$v0, 			4
			la			$a0, 			L1
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_p
			b			L9
L10:
_main_ret:
			lw			$ra, 			0($sp)
			lw			$fp, 			4($sp)
			lw			$s7, 			8($sp)
			lw			$t0, 			12($sp)
			lw			$t1, 			16($sp)
			lw			$t2, 			20($sp)
			lw			$t3, 			24($sp)
			lw			$t4, 			28($sp)
			addiu			$sp, 			$sp, 			32
			jr			$ra
			.data
_d:			.word			0
_p:			.word			0
_q:			.word			0
__iobuf:			.space			4
L8:			.asciiz			"\n"
L4:			.asciiz			" "
L1:			.asciiz			"> "
L3:			.asciiz			" are "
L2:			.asciiz			"factors of "
