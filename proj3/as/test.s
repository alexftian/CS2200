main:	!
	add $a0, $zero, $zero	!
	addi $a1, $zero, 11	!
loop:	!
	beq $a0, $a1, done	!
	sw $a0, 0($a0)		!
	addi $a0, $a0, 1	!
done:	!
