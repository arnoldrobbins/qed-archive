	data	1
	comm	env,44
	comm	special,0
	comm	buffer,784
	comm	curbuf,4
	comm	string,702
	comm	strarea,1026
	comm	stack,160
	comm	stackp,4
	comm	peekc,2
	comm	lastc,2
	comm	line,70
	comm	linp,4
	comm	savedfil,2
	comm	linebuf,512
	comm	zero,4
	comm	dot,4
	comm	dol,4
	comm	lastdol,4
	comm	endcore,4
	comm	fendcore,4
	comm	addr1,4
	comm	addr2,4
	comm	genbuf,512
	comm	linebp,4
	comm	ttybuf,6
	comm	monocase,2
	comm	ninbuf,2
	comm	io,2
	comm	onhup,2
	comm	onquit,2
	comm	onintr,2
	comm	lasterr,1
	comm	appflag,2
	comm	cflag,2
	comm	cprflag,2
	comm	dflag,2
	comm	eflag,2
	comm	gflag,2
	comm	biggflag,2
	comm	iflag,2
	comm	prflag,2
	comm	tflag,2
	comm	uflag,2
	comm	vflag,2
	comm	initflag,2
	comm	nestleve,2
	comm	lastttyc,2
	comm	listf,2
	comm	tfile,2
	comm	tfname,4
	comm	loc1,4
	comm	loc2,4
	comm	names,112
	comm	braslist,36
	comm	braelist,36
	comm	nbra,2
	comm	oneline,2
	comm	lock,2
	comm	bname,0
	comm	nflag,2
	comm	nchars,0
	comm	bbempty,2
	comm	strstart,4
	text
	global	length
length:
	link	%fp,&F%1
	mov.w	&M%1,-2(%fp)
	movm.l	&M%1,S%1(%fp)
#	line 9, file "string.c"
	mov.l	8(%fp),%a2
#	line 11, file "string.c"
	lea.l	(%a2),%a3
	mov.l	%a3,%d0
	bne	L%83
#	line 12, file "string.c"
	mov.l	&0,%d0
	br	L%82
L%83:
L%86:
L%85:
#	line 13, file "string.c"
	tst.b	(%a3)+
	bne	L%86
L%84:
#	line 14, file "string.c"
	mov.l	%a3,%d0
	sub.l	%a2,%d0
	mov.l	%d0,%d0
	sub.w	&1,%d0
	br	L%82
L%82:
	movm.l	S%1(%fp),&M%1
	unlk	%fp
	rts
	set	S%1,-10
	set	T%1,-10
	set	F%1,-14
	set	M%1,06000
	data	1
	text
	global	startstr
startstr:
	link	%fp,&F%2
	mov.w	&M%2,-2(%fp)
	movm.l	&M%2,S%2(%fp)
#	line 17, file "string.c"
	mov.l	string+698,strstart
L%88:
	movm.l	S%2(%fp),&M%2
	unlk	%fp
	rts
	set	S%2,-6
	set	T%2,-6
	set	F%2,-10
	set	M%2,02000
	data	1
	text
	global	addstrin
addstrin:
	link	%fp,&F%3
	mov.w	&M%3,-2(%fp)
	movm.l	&M%3,S%3(%fp)
#	line 20, file "string.c"
	cmp.l	string+698,&strarea+1026
	bne	L%91
#	line 21, file "string.c"
	jsr	strcompa
L%91:
#	line 22, file "string.c"
	mov.w	8(%fp),%d0
	mov.l	string+698,%a1
	add.l	&1,string+698
	mov.b	%d0,(%a1)
L%90:
	movm.l	S%3(%fp),&M%3
	unlk	%fp
	rts
	set	S%3,-6
	set	T%3,-6
	set	F%3,-10
	set	M%3,02000
	data	1
	text
	global	dropstri
dropstri:
	link	%fp,&F%4
	mov.w	&M%4,-2(%fp)
	movm.l	&M%4,S%4(%fp)
#	line 25, file "string.c"
	mov.l	&1,%d0
	sub.l	%d0,string+698
L%94:
	movm.l	S%4(%fp),&M%4
	unlk	%fp
	rts
	set	S%4,-6
	set	T%4,-6
	set	F%4,-10
	set	M%4,02000
	data	1
	text
	global	cpstr
cpstr:
	link	%fp,&F%5
	mov.w	&M%5,-2(%fp)
	movm.l	&M%5,S%5(%fp)
#	line 29, file "string.c"
	mov.l	8(%fp),%a3
#	line 29, file "string.c"
	mov.l	12(%fp),%a4
L%99:
L%98:
#	line 30, file "string.c"
	mov.l	%a4,%a0
	add.l	&1,%a4
	mov.b	(%a3)+,(%a0)
	mov.b	(%a0),%d0
	ext.w	%d0
	tst.b	%d0
	bne	L%99
