/*
 *	The actual definitions of variables
 */
#include "vars.h"
#ifdef	VAX
#endif
/*	Fake setexit & reset from v6 using setjmp and longjmp	*/
#include <setjmp.h>
jmp_buf	env;
/*
 * Stack types.  Must appear in the order as in cspec[]/getchar.
 * XTTY, GLOB and BRWS are special types with no corresponding special character.
 * Special is never used directly - it is just used in tracing
 *	pushinp()/getchar.c can be called with a negative index for cspec[]
 */
extern char	special[]; /* "xgBbBcfFlprzN\"\\'" */
/*
 * Getchar-linked macros
 */
/*
 * The buffer structure.  All info associated with each buffer stored here
 */
struct buffer buffer[NBUFS];
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
struct string string[NSTRING+1];
char strarea[NSTRCHARS + 2];
/*
 * The getchar stack.
 */
struct stack stack[STACKSIZE];
struct stack *stackp;
int	peekc;
int	lastc;
char	line[70];
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
/*	DPM 90/12/22 - get rid of dialect-dependant tty stuff
 *	#include	<sgtty.h>
 *	struct	sgttyb ttybuf;
 */
int	ninbuf;
int	io;
int	onhup;
int	onquit;
int	onintr;
char	lasterr;
extern	pagesize;
extern char bformat;	/* = 'p' */
int	cflag;
int	cprflag;
int	eflag;
int	biggflag;
int	iflag;
int	qok;
int	eok;
int	nestlevel;
int	listf;
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
extern char	bname[]; /* ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~" */
extern char	lchars[];	/* = "pPlL" */
int	bbempty;	/* whether getc's internal buffer buffer needs reloading */
char	*getline();
int	*address();
char	*getenv();
long	lseek();
char	*sbrk();
