/*% cc -c -O %
 */
#include "vars.h"
#ifdef	PDP11
typedef	long ulong;
#else
typedef	unsigned long ulong;
#endif
int	col;
putdn(i)
{
	putlong((ulong)i);
	putchar('\n');
}
#ifdef	PDP11
/*
 *	In version 6, ldiv() is in the library. In version 7, it is separate:
	.globl	_ldiv, _ldivr
	_ldiv:
		mov	2(sp), r0
		mov	4(sp), r1
		div	6(sp), r0
		mov	r1,_ldivr
		rts	pc
	.bss
	_ldivr:	.=.+2
 */
putlong(i)
	long i;
{
	register char r;
	extern int ldiv(), ldivr;

	/* the following pornography saves bundles of memory */
	i = ldiv(i,10);
	r = ldivr + '0';
	if (i)
		putlong(i);
	putchar(r);
}
#endif
#ifndef	PDP11
putlong(i)
	ulong i;
{
	register r;
	r = i%10;
	i /= 10;
	if(i)
		putlong(i);
	putchar('0'+r);
}
#endif
putl(sp)
	register char *sp;
{
	listf++;
	puts(sp);
	listf = FALSE;
}
puts(sp)
	register char *sp;
{
	col = 0;
	while (*sp)
		putchar(*sp++);
	putchar('\n');
}
display(lf)
{
	register int *a1;
	register int r;
	register char *p;
	register i;
	int nf;
	listf = (lf == 'l' || lf == 'L');
	nf = (lf == 'P' || lf == 'L');
	lf = listf;
	setdot();
	nonzero();
	a1 = addr1;
	r = (a1 - zero) & 077777;
	do{
		col = 0;
		if(nf){
			putlong((ulong)r++);
			for(i=0; i<NBUFS; i++)
				if((*a1|01) == names[i]){
					putchar('\'');
					putchar(bname[i]);
				}
			listf = 0;
			putchar('\t');
			col = 8;
			listf = lf;
		}
		for(p = getline(*a1++,linebuf);*p;putchar(*p++));
		putchar('\n');
	}while (a1 <= addr2);
	dot = addr2;
	listf = FALSE;
}
putct(c){
	putchar(c);
	putchar('\t');
}
putchar(c)
register char c;
{
	register char *lp;

	lp = linp;
	if (listf) {
		if (c=='\n') {
			if(linp!=line && linp[-1]==' ') {
				*lp++ = '\\';
				*lp++ = 'n';
			}
		} else {
			if (col >= (72-4-2)) {
				*lp++ = '\\';
				*lp++ = '\n';
				*lp++ = '\t';
				col = 8;
			}
			col++;
			if (c=='\b' || c=='\\' || c=='\t') {
				*lp++ = '\\';
				c = c=='\b'? 'b' : c=='\t'? 't' : '\\';
				col++;
			} else if ((c&0200) || c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ = ((c>>6)&03)+'0';
				*lp++ = ((c>>3)&07)+'0';
				c     = ( c    &07)+'0';
				col += 3;
			}
		}
	}
	*lp++ = c;
	if(c == '\n' || lp >= &line[(sizeof line)-2-4]) {
		linp = lp;
		flush();
		lp = linp;
	}
	linp = lp;
}
flush()
{
	if(linp != line){
		write(1, line, linp-line);
		linp = line;
	}
}
