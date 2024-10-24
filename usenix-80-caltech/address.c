/*% cc -c -O %
 */
#include "vars.h"
int *address(deflt)
	int *deflt;
{
	register int c, plus, *a;
	int n, opcnt, nextopand, *b;

	if(errok)
		truth=TRUE;
	nextopand = -1;
	plus = TRUE;
	a = deflt? deflt : dot;
	for (opcnt=0; zero<=a && a<=dol; opcnt++) {
		do c=getchar(); while (c==' ' || c=='\t');
		if ('0'<=c && c<='9') {
			if (!opcnt && !deflt)
				a = zero;
			for (n=0; '0'<=c && c<='9'; c=getchar())
				n = n*10+(c-'0');
			ungetchar(c);
			if (plus)
				a =+ n;
			else
				a =- n;
		} else switch (c) {
		case '$':
			a = dol;
			/* fall through */
		case '.':
			if (opcnt || deflt)
				return(adderr('a'));
			break;
		case '\'':
			c = getchar();
			if (opcnt || deflt || posn(c, bname) < 0)
				return(adderr('a'));
			for (a=zero+1; a<=dol && names[posn(c, bname)]!=(*a|01); a++)
				;
			break;
		case '?':
			plus = !plus;
			/* fall through */
		case '/':
			compile(c);
			b = a;
			for (;;) {
				if (plus) {
					a++;
					if (a>dol)
						a = zero;
				} else {
					a--;
					if (a<zero)
						a=dol;
				}
				if (execute(a))
					break;
				if (a==b)
					return(adderr('/'));
			}
			break;
		default:
			if (nextopand == opcnt) {
				if (plus)
					a++;
				else
					a--;
				if (a<zero || dol<a)
					return(adderr('$'));
			}
			if (!(plus = c=='+') && c!='-' && c!='^') {
				ungetchar(c);
				return (opcnt==0? deflt : a);
			}
			nextopand = opcnt+1;
			continue;
		}
		plus = TRUE;
	}
	return(adderr('$'));
}
adderr(c)
{
	if(!errok)
		error(c);
	truth=FALSE;
	return(0);
}