L%97:
L%96:
	movm.l	S%5(%fp),&M%5
	unlk	%fp
	rts
	set	S%5,-14
	set	T%5,-14
	set	F%5,-18
	set	M%5,016000
	data	1
	text
	global	shiftstr
shiftstr:
	link	%fp,&F%6
	mov.w	&M%6,-2(%fp)
	movm.l	&M%6,S%6(%fp)
#	line 34, file "string.c"
	mov.l	&string,%a5
L%104:
#	line 34, file "string.c"
	cmp.l	%a5,&string+696
	bhi	L%103
#	line 35, file "string.c"
	tst.w	8(%fp)
	beq	L%105
#	line 36, file "string.c"
	add.l	&strarea,2(%a5)
	br	L%106
L%105:
#	line 38, file "string.c"
	sub.l	&strarea,2(%a5)
L%106:
L%102:
#	line 38, file "string.c"
	add.l	&6,%a5
	br	L%104
L%103:
L%101:
	movm.l	S%6(%fp),&M%6
	unlk	%fp
	rts
	set	S%6,-18
	set	T%6,-18
	set	F%6,-22
	set	M%6,036000
	data	1
	text
	global	clearstr
clearstr:
	link	%fp,&F%7
	mov.w	&M%7,-2(%fp)
	movm.l	&M%7,S%7(%fp)
#	line 41, file "string.c"
	mov.l	&string,%a0
	mov.w	8(%fp),%d1
	muls.w	&6,%d1
	lea.l	0(%a0,%d1.l),%a0
	mov.l	&0,%d1
	mov.w	%d1,(%a0)
#	line 42, file "string.c"
	mov.w	8(%fp),%d0
	muls.w	&6,%d0
	mov.l	&string+2,%a1
	mov.l	&strarea,0(%a1,%d0.l)
L%108:
	movm.l	S%7(%fp),&M%7
	unlk	%fp
	rts
	set	S%7,-14
	set	T%7,-14
	set	F%7,-18
	set	M%7,016000
	data	1
	text
	global	copystri
copystri:
	link	%fp,&F%8
	mov.w	&M%8,-2(%fp)
	movm.l	&M%8,S%8(%fp)
#	line 46, file "string.c"
	mov.l	8(%fp),%a5
L%111:
#	line 47, file "string.c"
	tst.b	(%a5)
	beq	L%112
#	line 48, file "string.c"
	mov.b	(%a5)+,%d0
	ext.w	%d0
	mov.w	%d0,(%sp)
	jsr	addstrin
	br	L%111
L%112:
L%110:
	movm.l	S%8(%fp),&M%8
	unlk	%fp
	rts
	set	S%8,-18
	set	T%8,-18
	set	F%8,-22
	set	M%8,036000
	data	1
	text
	global	eqstr
eqstr:
	link	%fp,&F%9
	mov.w	&M%9,-2(%fp)
	movm.l	&M%9,S%9(%fp)
L%115:
#	line 53, file "string.c"
	mov.l	8(%fp),%a0
	tst.b	(%a0)
	beq	L%116
#	line 54, file "string.c"
	mov.l	12(%fp),%a0
	add.l	&1,12(%fp)
	mov.l	8(%fp),%a1
	add.l	&1,8(%fp)
	mov.b	(%a1),%d1
	cmp.b	%d1,(%a0)
	beq	L%117
#	line 55, file "string.c"
	mov.l	&0,%d0
	br	L%114
L%117:
	br	L%115
L%116:
#	line 56, file "string.c"
	mov.l	12(%fp),%a0
	tst.b	(%a0)
	bne	L%118
	mov.l	&1,%d0
	br	L%119
L%118:
	mov.l	&0,%d0
L%119:
	br	L%114
L%114:
	movm.l	S%9(%fp),&M%9
	unlk	%fp
	rts
	set	S%9,-18
	set	T%9,-18
	set	F%9,-22
	set	M%9,036000
	data	1
	text
	global	dupstrin
dupstrin:
	link	%fp,&F%10
	mov.w	&M%10,-2(%fp)
	movm.l	&M%10,S%10(%fp)
#	line 64, file "string.c"
	mov.w	8(%fp),%d0
	muls.w	&6,%d0
	mov.l	&string,%a1
	mov.w	0(%a1,%d0.l),%d0
	ext.l	%d0
	mov.l	string+698,%a1
	lea.l	0(%a1,%d0.l),%a0
	cmp.l	%a0,&strarea+1026
	bls	L%122
#	line 65, file "string.c"
	jsr	strcompa
L%122:
#	line 66, file "string.c"
	mov.w	8(%fp),%d0
	muls.w	&6,%d0
	mov.l	&string+2,%a1
	mov.l	0(%a1,%d0.l),(%sp)
	jsr	copystri
L%121:
	movm.l	S%10(%fp),&M%10
	unlk	%fp
	rts
	set	S%10,-18
	set	T%10,-18
	set	F%10,-22
	set	M%10,036000
	data	1
	text
	global	setstrin
