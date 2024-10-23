/*% cc -c -O %
 */
#include "vars.h"
#define ESIZE	256
char	expbuf[ESIZE+4];
#define	CCHR	2
int	expgood	/*0*/;		/* flag indicating if compiled exp is good */
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define CFUNNY	10

#define	STAR	01
#define STARABLE CFUNNY

#define	CDOL	12
#define	CEOF	13
#define	CBRA	14
#define	CKET	15
#define CBOI	16
#define CEOI	17
#define CSPACE	18
int	circfl;
char	pmagic[] = "/.$^*+\\(){}[!_";
compile(eof)
char eof;
{
	register c;
	register char *ep, *penultep;
	char *lastep, *bracketp, bracket[NBRA];
	int getsvc();
	int getchar();

	if (eof == '\n')
		error('x');
	pmagic[0] = eof;
	if ((c=nextchar()) == eof) {
		if (!expgood)
			goto cerror;
		getchar();	/* eat the eof character */
		return;
	}
	expgood = FALSE;
	ep = expbuf;
	lastep = 0;
	bracketp = bracket;
	nbra = 0;
	startstring();	/* for the saved pattern register */
	circfl = 0;
	if (c=='^') {
		getsvc();	/* save the caret */
		circfl++;
	}
	for (;;) {
		c = getquote(pmagic, getsvc);
		if (c==eof) {
			if (bracketp!=bracket)
				goto cerror;
			*ep++ = CEOF;
			expgood = TRUE;
			dropstring();	/* lose the eof character */
			setstring(SAVPAT);
			return;
		}
		if (ep >= &expbuf[ESIZE-5])
			goto cerror;
		penultep = lastep;
		lastep = ep;

		switch (c) {
		case '('|0200:
			if (nbra >= NBRA)
				goto cerror;
			*bracketp++ = nbra;
			*ep++ = CBRA;
			*ep++ = nbra++;
			continue;
		case ')'|0200:
			if (bracketp <= bracket)
				goto cerror;
			*ep++ = CKET;
			*ep++ = *--bracketp;
			continue;
		case '{'|0200:
			*ep++ = CBOI;
			continue;
		case '}'|0200:
			*ep++ = CEOI;
			continue;
		case '_'|0200:
			*ep++ = CSPACE;
			continue;
		case '!'|0200:
			*ep++ = CFUNNY;
			continue;
		case '*':
		case '+':
			if (penultep==0){
				*ep++ = CCHR;
				*ep++ = c;
			} else {
				if(*penultep>STARABLE)
					goto cerror;
				if(c == '+'){
					if((ep-penultep)+ep >= &expbuf[ESIZE-1])
						goto cerror;
					do
						*ep++ = *penultep++;
					while (penultep!=lastep);
				}
				*penultep =| STAR;
				lastep = 0;
			}
			continue;
		case '\n':
			goto cerror;
		case '.':
			*ep++ = CDOT;
			continue;

		case '[':
			penultep = ep;
			*ep++ = CCL;
			*ep++ = 0;
			if ((c=getsvc()) == '^') {
				c = getsvc();
				ep[-2] = NCCL;
			}
			do {
				if (c == EOF || c == '\n')
					goto cerror;
				*ep++ = c;
				if ((lastc=getsvc()) == '-') {
					c=getsvc();
					if (c == EOF || c == '\n' || c<=ep[-1])
						goto cerror;
					ep[-1] =| 0200;
					*ep++ = c;
					lastc = getsvc();	/* prime lastc */
				} else if (dflag&&'a'<=(c|' ')&&(c|' ')<='z')
					*ep++ = c^' ';
				if (ep >= &expbuf[ESIZE-1])
					goto cerror;
			} while ((c=lastc) != ']');
			penultep[1] = ep-penultep-1;
			continue;


		case '$':
			if (nextchar() == eof) {
				*ep++ = CDOL;
				continue;
			}
			/* fall through */
		default:
			c =& ~0200;
			if(dflag && c|' '>='a' && c|' '<='z'){
				*ep++ = CCL;
				*ep++ = 3;
				*ep++ = c;
				*ep++ = c^' ';
			}
			else{
				*ep++ = CCHR;
				*ep++ = c;
			}
		}
	}
   cerror:
	error('p');
}
marked(addr)
	int *addr;
{
	register a;
	register n, *p;

	a = *addr | 01;
	p = names;
	n = NBUFS;
	do
		if(*p++ == a)
			return(TRUE);
	while(--n);
	return(0);
}
getsvc(){
	register c;
	addstring(c=getchar());
	return(c);
}
int execute(addr)
	int *addr;
{
	register char *p1, *p2;

	if (addr==0) {
		if((p1=loc2) == 0)	/* G command */
			p1 = linebuf;
		else if (circfl)	/* not first search in substitute */
			return(FALSE);
	} else {
		if (addr==zero)
			return(FALSE);
		p1 = getline(*addr, linebuf);
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return(advance(p1, p2));
	}
	do {
		if (*p2 != CCHR  ||  p2[1] == *p1) {
			if (advance(p1, p2)) {
				loc1 = p1;
				return(TRUE);
			}
		}
	} while (*p1++);
	return(FALSE);
}

int advance(alp, aep)
char *alp, *aep;
{
	register char *lp, *ep, *curlp;

	lp = alp;
	ep = aep;
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
			loc2 = lp;
			return(TRUE);

		case CBRA:
			braslist[*ep++] = lp;
			continue;

		case CKET:
			braelist[*ep++] = lp;
			continue;

		case CBOI:
			if (alfmatch(*lp,0)
			    && (lp==linebuf || !alfmatch(lp[-1],1)))
				continue;
			return(FALSE);

		case CEOI:
			if (!alfmatch(*lp,1)
			    && lp!=linebuf && alfmatch(lp[-1],1))
				continue;
			return(FALSE);

		case CSPACE:
			if (*lp==' ' || *lp=='\t') {
				while (*lp == ' ' || *lp=='\t')
					lp++;
				continue;
				}
			return(FALSE);

		default:
			error('!');
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


cclass(set, c, f)
	register char *set, c;
{
	register n;
	if (c == 0)
		return(0);
	n = *set++;
	while (--n) {
		if (*set<0) {
			if ((*set++ & 0177) <= c) {
				if (c <= *set++)
					return(f);
			} else
				set++;
			--n;
		} else if (*set++ == c)
			return(f);
	}
	return(!f);
}
