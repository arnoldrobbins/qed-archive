#
/*% cc -s -O % -lS
/*
 * grep -- print lines matching (or not matching) a pattern
 *
 * brutally attacked by Rob Pike Jan/80 to correspond to ed.
 */
#include <stdio.h>

/*
 * This is the table of opcodes for compiled patterns.  Opcodes not greater
 * than STARABLE can have the * operator applied to by ORing the opcode with
 * STAR (01), so they must be even.
 */

#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define CFUNNY	10

#define	STAR	01
#define STARABLE CFUNNY

#define	CDOL	12
#define	CEOF	13
#define CBOI	14
#define CEOI	15
#define CSPACE	16

#define	Q	0177600	/* Quote bit; used for Character classes */
#define	LBSIZE	256
#define	ESIZE	256

#define	TRUE	1
#define	FALSE	0
char	ibuf[512];
char	expbuf[ESIZE];
long	lnum;
char	linebuf[LBSIZE+1];
int	bflag;
int	nflag;
int	cflag;
int	vflag;
int	fflag;
int	dflag;
int	foundflag;
int	nfile;
int	circfl;
long	blkno;
long	tln;

main(argc, argv)
char **argv;
{
	char buf[BUFSIZ];

	setbuf(stdout, buf);
	while (--argc > 0 && (++argv)[0][0]=='-')
		switch (argv[0][1]) {

		case 'v':
			vflag++;
			continue;

		case 'b':
			bflag++;
			continue;

		case 'c':
			cflag++;
			continue;

		case 'n':
			nflag++;
			continue;

		case 'f':
			fflag++;
			continue;

		case 'd':
			dflag++;
			continue;

		default:
			error("Unknown flag\n");
			continue;
		}
	if (argc<=0)
		error("Usage: grep [-b] [-c] [-d] [-f] [-n] [-v] RE [files]\n");
	compile(*argv);
	nfile = --argc;
	if (argc<=0)
		execute(0);
	else while (--argc >= 0) {
		argv++;
		execute(*argv);
	}
	exit(foundflag);
}

compile(str)
register char *str;
{
	register char c, *ep, *sp;
	char *penultep;
  	char *lastep;

	ep = expbuf;
	sp = str;
	if (*sp=='^') {
		circfl++;
		sp++;
	}
	for (;;) {
		c = *sp++;
		if (c=='\0') {
			*ep++ = CEOF;
			return;
		}
		if (ep >= &expbuf[ESIZE-5])
			goto cerror;
		penultep = lastep;
		lastep = ep;

		switch (c) {

		case '\\':
			switch (c= *sp++) {
			case '{':
				*ep++ = CBOI;
				continue;
			case '}':
				*ep++ = CEOI;
				continue;
			case '_':
				*ep++ = CSPACE;
				continue;
			case '!':
				*ep++ = CFUNNY;
				continue;
			default:
				*ep++ = CCHR;
				*ep++ = c;
				continue;
			}

		case '.':
			*ep++ = CDOT;
			continue;

		case '[':
			*ep++ = CCL;
			penultep = ep;
			*ep++ = 0;
			if ((c= *sp++) == '^') {
				c = *sp++;
				ep[-2] = NCCL;
			}
			do {
				if (c == '\0' || ep>=&expbuf[ESIZE-3])
					goto cerror;
				*ep++ = c;
				c = *sp++;
				if (c=='-') {
					c = *sp++;
					if (c == '\0' || c==']') {
						--sp;
						c = '-';
					} else {
						if (*--ep>c)
							goto cerror;
						*ep++ =| Q;
						*ep++ = c;
						c = *sp++;
					}
				}
			} while (c!=']');
			*penultep = ep-penultep;
			continue;

		case '*':
			if (penultep==0
			||  *penultep>STARABLE)
				goto cerror;
			*penultep =| STAR;
			lastep = 0;
			continue;

		case '+':
			if (penultep==0
			||  *penultep>STARABLE
			||  (ep-penultep)+ep >= &expbuf[ESIZE-1])
				goto cerror;
			do *ep++ = *penultep++; while (penultep!=lastep);
			*penultep =| STAR;
			lastep = 0;
			continue;

		case '$':
			if (*sp == '\0') {
				*ep++ = CDOL;
				continue;
			}
			/* fall through */
		default:
			if(dflag && (c|' ')>='a' && (c|' ')<='z'){
				*ep++ = CCL;
				*ep++ = 3;
				*ep++ = c;
				*ep++ = c^' ';
			} else {
				*ep++ = CCHR;
				*ep++ = c;
			}
			continue;
		}
	}
   cerror:
	error("RE error\n");
}

