addi x5, x0, 5  # x5 = 5
addi x6, x0, 3  # x6 = 3
addi x7, x0, 0  ; x7 = 0

addi x8, x0, 2
beq  x5, x6, 12
addi x7, x7, 1
mul  x7, x7, x8
addi x7, x7, 2
