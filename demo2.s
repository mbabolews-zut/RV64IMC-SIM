addi sp, sp, -16

addi s0, zero, 16
addi s1, zero, 0
gen_rand_loop:
	# generate random number from range [-110; 110]
	addi a0, zero, 100
	addi a1, zero, -110
	addi a2, zero, 110
	ecall

	# print the number
	add a1, zero, a0
	addi a0, zero, 1
	ecall
	
	# store the number
	sb a1, 0(sp)
	addi sp, sp, 1

	jal ra, print_space
	
	addi s1, s1, 1
	bne s0, s1, gen_rand_loop

jal ra, print_nl

# bubble sort
addi s0, sp, -1
addi s1, sp, -16
outer_loop:
	addi s2, s1, 1
	lb t0, 0(s1)
inner_loop:
		lb t1, 0(s2)
		bge t1, t0, inner_inc

			sb t1, 0(s1)
			sb t0, 0(s2)
			add t0, zero, t1
inner_inc: 
		addi s2, s2, 1
		bne s2, sp, inner_loop
outer_inc:
	addi s1, s1, 1
	bne s0, s1, outer_loop
		  
		
addi a0, sp, -16
addi a1, zero, 16
jal ra, print_array
jal zero, end 

# helpers
print_nl:
	addi a0, zero, 11
	addi a1, zero, 0xA
	ecall
	jalr zero, ra, 0

print_space:
	addi a0, zero, 11
	addi a1, zero, 0x20
	ecall
	jalr zero, ra, 0

print_array:
	addi t4, ra, 0 # preserve return address
	add t0, zero, a0
	add t1, a0, a1
	print_arr_loop:
		addi a0, zero, 1
		lb a1, 0(t0)
		ecall
		jal ra, print_space
		addi t0, t0, 1
		bne t1, t0, print_arr_loop
	jal ra, print_nl
	jalr zero, t4, 0

end: