/*
 * QED
 */
#define	VAX	VAX
#ifdef	VAX
#define	BIGTMP	LOTSOFBITS
#endif
/*	Fake setexit & reset from v6 using setjmp and longjmp	*/
#include <setjmp.h>
jmp_buf	env;
#define	setexit()	setjmp(env)
#define	reset()		longjmp(env, 0)
#define TRUE	1
#define FALSE	0
#define	LBSIZE	512
#define RHSIZE	(LBSIZE/4)	/* ed says /2; but that's unreasonable! */
#define	NBRA	9
#define	EOF	(-1)
#define	FILERR	0200
#define	SAVENEVER	0
#define	SAVEIFFIRST	1
#define	SAVEALWAYS	2
/*
 * Stack types.  Must appear in the order as in cspec[]/getchar.
 * XTTY, GLOB and BRWS are special types with no corresponding special character.
 * Special is never used directly - it is just used in tracing
 *	pushinp()/getchar.c can be called with a negative index for cspec[]
 */
char	special[]; /* "xgBbBcfFlprzN\"\\'" */
#define	cspec	(special + 3)
#define	XTTY		0175
#define	GLOB		0176
#define	BRWS		0177
#define	BUF		0
#define	CURBN		1
#define	QUOTE		2
#define	FILEN		3
#define	BFILEN		4
#define	TTY		5
#define	PAT		6
#define	RHS		7
#define	STRING		8
#define	NEWL		9
#define	NOTHING		10
#define	BACKSLASH	11
#define	LITERAL		12
/*
 * Getchar-linked macros
 */
#define ungetchar(c)	(peekc = (c))
#define nextchar()	(peekc = getchar())
#define NBUFS 56
/*
 * The buffer structure.  All info associated with each buffer stored here
 */
struct buffer{
	int *zero;
	int *dot;
	int *dol;
	char cflag;
	char gmark;
}buffer[NBUFS];
struct buffer *curbuf;
/*
 * The string structure
 * The first NBUFS strings are the registers
 * The next NBUFS are the file names
 * The next two are the saved pattern and saved right hand side
 * The next is the special register for browsing (the `ob' command)
 * The next is a file buffer.
 * Strings are stored in the `strarea' area and garbage collected
 * when the area is full.  The NSTRCHARS parameter can be increased
 * if you've got lots of core.
 * The first two characters of `strarea' form the null string.
 * The third and subsequent characters are the storage for non null
 * strings.
 * The null string has to be stored in this area so that
 * the R and S commands write out valid pointers to the null string.
 * The last entry in the string structure is used to store the pointer
 * to the next free position.
 * In string.c strfree is defined as "string[NSTRING].str".
 */
#define	NSTRING	(NBUFS+NBUFS+4)
#define	COUNT	(26+'C'-'A')
#define	TRUTH	(26+'T'-'A')
#define	UNIX	(26+'U'-'A')
#define	FILE(z)	(NBUFS+(z))
#define	SAVPAT	(NBUFS+NBUFS)
#define	SAVRHS	(SAVPAT+1)
#define	BROWSE	(SAVRHS+1)
#define	FILEBUF	(BROWSE+1)
struct string{
	int len;
	char *str;
}string[NSTRING+1];
#define NSTRCHARS 1024
char strarea[NSTRCHARS + 2];
#define	nullstr strarea
#define	strchars (&strarea[2])
#define STACKSIZE 16
/*
 * The getchar stack.
 */
struct stack{
	char type;
	char literal;
	union{
		struct buffer *u1bufptr;
		char *u1globp;
	}u1;
	union{
		int u2lineno;
		int u2strname;
	}u2;
	int charno;
}stack[STACKSIZE];
#define	bufptr	u1.u1bufptr
#define	globp	u1.u1globp
#define	lineno	u2.u2lineno
#define	strname	u2.u2strname
struct stack *stackp;
int	peekc;
int	lastc;
char	line[70];
char	*linp;
int	savedfile;
char	linebuf[LBSIZE];
int	*zero;
int	*dot;
int	*dol;
int 	*lastdol;
int	*endcore;
int	*fendcore;
int	*addr1;
int	*addr2;
char	genbuf[LBSIZE];
char	*linebp;
#include	"sgtty.h"
struct	sgttyb ttybuf;
int	ninbuf;
int	io;
int	onhup;
int	onquit;
int	onintr;
char	lasterr;
#define	PAGESIZE	22
extern	pagesize;
extern char bformat;	/* = 'p' */
int	appflag;
int	cflag;
int	cprflag;
int	dflag;
int	eflag;
int	gflag;
int	biggflag;
int	iflag;
int	prflag;
int	tflag;
int	uflag;
int	vflag;
int	qok;
int	eok;
int	initflag;
int	nestlevel;
int	lastttyc;
int	listf;
int	tfile;
int	tfile2;
char	*tfname;
char	*loc1;
char	*loc2;
int	names[NBUFS];
char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;
int	oneline;
int	lock;
char	bname[]; /* ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~" */
char	lchars[];	/* = "pPlL" */
int	bbempty;	/* whether getc's internal buffer buffer needs reloading */
char	*getline();
int	*address();
char	*getenv();
long	lseek();
char	*sbrk();
