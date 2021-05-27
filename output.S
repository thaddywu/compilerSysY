	.global v0
	.section .sdata
	.align 2
	.type v0, @object
	.size v0, 4
v0:
	.word 0
	.global v1
	.section .sdata
	.align 2
	.type v1, @object
	.size v1, 4
v1:
	.word 0
	.global v2
	.section .sdata
	.align 2
	.type v2, @object
	.size v2, 4
v2:
	.word 0
	.comm v3, 8388608, 4
	.comm v4, 8388608, 4
	.comm v5, 8388608, 4
	.comm v6, 8388608, 4
	.text
	.align 2
	.global multiply
	.type multiply, @function
multiply:
	addi sp, sp, -144
	sw ra, 140(sp)
	sw s7, 28(sp)
	bne a1, x0, .l29
	li a0, 0
	lw s7, 28(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
.l29:
	li s9, 1048576
	bgt s9, a1, .l34
.l34:
	li s7, 1073741824
.l30:
	ble s7, a1, .l31
	srai s7, s7, 1
	j .l30
.l31:
	rem t0, a0, s0
.l32:
	li s10, 1
	beq s7, s10, .l35
	srai s7, s7, 1
	add t0, t0, t0
	rem t0, t0, s0
	and t1, a1, s7
	bne t1, s7, .l33
	add t0, t0, a0
	rem t0, t0, s0
	j .l32
.l33:
	j .l32
.l35:
	mv a0, t0
	lw s7, 28(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
	.size multiply, .-multiply
	.text
	.align 2
	.global power
	.type power, @function
power:
	addi sp, sp, -128
	sw ra, 124(sp)
	sw s7, 28(sp)
	bne a1, x0, .l36
	li a0, 1
	lw s7, 28(sp)
	lw ra, 124(sp)
	addi sp, sp, 128
	ret
.l36:
	li s9, 1048576
	bgt s9, a1, .l41
.l41:
	li s7, 1073741824
.l37:
	ble s7, a1, .l38
	srai s7, s7, 1
	j .l37
.l38:
	slli s7, s7, 1
	li t0, 1
.l39:
	li s10, 1
	beq s7, s10, .l42
	srai s7, s7, 1
	sw a0, 88(sp)
	mv a0, t0
	sw a1, 84(sp)
	mv a1, t0
	call multiply
	mv t0, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	and t1, a1, s7
	bne t1, s7, .l40
	sw a0, 88(sp)
	mv a0, t0
	sw a1, 84(sp)
	lw a1, 88(sp)
	call multiply
	mv t0, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	j .l39
.l40:
	j .l39
.l42:
	mv a0, t0
	lw s7, 28(sp)
	lw ra, 124(sp)
	addi sp, sp, 128
	ret
	.size power, .-power
	.text
	.align 2
	.global memmove
	.type memmove, @function
memmove:
	addi sp, sp, -144
	sw ra, 140(sp)
	sw s7, 28(sp)
	li s7, 0
.l7:
	bge s7, a3, .l8
	slli t0, s7, 2
	add s9, a2, t0
	lw t0, 0(s9)
	add t1, a1, s7
	slli t1, t1, 2
	add s9, a0, t1
	sw t0, 0(s9)
	addi s7, s7, 1
	j .l7
.l8:
	mv a0, s7
	lw s7, 28(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
	.size memmove, .-memmove
	.text
	.align 2
	.global fft
	.type fft, @function
fft:
	addi sp, sp, -288
	sw ra, 284(sp)
	sw s7, 28(sp)
	li s10, 1
	bne a2, s10, .l9
	li a0, 1
	lw s7, 28(sp)
	lw ra, 284(sp)
	addi sp, sp, 288
	ret
.l9:
	li s7, 0
.l10:
	bge s7, a2, .l11
	andi t0, s7, 1
	bne t0, x0, .l12
	add t0, s7, a1
	slli t0, t0, 2
	add s9, a0, t0
	lw t0, 0(s9)
	srai t1, s7, 1
	slli t1, t1, 2
	add s9, s3, t1
	sw t0, 0(s9)
	j .l13
.l12:
	add t0, s7, a1
	slli t0, t0, 2
	add s9, a0, t0
	lw t0, 0(s9)
	srai t1, a2, 1
	srai t2, s7, 1
	add t1, t1, t2
	slli t1, t1, 2
	add s9, s3, t1
	sw t0, 0(s9)
.l13:
	addi s7, s7, 1
	j .l10
.l11:
	sw a0, 88(sp)
	sw a1, 84(sp)
	sw a2, 80(sp)
	mv a2, s3
	sw a3, 76(sp)
	lw a3, 80(sp)
	call memmove
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	srai t0, a2, 1
	sw a0, 88(sp)
	mv a0, a3
	sw a1, 84(sp)
	mv a1, a3
	sw t0, 32(sp)
	sw a3, 76(sp)
	call multiply
	mv s7, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	sw a0, 88(sp)
	sw a1, 84(sp)
	mv a2, t0
	sw a3, 76(sp)
	mv a3, s7
	sw t0, 32(sp)
	call fft
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	add s7, a1, t0
	sw a0, 88(sp)
	mv a0, a3
	sw a1, 84(sp)
	mv a1, a3
	sw t0, 32(sp)
	sw a3, 76(sp)
	call multiply
	mv t1, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	sw a0, 88(sp)
	sw a1, 84(sp)
	mv a1, s7
	mv a2, t0
	sw a3, 76(sp)
	mv a3, t1
	sw t0, 32(sp)
	call fft
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	li s7, 0
	li t1, 1
.l14:
	bge s7, t0, .l15
	add t2, a1, s7
	slli t4, t2, 2
	add s9, a0, t4
	lw t3, 0(s9)
	add t2, t2, t0
	slli t2, t2, 2
	add s9, a0, t2
	lw t5, 0(s9)
	sw a0, 88(sp)
	mv a0, t1
	sw a1, 84(sp)
	mv a1, t5
	sw t0, 32(sp)
	sw t1, 36(sp)
	sw t2, 40(sp)
	sw t3, 44(sp)
	sw t4, 48(sp)
	sw t5, 52(sp)
	sw a3, 76(sp)
	call multiply
	mv t6, a0
	lw t0, 32(sp)
	lw t1, 36(sp)
	lw t2, 40(sp)
	lw t3, 44(sp)
	lw t4, 48(sp)
	lw t5, 52(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	add t6, t3, t6
	rem t6, t6, s0
	add s9, a0, t4
	sw t6, 0(s9)
	sw a0, 88(sp)
	mv a0, t1
	sw a1, 84(sp)
	mv a1, t5
	sw t0, 32(sp)
	sw t1, 36(sp)
	sw t2, 40(sp)
	sw t3, 44(sp)
	sw a3, 76(sp)
	call multiply
	mv t4, a0
	lw t0, 32(sp)
	lw t1, 36(sp)
	lw t2, 40(sp)
	lw t3, 44(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	sub t3, t3, t4
	add t3, t3, s0
	rem t3, t3, s0
	add s9, a0, t2
	sw t3, 0(s9)
	sw a0, 88(sp)
	mv a0, t1
	sw a1, 84(sp)
	mv a1, a3
	sw t0, 32(sp)
	sw a3, 76(sp)
	call multiply
	mv t1, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a3, 76(sp)
	addi s7, s7, 1
	j .l14
.l15:
	li a0, 0
	lw s7, 28(sp)
	lw ra, 284(sp)
	addi sp, sp, 288
	ret
	.size fft, .-fft
	.text
	.align 2
	.global main
	.type main, @function
main:
	addi sp, sp, -224
	sw ra, 220(sp)
	sw s7, 28(sp)
	la s3, v3
	la s4, v4
	la s5, v5
	la s6, v6
	li s0, 998244353
	li s2, 2097152
	mv a0, s4
	call getarray
	mv t0, a0
	mv a0, s5
	sw t0, 32(sp)
	call getarray
	mv t1, a0
	lw t0, 32(sp)
	li a0, 60
	sw t0, 32(sp)
	sw t1, 36(sp)
	call _sysy_starttime
	lw t0, 32(sp)
	lw t1, 36(sp)
	li s1, 1
.l16:
	add s7, t0, t1
	li s8, 1
	sub s7, s7, s8
	bge s1, s7, .l17
	slli s1, s1, 1
	j .l16
.l17:
	li t0, 998244352
	div t1, t0, s1
	li a0, 3
	mv a1, t1
	sw t0, 32(sp)
	call power
	mv t1, a0
	lw t0, 32(sp)
	mv a0, s4
	li a1, 0
	mv a2, s1
	mv a3, t1
	sw t0, 32(sp)
	call fft
	lw t0, 32(sp)
	div t1, t0, s1
	li a0, 3
	mv a1, t1
	sw t0, 32(sp)
	call power
	mv t1, a0
	lw t0, 32(sp)
	mv a0, s5
	li a1, 0
	mv a2, s1
	mv a3, t1
	sw t0, 32(sp)
	call fft
	lw t0, 32(sp)
	li t1, 0
.l18:
	bge t1, s1, .l19
	slli t2, t1, 2
	add s9, s4, t2
	lw t3, 0(s9)
	add s9, s5, t2
	lw t4, 0(s9)
	mv a0, t3
	mv a1, t4
	sw t0, 32(sp)
	sw t1, 36(sp)
	sw t2, 40(sp)
	call multiply
	mv t3, a0
	lw t0, 32(sp)
	lw t1, 36(sp)
	lw t2, 40(sp)
	add s9, s4, t2
	sw t3, 0(s9)
	addi t1, t1, 1
	j .l18
.l19:
	div t1, t0, s1
	sub t0, t0, t1
	li a0, 3
	mv a1, t0
	call power
	mv t0, a0
	mv a0, s4
	li a1, 0
	mv a2, s1
	mv a3, t0
	call fft
	li t1, 0
.l20:
	bge t1, s1, .l21
	slli t0, t1, 2
	add s9, s4, t0
	lw t2, 0(s9)
	li t3, 998244351
	mv a0, s1
	mv a1, t3
	sw t0, 32(sp)
	sw t1, 36(sp)
	sw t2, 40(sp)
	call power
	mv t3, a0
	lw t0, 32(sp)
	lw t1, 36(sp)
	lw t2, 40(sp)
	mv a0, t2
	mv a1, t3
	sw t0, 32(sp)
	sw t1, 36(sp)
	call multiply
	mv t2, a0
	lw t0, 32(sp)
	lw t1, 36(sp)
	add s9, s4, t0
	sw t2, 0(s9)
	addi t1, t1, 1
	j .l20
.l21:
	li a0, 79
	call _sysy_stoptime
	mv a0, s7
	mv a1, s4
	call putarray
	li a0, 0
	lw s7, 28(sp)
	lw ra, 220(sp)
	addi sp, sp, 224
	ret
	.size main, .-main
