.data
b:
	.space 12
c:
	.space 40
a:
	.space 20
d:
	.space 20
.text
	j main
main:
	la t0, b
	la t1, c
	li t2, 0
	mv t3, t2
	la t4, a
	la t5, d
	li t6, 0
	li s2, 10
	sub s3, t6, s2
	addi s4, t5, 0
	mv s5, s3
	sw s5, 0(s4)
	li s6, 0
	li s7, 100
	sub s8, s6, s7
	addi s9, t5, 4
	mv s10, s8
	sw s10, 0(s9)
	addi s11, t5, 8
	li t2, 12
	sw t2, 0(s11)
	addi t6, t5, 12
	li s2, 33
	sw s2, 0(t6)
	addi s3, t5, 16
	li s4, 5
	sw s4, 0(s3)
label0:	li s5, 5
	blt t3, s5, label1
	j label2
label1:	li s6, 4
	mul s7, s6, t3
	li s8, 4
	mul s9, s8, t3
	la s11, d
	add s10, s11, s9
	lw t2, 0(s10)
	la s2, a
	add t6, s2, s7
	sw t2, 0(t6)
	li s3, 1
	add s4, t3, s3
	mv t3, s4
	j label0
label2:	li s5, 0
	mv t3, s5
label3:	li s6, 4
	blt t3, s6, label4
	j label5
label4:	li s7, 1
	add s8, t3, s7
	mv s9, s8
label6:	li s10, 5
	blt s9, s10, label7
	j label8
label7:	li t2, 4
	mul t6, t2, t3
	la s4, a
	add s3, s4, t6
	lw s5, 0(s3)
	li s6, 4
	mul s7, s6, s9
	la s10, a
	add s8, s10, s7
	lw t2, 0(s8)
	bgt s5, t2, label9
	j label10
label9:	li t6, 4
	mul s3, t6, t3
	la s6, a
	add s5, s6, s3
	lw s7, 0(s5)
	mv s8, s7
	
	li t2, 4
	mul t6, t2, t3
	li s3, 4
	mul s5, s3, s9
	la s8, a
	add s7, s8, s5
	lw t2, 0(s7)
	la s3, a
	add t6, s3, t6
	sw t2, 0(t6)
	li s7, 4
	mul t2, s7, s9
	la s5, a
	add t6, s5, t2
	sw s7, 0(t6)
label10:	li t2, 1
	add t6, s9, t2
	mv s9, t6
	j label6
label8:	li s7, 1
	add t2, t3, s7
	mv t3, t2
	j label3
label5:	li t6, 0
	mv t3, t6
label11:	li s7, 5
	blt t3, s7, label12
	j label13
label12:	li t2, 4
	mul t6, t2, t3
	la t2, a
	add s7, t2, t6
	lw t6, 0(s7)
	addi sp, sp, -8
	sw a0, 0(sp)
	sw a1, 4(sp)
	li a0, 1
	mv a1, t6
	ecall
	lw a0, 0(sp)
	lw a1, 4(sp)
	addi sp, sp, 8
	li s7, 1
	add t6, t3, s7
	mv t3, t6
	j label11
label13:	li s7, 0
	li a0, 10
	li a1, 0
	ecall
