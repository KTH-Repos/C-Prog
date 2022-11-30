  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

  #
  # Please add your own code below this line
  #
  	.data
  	.align 2
 	.text
 	
 	.global hexasc
 	.global delay
 	.global time2string
 	 
hexasc:		
	li $t0, 9
	andi $t4, $a0, 0xf	# Kollar enbart på de första 4 bitarna ( 0000 xxxx )
	
	slt $t0, $t0, $t4 	# t1 får värdet 1 ifall: 9 < a0
	beq $t0, 0, lessthanten
	
	li $t1, 0x37
	add $v0, $t1, $t4	
	jr $ra
	nop
lessthanten:
	li $t1, 0x30
	add $v0, $t4, $t1
	jr $ra
	nop
delay:
	move $t0, $a0
	
	while: 
	blt $t0, 1, exitwhile
	
	 sub  $t0, $t0, 1
	 li $t1, 0
		for:
		 bgt $t1, 4400, exitfor 
		 addi $t1, $t1, 1
		 j for
		 nop
		 exitfor:
	 j while
	 nop
	exitwhile:

 jr $ra
 nop
 
time2string:
	# MINUTER	
	andi $t0, $a1, 0xffff	# LSB Kollar enbart på de första 16 bitarna ( 0000 xxxx ) 
	srl  $t0, $t0, 12	# Vi tar bort 12 bitar åt höger
	
	PUSH $ra
	PUSH $a0
	move $a0, $t0
	jal hexasc
	nop
	move $t0, $v0		# copy return value to argument register
	POP $a0
	POP $ra

	sb $t0, 0($a0)
	
	andi $t0, $a1, 0xffff	# LSB Kollar enbart på de första 16 bitarna ( 0000 xxxx ) 
	srl  $t0, $t0, 8	# Vi tar bort 8 bitar åt höger
	PUSH $ra
	PUSH $a0
	move $a0, $t0
	jal hexasc
	nop
	move $t0, $v0		# copy return value to argument register
	POP $a0
	POP $ra
	sb $t0, 1($a0)
	
	# COLON
	li $t1, 0x3A
	sb $t1, 2($a0)
	
	# SEKUNDER	
	
	#Skriver ut x0
	andi $t2, $a1, 0xff	# LSB Kollar enbart på de första 8 bitarna ( 0000 xxxx ) 
	srl  $t2, $t2, 4	# Vi tar bort 4 bitar åt höger
	PUSH $ra	
	PUSH $a0
	move $a0, $t2
	jal hexasc
	nop
	POP $a0
	POP $ra
	move $t2, $v0
	sb $t2, 3($a0)
	
	#Skriver ut 0x
	andi $t2, $a1, 0xff	# LSB Kollar enbart på de första 8 bitarna ( 0000 xxxx )
	PUSH $ra	
	PUSH $a0
	move $a0, $t2
	jal hexasc
	nop
	POP $a0
	POP $ra
	move $t2, $v0
	sb $t2, 4($a0)

	# NULL BYTE		
	li $t3, 0x00
	sb $t3, 5($a0)
	
	lb $t0, 3($a0)
	lb $t1, 4($a0)
	
	beq $t0, 0x30, special
	
	jr $ra
	nop
	
	special:
		beq $t1, 0x30, correct
	jr $ra
	nop 
	
	correct:
		li $t0, 0x58 # X i ASCII'
		li $t1, 0x00 
		sb $t0, 5($a0)
		sb $t1, 6($a0)
	jr $ra
	nop


