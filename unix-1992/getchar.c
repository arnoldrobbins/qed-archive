/*% cc -c -O %
 */
#include "vars.h"
char digits[] = "0123456789";	/* getnum() & atoi() work even on non-ASCII systems */
getnum()
{
	register n, i;
	n=0;
	while((i=posn(nextchar(), digits)) >= 0){
		getchar();
		n = n*10 + i;
	}
	return(n);
}

getsigned(){
	register sign;
	if(nextchar()=='-'){
		getchar();
		sign = -1;
	}
	else
		sign = 1;
	return(getnum()*sign);
}

atoi(s)
	register char *s;
{
	register n, i;
	int sign;

	n = 0;
	sign = 1;
	if(*s == '-'){
		sign = -1;
		s++;
	}
	while(*s){
		i = posn(*s++, digits);
		if(i < 0)
			return(sign*n);	/* error handling done elsewhere */
		n = n*10 + i;
	}
	return(sign*n);
}

alldigs(s)
	register char *s;
{
	if(*s == '-')
		s++;
	while(*s)
		if(posn(*s++, digits) < 0)
			return(FALSE);
	return(TRUE);
}

char bname[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~";
getname(e)
{
	int getc();
	return(getnm(e, getc));
}
getaz(e)
{
	int getchar();
	return(getnm(e, getchar));
}
getnm(e, f)
	int (*f)();
{
	register i;
	i = posn((*f)(), bname);
	if(i < 0)
		error(e);
	return(i);
}
char	special[] = "xgBbBcfFlprzN\"\\'";
int	qcount;
int	afterq;
getchar()
{
	register c;
	register peek;

	peek=peekc;
	c=getc();
	if(tflag && !peek && stackp!= &stack[0]){
		if(c==EOF)
			puts("[*EOF*]");
		else
			putchar(c);
	}
	return(c);
}
getc()
{
	register c;
	register struct buffer *bufp;
	register struct stack *sp;
	int *lp;
	int numeric;
	int delta;
	int literal;
	int i;
	char *p;
	static char bufbuf[LBSIZE];

	if(c = peekc)	/* assignment = */
		peekc = 0;
	else if(qcount) {
		--qcount;
		c = '\\';
	} else if(c = afterq)	/* assignment = */
		afterq = 0;
	else {
		for(;;popinp()){
			sp=stackp;
			switch(sp->type){
			case TTY:
			case XTTY:
				if(cprflag && sp == &stack[0] &&
					lastttyc == '\n')
					write(2, "*", 1);
				c = ttyc();
				if(sp != &stack[0] && c == '\n') {
					if(stackp->type == TTY)
						continue;
					else		/* XTTY */
						popinp();
				}
				break;
			case GLOB:
				if ((c = *stackp->globp++) == 0){
					popinp();
					c = EOF;
				}
				break;
			case STRING:
			case BFILEN:
			case PAT:
			case RHS:
			case BRWS:
			case FILEN:
				if(string[sp->strname].len < sp->charno){
					c=EOF;
					continue;
				}
				c=string[sp->strname].str[sp->charno++];
				if(c==0)
					continue;
				break;
			case BUF:
				bufp=sp->bufptr;
				if(bufp==curbuf)
					syncbuf();
				lp=bufp->zero+sp->lineno;
				if(bbempty){
					if(lp>bufp->dol)
						continue;
					p = getline(*lp, bufbuf);
					bbempty = FALSE;
					for(i=sp->charno; i-- && *p; p++)
						;
				}else
					p = bufbuf+sp->charno;
				sp->charno++;
				if((c = *p) == '\0'){
					/*
					 * Leave the last newline in the buffer out.
					 */
					bbempty = TRUE;
					if(lp>=bufp->dol)
						continue;
					sp->lineno++;
					sp->charno=0;
					c='\n';
				}
			}
			break;
		}
		literal = FALSE;
		if(c == '\\' && !stackp->literal){
		Getc:
			c = getc();
			i = posn(c, cspec);
			switch(i) {
			case LITERAL:
				if(posn(peekc=getc(), "bfFlprz!") >= 0)
					literal = TRUE;
				goto Getc;	/* business as usual... */
			case QUOTE:
				c = '\\';
				break;
			case NEWL:
				c = '\n';
				break;
			case STRING:
				delta = 0;
				numeric = 0;
				c = getc();
				if(c == '#'){
					numeric++;
					c = getc();
				}
				if(c == '+' || c == '-'){
					delta = 1;
					if(c == '-')
						delta = -delta;
					c=getc();
				}
				ungetchar(c);
				c = getname('z');
				if(delta){
					if(numeric){
						if(!alldigs(string[c].str))
							error('#');
						numset(c, atoi(string[c].str)+delta);
					} else
						strinc(c, delta);
				}
				pushinp(i, c, literal);
				literal = FALSE;
				c=getc();
				break;
			case BUF:
			case BFILEN:
				c = getname(cspec[i]);
				if(i == BUF)
					bbempty = TRUE;
				/* fall through */
			case TTY:
			case FILEN:
			case PAT:
			case RHS:
				pushinp(i, c, literal);
				literal = FALSE;
				/* fall through */
			case NOTHING:
				c = getc();
				break;
			case CURBN:
				c = bname[curbuf-buffer];
				break;
			case BACKSLASH:
				qcount++;
				break;
			default:
				afterq = c;
				c = '\\';
				break;
			}
		}
	}
	lastc = c;
	return(c);
}
ttyc()
{
	char c;
	initflag = 0;
	if(read(0, &c, 1) > 0)
		lastttyc = c&0177;
	else
		lastttyc = EOF;
	return(lastttyc);
}
posn(c, s)
	register char c;
	register char *s;
{
	register char *is;
	is = s;
	while(*s)
		if(c == *s++)
			return(s - is - 1);
	return(-1);
}
pushinp(type, arg, literal)
	register arg;
{
	register struct stack *s;

	s = ++stackp;
	if(s == stack+STACKSIZE)
		error('l');
	stackp->literal = literal;
	if(tflag){
		putchar('[');
		if(literal)
			putchar('\'');
		putchar(cspec[type]);
	}
	s->type=type;
	s->charno = 0;
	switch(type){
	case BFILEN:
		s->strname = FILE(arg);
		break;
	case STRING:
		s->strname = arg;
		break;
	case FILEN:
		s->strname = savedfile;
		break;
	case PAT:
		s->strname = SAVPAT;
		break;
	case RHS:
		s->strname = SAVRHS;
		break;
	case BRWS:
		s->strname = BROWSE;
		break;
	case BUF:
		if((s->bufptr=buffer+arg) == curbuf && appflag)
			error('\\');
		s->lineno=1;
		break;
	case GLOB:
		s->globp = (char *)arg;
		break;
	}
	if(tflag){
		if(type==BFILEN || type==STRING || type==BUF)
			putchar(bname[arg]);
		putchar('=');
	}
}
popinp(){
	if(stackp->type == BUF)
		bbempty = TRUE;
	if(tflag){
		putchar(']');
		flush();
	}
	--stackp;
}
gettty()
{
	register c, gf;
	register char *p;
	p = linebuf;
	gf = stackp->type;
	while((c=getchar()) != '\n') {
		if (c==EOF) {
			if (gf == GLOB)
				peekc=c;
			return(c);
		}
		if (c == 0)
			continue;
		*p++ = c;
		if (p >= &linebuf[LBSIZE-2])
			error('l');
	}
	*p++ = 0;
	if (!oneline && linebuf[0]=='.' && linebuf[1]==0)
		return(EOF);
	return(0);
}
getquote(p, f)
	register char *p;
	int (*f)();
{
	register c;
	c = (*f)();
	if(c == '\\') {
		if(peekc == 0)
			nextchar();	/* prime peekc */
		if(posn(peekc, p) >= 0) {
			c = peekc | 0200;
			(*f)();		/* clear peekc */
		}
	}
	return(c);
}
