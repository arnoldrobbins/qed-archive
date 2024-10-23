/*% cc -s -O % -o qedbufs -lS
 *
 * print strings and buffers in a qed save file
 *
 *	created dmtilbrook jun/79
 */
#include <stdio.h>
#define USAGE	"qedbufs [ -n% ] [ file ... ]"
#define NBUFS 56
struct buffer {
	int *zero;
	int *dot;
	int *dol;
	char cflag;
	char gmark;
}buffer[NBUFS];
#define	NSTRING (NBUFS+NBUFS+4)
#define FILE(z) (NBUFS+(z))
#define SAVPAT	(NBUFS+NBUFS)
#define	SAVRHS	(SAVPAT+1)
#define	BROWSE	(SAVRHS+1)
#define FILEBUF	(BROWSE+1)
struct string {
	int len;
	char *str;
}string[NSTRING+1];
#define NSTRCHARS 1024
char strarea[NSTRCHARS + 2];
int	mflag;
int	zflag;
int	bflag;
char	**files;
char	numfiles;
char	*def[] &"q";
int	fi;
char	holdit[512];
char	bname[] "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~";
char	line[70];
char	listf;
int	col;
char	*linp line;
main(argc, argv) int argc; char *argv[];
{
	arginterp(argc, argv);
	do {
		sprintf(holdit, "%s:bq", *files++);
		if ((fi = open(holdit, 0)) < 0) {
			perror(holdit);
			continue;
		}
		doit(holdit);
		close(fi);
	} while (--numfiles);
	exit(0);
}
arginterp(argc, argv) int argc; char **argv;
{
	register char *p;
	register int i;
	for (--argc, argv++; **argv == '-'; argc--, argv++) {
		switch (argv[0][1]) {
		case 'b':
		case 'z':
		case '%':
		case 'n':
			for (p = &argv[0][1]; *p; ) {
				switch (*p++) {
				case '%':
				case 'z':
					zflag++;
					continue;
				case 'n':
				case 'b':
					bflag++;
					continue;
				default:
					usage("invalid flag", *argv);
				}
			}
			continue;
		case 0:
			zflag++;
			bflag++;
			continue;
		default:
			usage("invalid option", *argv);
		}
	}
	if (!zflag && !bflag) {
		bflag++;
	}
	if (numfiles = argc) {
		if (numfiles > 1) mflag++;
		files = argv;
	}else{
		numfiles = 1;
		files = &def;
	}
}
check(cbit, mess) int cbit; char *mess;
{
	if (cbit >= 0) return(cbit);
	perror(mess);
	exit(1);
}
usage(m1, m2) char *m1, *m2;
{
	write(2, m1, strlength(m1));
	write(2, ": ", 2);
	write(2, m2, strlength(m2));
	write(2, "\n", 1);
	write(2, "Usage: ", 6);
	write(2, USAGE, strlength(USAGE));
	write(2, "\n", 1);
	exit(1);
}
doit(ff) char *ff;
{
	register int i;
	register int j;
	register struct buffer *b;
	int gotstrings;
	if (mflag) printf("%s:\n", ff);
	if (read(fi, buffer, sizeof buffer) != sizeof buffer) {
		printf("Insufficient buffer info\n");
		return;
	}
	if (read(fi, strarea, sizeof strarea) != sizeof strarea
		|| read(fi, string, sizeof string) != sizeof string) {
		printf("Can't read string area\n");
		gotstrings = 0;
	}
	else {
		shiftstring();
		gotstrings = 1;
	}
	if (bflag) {
		for (i = 0; i < NBUFS; i++) {
			putbuf(i);
		}
	}
	if (zflag) putstrs(NBUFS);
}
putbuf(i)
{
	register struct buffer *b;
	b = &buffer[i];
	if (b->dol == b->zero && *(string[FILE(i)].str) == 0) {
		return;
	}
	printf("%c%c %d", bname[i], b->cflag? '\'': ' ',b->dol - b->zero);
	if (string[FILE(i)].str[0]) {
		listf = 1;
		printf("\t%s\n", string[FILE(i)].str);
		listf = 0;
	}
	else
		putch('\n');
}
putstrs()
{
	int i;
	register char *p;
	for (i=0; i<NBUFS; i++) {
		p=string[i].str;
		if (*p!=0) {
			puts(bname[i], p);
		}
	}
	puts('P', string[SAVPAT].str);
	puts('R', string[SAVRHS].str);
	puts('B', string[BROWSE].str);
}
shiftstring() {	/* A highly machine-dependent routine */
	register struct string *sp;
	for (sp=string; sp<=string+NSTRING; sp++)
		sp->str =+ strarea;
}
puts(c, p) register char c, *p;
{
	putch(c);
	putch('\t');
	listf++;
	while (c = *p++)
		putch(c);
	putch('\n');
	listf = 0;
}
putch(c)
register char c;
{
	register char *lp;

	lp = linp;
	if (listf) {
		if (c=='\n') {
			if(linp!=line && *(linp-1)==' ') {
				*lp++ = '\\';
				*lp++ = 'n';
			}
		} else {
			if (col >= (72-4-2)) {
				col = 0;
				*lp++ = '\\';
				*lp++ = '\n';
			}
			col++;
			if (c=='\b' || c=='\t' || c=='\\') {
				*lp++ = '\\';
				c = c=='\b'? 'b' : (c=='\t'? 't' : '\\');
				col++;
			} else if (c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ =  (c>>6)    +'0';
				*lp++ = ((c>>3)&07)+'0';
				c     = ( c    &07)+'0';
				col =+ 3;
			}
		}
	}
	*lp++ = c;
	if (c == '\n' || lp >= &line[64]) {
		if (c == '\n')
			col = 0;
		if (lp != line)
			write(1, line, lp-line);
		lp = line;
	}
	linp = lp;
}
strlength(pp) register char *pp; {
	register char *p;
	for (p = pp; *p++; );
	return (p - pp - 1);
}