execute(file)
{
	register char *p1, *p2;
	register c;
	int f;
	char *ebp, *cbp;

	if (file) {
		if ((f = open(file, 0)) < 0) {
			error("Can't open %s\n", file);
		}
	} else
		f = 0;
	ebp = ibuf;
	cbp = ibuf;
	lnum = 0L;
	tln = 0L;
	blkno = -1L;
	for (;;) {
		lnum++;
		/*
		 * Flush output buffer every 512 input lines; nice
		 * when interacting.
		 */
		if((lnum&0377) == 0)
			fflush(stdout);
		p1 = linebuf;
		p2 = cbp;
		for (;;) {
			if (p2 >= ebp) {
				if ((c = read(f, ibuf, 512)) <= 0) {
					close(f);
					if (cflag) {
						if ((fflag && nfile == 1) || nfile > 1)
							printf("%s:", file);
						printf("%ld\n", tln);
					}
					return;
				}
				blkno++;
				p2 = ibuf;
				ebp = ibuf+c;
			}
			if ((c = *p2++) == '\n')
				break;
			if(c)
			if (p1 < &linebuf[LBSIZE-1])
				*p1++ = c;
		}
		*p1++ = 0;
		cbp = p2;
		p1 = linebuf;
		p2 = expbuf;
		if (circfl) {
			if (advance(p1, p2))
				goto Found;
			goto Not_found;
		}
		do {
			if(*p2 != CCHR || p2[1] == *p1) {
				if(advance(p1, p2))
					goto Found;
			}
		} while (*p1++);
Not_found:
		if (vflag)
			succeed(file);
		continue;
Found:
		if (vflag==0)
			succeed(file);
	}
}

int advance(lp, ep)
register char *lp, *ep;
{
	register char *curlp;

	for (;;) {
		curlp = lp;
		switch (*ep++) {

		case CCHR:
			if (*ep++ == *lp++)
				continue;
			return(FALSE);

		case CCHR|STAR:
			do ; while (*lp++ == *ep);
			ep++;
			break;

		case CDOT:
			if (*lp++)
				continue;
			return(FALSE);

		case CDOT|STAR:
			do ; while (*lp++);
			break;

		case CCL:
		case NCCL:
			if (cclass(ep, *lp++, ep[-1]==CCL)) {
				ep =+ *ep;
				continue;
			}
			return(FALSE);

		case CCL|STAR:
		case NCCL|STAR:
			do ; while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
			ep =+ *ep;
			break;

		case CFUNNY:
			if (*lp>=' ' && *lp!='\177' || *lp=='\t' || *lp=='\0')
				return(FALSE);
			lp++;
			continue;

		case CFUNNY|STAR:
			while (*lp<' ' && *lp && *lp!='\t'  ||  *lp=='\177')
				lp++;
			lp++;
			break;

		case CDOL:
			if (*lp==0)
				continue;
			return(FALSE);

		case CEOF:
			return(TRUE);

		case CBOI:
			if (alfmatch(*lp,0)
			&&  (lp==linebuf || !alfmatch(lp[-1],1)))
				continue;
			return(FALSE);

		case CEOI:
			if (!alfmatch(*lp,1)
			&&  lp!=linebuf && alfmatch(lp[-1],1))
				continue;
			return(FALSE);

		case CSPACE:
			if (*lp==' ' || *lp=='\t') {
				do lp++; while (*lp==' ' || *lp=='\t');
				continue;
				}
			return(FALSE);

		default:
			error("RE botch\n");
		}
		/* star logic: executed by falling out of switch */
		do {
			lp--;
			if (advance(lp, ep))
				return(TRUE);
		} while (lp > curlp);
		return(FALSE);
	}
}

int alfmatch(c,tail)
register char c;
{
	return (('a' <= c && c <= 'z')  ||
		('A' <= c && c <= 'Z')  ||
		(c == '_') ||
		(tail && '0' <= c && c<= '9'));
}

int cclass(aset, ac, af)
char *aset, ac;
{
	register char *set, c;
	register int n;

	set = aset;
	if ((c = ac) == 0)
		return(0);
	n = *set++;
	while (--n) {
		if (*set<0) {
			if ((*set++ & 0177) <= c) {
				if (c <= *set++)
					return(af);
			} else
				set++;
			--n;
		} else if (*set++ == c)
			return(af);
	}
	return(!af);
}

error(s, a)
{
	fprintf(stderr, "grep: ");
	fprintf(stderr, s, a);
	exit(2);
}

succeed(f)
{
	foundflag++;
	if (cflag) {
		tln++;
		return;
	}
	if ((fflag && nfile == 1) || nfile > 1)
		printf("%s:", f);
	if (bflag)
		printf("%ld:", blkno);
	if (nflag)
		printf("%ld:", lnum);
	printf("%s\n", linebuf);
}
