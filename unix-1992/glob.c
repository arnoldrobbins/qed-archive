/*% cc -c -O %
 */
#include "vars.h"
#define	ALWAYS	2
#define GBSIZE	256
until(nfl, n)
{
	register c, type;
	char ubuf[GBSIZE];

	c = getchar();
	type=posn(c, "ft ");
	if(type < 0){
		if(nfl==0)
			error('x');
		ungetchar(c);
	}
	getglob(ubuf);
	settruth(FALSE);
	if(nfl && n<=0)
		return;	/* do it zero times */
	do ; while (exglob(ubuf, ubuf) &&
		   !((nfl && --n==0) || 
		     (type != ALWAYS && type == truth())));
}
global(k)
{
	register int *a1;
	char globuf[GBSIZE];
	struct buffer *startbuf;

	if(gflag++)
		error('g');
	setall();
	if(nextchar() == '\n')
		error('x');
	compile(getchar());
	getglob(globuf);
	for (a1=zero; ++a1<=dol;) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k)
			*a1 |= 01;
	}
	startbuf = curbuf;
	for (a1=zero; ++a1<=dol; ) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			if (!exglob(globuf, "p"))
				break;
			chngbuf(startbuf-buffer);
			a1 = zero;
		}
	}
	chngbuf(startbuf-buffer);
	gflag=FALSE;
}

globuf(k)
{
	register struct buffer *bp;
	char globbuf[GBSIZE];

	if (biggflag++)
		error('G');
	if (nextchar() =='\n')
		error('x');
	compile(getchar());
	getglob(globbuf);
	for (bp=buffer; bp < &buffer[NBUFS]; bp++)
		bp->gmark = FALSE;
	ncom('G');	/* mark interesting buffers */
	for (bp=buffer; bp < &buffer[NBUFS]; bp++)
		if (bp->gmark == k) {
			chngbuf(bp-buffer);
			if (!exglob(globbuf, "f\n"))
				break;
		}
	biggflag = FALSE;
}

getglob(globuf)
	char globuf[];
{
	register char *gp;
	register c;
	int getchar();
	gp = globuf;
	while ((c = getquote("\n", getchar)) != '\n') {
		if (c==EOF)
			error('x');
		*gp++ = c & 0177;
		if (gp >= &globuf[GBSIZE-2])
			error('l');
	}
	*gp++ = '\n';
	*gp++ = 0;
}

exglob(cmd, dflt)
	char *cmd, *dflt;
{
	register int nesting;

	pushinp(GLOB, eqstr(cmd,"\n")? dflt : cmd, FALSE);
	nesting = ++nestlevel;
	commands();
	if (nesting!=nestlevel)
		return(FALSE);
	nestlevel--;
	return(TRUE);
}
