	.globl	_ldiv, _ldivr
	_ldiv:
		mov	2(sp), r0
		mov	4(sp), r1
		div	6(sp), r0
		mov	r1,_ldivr
		rts	pc
	.bss
	_ldivr:	.=.+2
