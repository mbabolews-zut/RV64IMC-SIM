addi x5, x0, 5  # x5 = 5
addi x6, x0, 5  # x6 = 5
addi x7, x0, 0  ; x7 = 0

addi x8, x0, 1
beq  x5, x6, 8
addi x7, x7, 1
skip:
mul  x7, x7, x8
addi x7, x7, 2
