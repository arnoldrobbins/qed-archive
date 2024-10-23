/*% cc -c -O %
 */
#include "vars.h"
#define SIGINTR	2
bufinit(n)
{
	struct buffer *bufp;
	register *fend;
	fend=n;
	for(bufp=buffer;bufp!=buffer+NBUFS;bufp++){
		bufp->zero=fend;
		bufp->dot=fend;
		bufp->dol=fend;
		bufp->cflag = FALSE;
	}
}
chngbuf(bb)
	int bb;
{
	syncbuf();
	newbuf(bb);
	savedfile = FILE(curbuf-buffer);
}
newbuf(bb){
	curbuf = buffer+bb;
	zero=curbuf->zero;
	dot=curbuf->dot;
	dol=curbuf->dol;
	cflag = curbuf->cflag;
}
fixbufs(n)
	register n;
{
	register struct buffer *bufp;
	lock++;
	for(bufp=curbuf+1;bufp!=buffer+NBUFS;bufp++){
		bufp->zero=+n;
		bufp->dot=+n;
		bufp->dol=+n;
	}
	unlock();
}
syncbuf()
{
	/* curbuf->zero = zero;	/* we never assign to it, so needn't save */
	curbuf->dot=dot;
	curbuf->dol=dol;
	curbuf->cflag = cflag!=FALSE;	/* Normalize to fit in a char */
}
error(code)
{
	register c;
	register struct stack *b;
	unlock();
	errok=FALSE;
	if(code){
		for(;stackp != stack ;--stackp)
			if(stackp->type == BUF || stackp->type == STRING){
				putchar('?');
				if(stackp->type == BUF){
					putchar('b');
					putchar(bname[stackp->bufptr - buffer]);
					count = stackp->lineno;
					putd();
					putchar('.');
				}else{
					putchar('z');
					putchar(bname[stackp->lineno]);
				}
				count = stackp->charno;
				putd();
				putchar(' ');
			}
		putchar('?');
		putchar(code&~FILERR);
		if(code&FILERR){
			putchar(' ');
			putl(string[FILEBUF].str);
		} else
			putchar('\n');
	}
	nestlevel = 0;
	listf = FALSE;
	gflag = FALSE;
	biggflag = FALSE;
	stackp = stack;
	count = 0L;
	if(code && code!='?'){		/* if '?', system cleared tty input buf */
		while(lastttyc!='\n' && lastttyc!=EOF){
			litflag = TRUE;	/* in case he types a \b */
			getchar();
		}
	}
	litflag = FALSE;
#ifdef	V7
	lseek(0, 0L, 2);
#endif
#ifndef	V7
	seek(0, 0, 2);
#endif
	if (io > 0) {
		close(io);
		io = -1;
	}
	appflag=0;
	reset();
}
init()
{
	register char *p;
	register pid;
	lock++;
	close(tfile);
	tfname = "/tmp/qxxxxx";
	pid = getpid();
	srand(pid);
	for (p = &tfname[11]; p > &tfname[6];) {
		*--p = (pid%10) + '0';
		pid =/ 10;
	}
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	brk(fendcore);
	bufinit(fendcore);
	newbuf(0);
	lastdol=dol;
	endcore = fendcore - 2;
	stackp=stack;
	stackp->type=TTY;
	unlock();
}
comment()
{
	register c, mesg;

	c = getchar();
	mesg = 0;
	if(c == '\"') {
		mesg++;
		c = getchar();
	}
	while(c != '\n' && c != '\"') {
		if(mesg)
			putchar(c);
		c = getchar();
	}
	if(mesg) {
		if(c == '\n')
			putchar(c);
		flush();
	}
}
abs(n)
register n;
{
	return(n<0?-n:n);
}
