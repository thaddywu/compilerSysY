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
	sw s3, 12(sp)
	sw s4, 16(sp)
	bne a1, x0, .l0
	li a0, 0
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
.l0:
	li s10, 1
	bne a1, s10, .l1
	rem s3, a0, s0
	mv a0, s3
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
.l1:
	li s11, 2
	div s3, a1, s11
	sw a0, 88(sp)
	sw a1, 84(sp)
	mv a1, s3
	call multiply
	mv s4, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	add s3, s4, s4
	rem s4, s3, s0
	li s11, 2
	rem s3, a1, s11
	li s10, 1
	bne s3, s10, .l2
	add s3, s4, a0
	rem s3, s3, s0
	mv a0, s3
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
	j .l3
.l2:
	mv a0, s4
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 140(sp)
	addi sp, sp, 144
	ret
.l3:
	.size multiply, .-multiply
	.text
	.align 2
	.global power
	.type power, @function
power:
	addi sp, sp, -128
	sw ra, 124(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	bne a1, x0, .l4
	li a0, 1
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 124(sp)
	addi sp, sp, 128
	ret
.l4:
	li s11, 2
	div s3, a1, s11
	sw a0, 88(sp)
	sw a1, 84(sp)
	mv a1, s3
	call power
	mv s4, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	sw a0, 88(sp)
	mv a0, s4
	sw a1, 84(sp)
	mv a1, s4
	call multiply
	mv s4, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	li s11, 2
	rem s3, a1, s11
	li s10, 1
	bne s3, s10, .l5
	sw a0, 88(sp)
	mv a0, s4
	lw a1, 88(sp)
	call multiply
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	mv a0, s3
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 124(sp)
	addi sp, sp, 128
	ret
	j .l6
.l5:
	mv a0, s4
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 124(sp)
	addi sp, sp, 128
	ret
.l6:
	.size power, .-power
	.text
	.align 2
	.global memmove
	.type memmove, @function
memmove:
	addi sp, sp, -144
	sw ra, 140(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	li s5, 0
.l7:
	bge s5, a3, .l8
	li s11, 4
	mul s3, s5, s11
	add s9, a2, s3
	lw s4, 0(s9)
	add s3, a1, s5
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	sw s4, 0(s9)
	li s11, 1
	add s5, s5, s11
	j .l7
.l8:
	mv a0, s5
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw s5, 20(sp)
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
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	sw s6, 24(sp)
	sw s7, 28(sp)
	li s10, 1
	bne a2, s10, .l9
	li a0, 1
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw s5, 20(sp)
	lw s6, 24(sp)
	lw s7, 28(sp)
	lw ra, 284(sp)
	addi sp, sp, 288
	ret
.l9:
	li t0, 0
.l10:
	bge t0, a2, .l11
	li s11, 2
	rem s3, t0, s11
	bne s3, x0, .l12
	add s3, t0, a1
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	lw s4, 0(s9)
	li s11, 2
	div s3, t0, s11
	li s11, 4
	mul s3, s3, s11
	la s9, v3
	add s9, s9, s3
	sw s4, 0(s9)
	j .l13
.l12:
	add s3, t0, a1
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	lw s5, 0(s9)
	li s11, 2
	div s4, a2, s11
	li s11, 2
	div s3, t0, s11
	add s3, s4, s3
	li s11, 4
	mul s3, s3, s11
	la s9, v3
	add s9, s9, s3
	sw s5, 0(s9)
.l13:
	li s11, 1
	add t0, t0, s11
	j .l10
.l11:
	sw a0, 88(sp)
	sw a1, 84(sp)
	sw a2, 80(sp)
	la a2, v3
	sw a3, 76(sp)
	lw a3, 80(sp)
	call memmove
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s11, 2
	div s4, a2, s11
	sw a0, 88(sp)
	mv a0, a3
	sw a1, 84(sp)
	mv a1, a3
	sw a2, 80(sp)
	sw a3, 76(sp)
	call multiply
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	sw a0, 88(sp)
	sw a1, 84(sp)
	sw a2, 80(sp)
	mv a2, s4
	sw a3, 76(sp)
	mv a3, s3
	call fft
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s11, 2
	div s3, a2, s11
	add s5, a1, s3
	li s11, 2
	div s4, a2, s11
	sw a0, 88(sp)
	mv a0, a3
	sw a1, 84(sp)
	mv a1, a3
	sw a2, 80(sp)
	sw a3, 76(sp)
	call multiply
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	sw a0, 88(sp)
	sw a1, 84(sp)
	mv a1, s5
	sw a2, 80(sp)
	mv a2, s4
	sw a3, 76(sp)
	mv a3, s3
	call fft
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li t0, 0
	li s7, 1
.l14:
	li s11, 2
	div s3, a2, s11
	bge t0, s3, .l15
	add s3, a1, t0
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	lw s6, 0(s9)
	add s4, a1, t0
	li s11, 2
	div s3, a2, s11
	add s3, s4, s3
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	lw s5, 0(s9)
	mv a0, s7
	sw a1, 84(sp)
	mv a1, s5
	sw t0, 32(sp)
	sw a2, 80(sp)
	sw a3, 76(sp)
	call multiply
	mv s3, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	add s3, s6, s3
	rem s4, s3, s0
	add s3, a1, t0
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	sw s4, 0(s9)
	mv a0, s7
	sw a1, 84(sp)
	mv a1, s5
	sw t0, 32(sp)
	sw a2, 80(sp)
	sw a3, 76(sp)
	call multiply
	mv s3, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	sub s3, s6, s3
	add s3, s3, s0
	rem s5, s3, s0
	add s4, a1, t0
	li s11, 2
	div s3, a2, s11
	add s3, s4, s3
	li s11, 4
	mul s3, s3, s11
	add s9, a0, s3
	sw s5, 0(s9)
	mv a0, s7
	sw a1, 84(sp)
	mv a1, a3
	sw t0, 32(sp)
	sw a2, 80(sp)
	sw a3, 76(sp)
	call multiply
	mv s7, a0
	lw t0, 32(sp)
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s11, 1
	add t0, t0, s11
	j .l14
.l15:
	li a0, 0
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw s5, 20(sp)
	lw s6, 24(sp)
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
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	sw s6, 24(sp)
	sw s7, 28(sp)
	li s0, 998244353
	li s2, 2097152
	la a0, v4
	call getarray
	mv s7, a0
	lw a0, 88(sp)
	la a0, v5
	call getarray
	mv s6, a0
	lw a0, 88(sp)
	li a0, 60
	call _sysy_starttime
	lw a0, 88(sp)
	li s1, 1
.l16:
	add s3, s7, s6
	li s11, 1
	sub s3, s3, s11
	bge s1, s3, .l17
	li s11, 2
	mul s1, s1, s11
	j .l16
.l17:
	li s11, 1
	sub s3, s0, s11
	div s3, s3, s1
	li a0, 3
	mv a1, s3
	call power
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	la a0, v4
	li a1, 0
	mv a2, s1
	mv a3, s3
	call fft
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s11, 1
	sub s3, s0, s11
	div s3, s3, s1
	li a0, 3
	mv a1, s3
	call power
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	la a0, v5
	li a1, 0
	mv a2, s1
	mv a3, s3
	call fft
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s5, 0
.l18:
	bge s5, s1, .l19
	li s11, 4
	mul s3, s5, s11
	la s9, v4
	add s9, s9, s3
	lw s4, 0(s9)
	li s11, 4
	mul s3, s5, s11
	la s9, v5
	add s9, s9, s3
	lw s3, 0(s9)
	mv a0, s4
	mv a1, s3
	call multiply
	mv s4, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	li s11, 4
	mul s3, s5, s11
	la s9, v4
	add s9, s9, s3
	sw s4, 0(s9)
	li s11, 1
	add s5, s5, s11
	j .l18
.l19:
	li s11, 1
	sub s4, s0, s11
	li s11, 1
	sub s3, s0, s11
	div s3, s3, s1
	sub s3, s4, s3
	li a0, 3
	mv a1, s3
	call power
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	la a0, v4
	li a1, 0
	mv a2, s1
	mv a3, s3
	call fft
	lw a0, 88(sp)
	lw a1, 84(sp)
	lw a2, 80(sp)
	lw a3, 76(sp)
	li s5, 0
.l20:
	bge s5, s1, .l21
	li s11, 4
	mul s3, s5, s11
	la s9, v4
	add s9, s9, s3
	lw s4, 0(s9)
	li s11, 2
	sub s3, s0, s11
	mv a0, s1
	mv a1, s3
	call power
	mv s3, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	mv a0, s4
	mv a1, s3
	call multiply
	mv s4, a0
	lw a0, 88(sp)
	lw a1, 84(sp)
	li s11, 4
	mul s3, s5, s11
	la s9, v4
	add s9, s9, s3
	sw s4, 0(s9)
	li s11, 1
	add s5, s5, s11
	j .l20
.l21:
	li a0, 79
	call _sysy_stoptime
	lw a0, 88(sp)
	add s3, s7, s6
	li s11, 1
	sub s3, s3, s11
	mv a0, s3
	la a1, v4
	call putarray
	lw a0, 88(sp)
	lw a1, 84(sp)
	li a0, 0
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw s5, 20(sp)
	lw s6, 24(sp)
	lw s7, 28(sp)
	lw ra, 220(sp)
	addi sp, sp, 224
	ret
	.size main, .-main