setstrin:
	link	%fp,&F%11
	mov.w	&M%11,-2(%fp)
	movm.l	&M%11,S%11(%fp)
#	line 69, file "string.c"
	mov.l	&0,%d0
	mov.w	%d0,(%sp)
	jsr	addstrin
#	line 70, file "string.c"
	mov.l	&string,-4+T%11(%fp)
	mov.l	strstart,(%sp)
	jsr	length
	mov.w	8(%fp),%d1
	muls.w	&6,%d1
	add.l	%d1,-4+T%11(%fp)
	mov.l	-4+T%11(%fp),%a1
	mov.w	%d0,(%a1)
	bne	L%125
#	line 71, file "string.c"
	mov.w	8(%fp),%d0
	muls.w	&6,%d0
	mov.l	&string+2,%a1
	mov.l	&strarea,0(%a1,%d0.l)
	br	L%126
L%125:
#	line 73, file "string.c"
	mov.w	8(%fp),%d0
	muls.w	&6,%d0
	mov.l	&string+2,%a1
	mov.l	strstart,0(%a1,%d0.l)
L%126:
#	line 74, file "string.c"
	cmp.l	string+698,&strarea+1026
	blo	L%127
#	line 75, file "string.c"
	jsr	strcompa
L%127:
L%124:
	movm.l	S%11(%fp),&M%11
	unlk	%fp
	rts
	set	S%11,-18
	set	T%11,-18
	set	F%11,-26
	set	M%11,036000
	data	1
	text
	global	strcompa
strcompa:
	link	%fp,&F%12
	mov.w	&M%12,-2(%fp)
	movm.l	&M%12,S%12(%fp)
#	line 81, file "string.c"
	add.w	&1,lock
#	line 82, file "string.c"
	mov.l	&strarea+2,-12+S%12(%fp)
L%131:
#	line 84, file "string.c"
	mov.l	&strarea+1026,-16+S%12(%fp)
#	line 85, file "string.c"
	mov.l	&string,-4+S%12(%fp)
L%134:
#	line 85, file "string.c"
	cmp.l	-4+S%12(%fp),&string+696
	beq	L%133
#	line 86, file "string.c"
	mov.l	-4+S%12(%fp),%a0
	mov.l	-12+S%12(%fp),%a1
	cmp.l	%a1,2(%a0)
	bhi	L%135
	mov.l	-4+S%12(%fp),%a0
	mov.l	2(%a0),%a0
	cmp.l	%a0,-16+S%12(%fp)
	bhs	L%135
#	line 87, file "string.c"
	mov.l	-4+S%12(%fp),%a0
	mov.l	2(%a0),-16+S%12(%fp)
#	line 88, file "string.c"
	mov.l	-4+S%12(%fp),-8+S%12(%fp)
L%135:
L%132:
#	line 90, file "string.c"
	add.l	&6,-4+S%12(%fp)
	br	L%134
L%133:
#	line 90, file "string.c"
	cmp.l	-16+S%12(%fp),&strarea+1026
	bne	L%136
	br	L%130
L%136:
#	line 92, file "string.c"
	mov.l	-8+S%12(%fp),%a0
	mov.l	-12+S%12(%fp),2(%a0)
L%139:
L%138:
#	line 93, file "string.c"
	mov.l	-16+S%12(%fp),%a0
	add.l	&1,-16+S%12(%fp)
	mov.l	-12+S%12(%fp),%a1
	add.l	&1,-12+S%12(%fp)
	mov.b	(%a0),(%a1)
	mov.b	(%a1),%d0
	ext.w	%d0
	tst.b	%d0
	bne	L%139
L%137:
L%129:
	br	L%131
L%130:
#	line 95, file "string.c"
	mov.l	strstart,-16+S%12(%fp)
#	line 96, file "string.c"
	mov.l	-12+S%12(%fp),strstart
L%140:
#	line 97, file "string.c"
	mov.l	-16+S%12(%fp),%a0
	cmp.l	%a0,string+698
	beq	L%141
#	line 98, file "string.c"
	mov.l	-16+S%12(%fp),%a0
	add.l	&1,-16+S%12(%fp)
	mov.l	-12+S%12(%fp),%a1
	add.l	&1,-12+S%12(%fp)
	mov.b	(%a0),(%a1)
	br	L%140
L%141:
#	line 99, file "string.c"
	mov.l	-12+S%12(%fp),string+698
#	line 100, file "string.c"
	cmp.l	-12+S%12(%fp),&strarea+1026
	bne	L%142
#	line 101, file "string.c"
	mov.l	strstart,string+698
#	line 102, file "string.c"
	mov.l	&90,%d0
	mov.w	%d0,(%sp)
	jsr	error
L%142:
#	line 104, file "string.c"
	jsr	unlock
L%128:
	movm.l	S%12(%fp),&M%12
	unlk	%fp
	rts
	set	S%12,-18
	set	T%12,-34
	set	F%12,-38
	set	M%12,036000
	data	1
