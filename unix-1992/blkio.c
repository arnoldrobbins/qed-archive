/*% cc -c -O %
 */
#include "vars.h"
#define BLKSIZE	512
#ifdef	BIGTMP
#define	MAXBLOCKS	4095
#else
#define	MAXBLOCKS	255
#endif
#define	BLMASK		MAXBLOCKS
char	ibuff[512];
int	iblock = -1;
int	oblock = 0;
char	obuff[512];
int	ooff;		/* offset of next byte in obuff */
initio()
{
	lock++;
	iblock = -1;
	oblock = 0;
	ooff = 0;
	unlock();
}
char *getline(tl, lbuf)
	int tl;
	char *lbuf;
{
	register char *bp, *lp;
	register int nl;
	extern int read();

	lp = lbuf;
	nl = -((tl<<1) & 0774);
	tl = (tl>>8) & BLMASK;
	do {
		if (nl<=0) {
			if (tl==oblock)
				bp = obuff;
			else {
				bp = ibuff;
				if (tl!=iblock) {
					iblock = -1;	/* signal protection */
					blkio(tl, bp, read);
					iblock = tl;
				}
			}
			tl++;
			bp -= nl;
			nl += BLKSIZE;
		}
		nl--;
	} while (*lp++ = *bp++);
	return(lbuf);
}
int putline()
{
	register char *op, *lp;
	register int r;
	extern int write();

	modified();
	lp = linebuf;
	r = (oblock<<8) + (ooff>>1);	/* ooff may be 512! */
	op = obuff + ooff;
	lock++;
	do {
		if (op >= obuff+BLKSIZE) {
			/* delay updating oblock until after blkio succeeds */
			blkio(oblock, op=obuff, write);
			oblock++;
			ooff = 0;
		}
		if((*op = *lp++) == '\n') {
			*op++ = '\0';
			linebp = lp;
			break;
		}
	} while (*op++);
	ooff = (((op-obuff)+3)&~3);
	unlock();
	return (r);
}
blkio(b, buf, iofcn)
	char *buf;
	int (*iofcn)();
{
	if (b>=MAXBLOCKS
	|| (lseek(tfile, ((long) b) * 512L, 0)<0L)
	|| (*iofcn)(tfile, buf, 512) != 512) {
		error('T');
	}
}
