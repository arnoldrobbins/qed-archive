#
/*%cc -s -n -O %
 */

#define	PDP11	PDP11
/*
 * Editor
 * Copyright 1974, Bell Telephone Laboratories, Incorporated
 *
 * #"# Modified by Tom Duff:
 *	- Added optional number to substitute command to specify which
 *	  occurrence of the pattern is to be replaced.
 *	- Added address separators * and &.
 *	- Added option command and d, p, v, and & options.
 *	- In error messages, added the letter indicating the type of error.
 *
 * #"# Modified by Rob Pike:
 *	- Added regular expressions \{ and \}.
 *	- Made l command print DEL correctly.
 *	- Fixed a bug which transferred marks from the first line entered
 *	  to line zero.
 *	- Fixed a bug in ';' which caused crashes.
 *
 * #"# Modified 1977 March by Rob Pike:
 *	On HANGUP signal, write buffer (if not empty) into a file named
 *	<remembered filename><rescsuf>.
 *
 * #"# Modified 1977 December 21 by Henry Spencer:
 *	Made ed use "/etc/prompt" to set default prompting. (not used at HEP)
 *
 * #"# Modified 1977 December 24 by Rob Pike:
 *	Added "e" option to make "l" command print "|$" at the ends of lines.
 *
 * #"# Modified 1979 April 16 by Henry Spencer:
 *	Catch HUP signal only if they were not being ignored.
 *
 * #"# Modified 1979 April 17 by D. Hugh Redelmeier:
 *	- Made s command preserve marks of k, g, and v commands.
 *	- Made e delete marks.
 *	- Made filename() reject all nongraphic characters.  A  funny file
 *	  name can still be used as an arg to ed, so the f command lists
 *	  file name in l mode.  The e and f commands now validate the syntax
 *	  of their operand before replacing the name of the current file.
 *	  The e command does not delete the current buffer until the filename
 *	  is parsed.
 *	- Made several changes to make sure the state is legitimate after an
 *	  INTR or HUP signal.  Made sure iblock is correct or conservative.
 *	  Added expgood to indicate if compiled regular expression is valid.
 *	  Added a lock facility, and used it in clearbuf(), delete(), append(),
 *	  move(), putline(), and filename().
 *	- In HUP signal handling, made sure name of created file was not
 *	  identical to <savedfile>.
 *	- Made '!' command set INTR signal catching to same as on entry to ed.
 *	- Made '!' command print "!" at end only if in verbose mode.
 *	- Made '!' command accept an address range, meaning "pipe the addressed
 *	  lines as standard input to the shell command".
 *	- In '!' command, made "$2" in shell command signify <savedfile>.
 *	- Added u (undo) command to undo the last line changed by s, x, or u.
 *	  (Does not delete new lines created by substituting newlines.)
 *	- Added (.,.+1)j (join) command which collapses all addressed lines
 *	  into one.
 *	- Added W command, a variant of the w command which will write null
 *	  files without complaining.
 *	- Made any I/O error during e, r, or w command generate error 'o'.
 *	- Made reading a file whose last line did not end in newline generate
 *	  error 'n'.  Made a, c, and i commands generate same error in
 *	  analogous case.
 *	- Added the x (xform) command to graphically edit lines.  Processing
 *	  of this command depends on terminal type, so the t option was added.
 *	  The x command generates error "b" if it does not know what the
 *	  terminal type is or if the editing request is malformed.
 *	- Added cd <name> command to change the current drectory.  Causes
 *	  the current file name to be discarded unless it begins with "/".
 *	- Added noisy option to make s commands (not inside global) and x
 *	  commands default to p suffix.  Put option processing in a procedure.
 *	- Re-implemented address decoding logic.
 *	- Implemented "address plus (or minus) /regular expression/" as an
 *	  address.  Meaning: first match found searching forward (or backward)
 *	  from initial address.  If "?" is used instead of "/", search
 *	  direction is reversed.
 *	- Fixed several bugs associated with ';', '*', and '&' in addresses.
 *	- Made it illegal to have an empty address either side of ',' or ';'.
 *	- Stopped '*' and '&' setting '.'.
 *	- Made '&' an address separator only if it is the first separator.
 *	- Made newline command print range unless last address separator was
 *	  ';' (previously it printed range iff last character before newline
 *	  was '&').
 *	- Cleaned up pattern processing.
 *	- Added \_ to match maximal length, non-empty sequence of spaces
 *	  and tabs.
 *	- Added \! and \!* to match any control character except tab and
 *	  newline (includes rubout).
 *	- Added \+ operator to match one or more occurrences of preceding
 *	  regular expression.
 *	- Added ranges to character class regular expressions.
 *	- Made dualcase mode apply only to unescaped letters, and only
 *	  outside character classes.
 *	- Fixed a bug with \{: it used to check for start-of-line by comparing
 *	  the pointer with the start of the wrong buffer.  If the compare had
 *	  succeeded, it would have assumed that the next character was
 *	  alphabetic.
 *	- Made \} require the location of the non-alphanumeric it matches
 *	  be preceded by an alphanumeric.
 *	- Made it illegal to star something more than once.
 *	- Made illegal starring invalidate current pattern (it should
 *	  have already, but didn't).
 *	- Re-implemented the substitute command.
 *	- Previously, a substitute with a "g" suffix looped indefinitely
 *	  if its pattern matched a null string (unless the pattern started
 *	  with a "^" or a starred item, or ended with a "$").  Now, a null
 *	  string can only be matched once per location (that is, once between
 *	  each character).
 *	- Previously, subsequent replacements on a line matched the updated
 *	  version; now they match the original version.  This makes a
 *	  difference with \{ and \} since they examine the character preceding
 *	  the location they match.
 *	- In the replacement part of substitute, made escaped n equivalent to
 *	  escaped newline.
 *	- Made maximum number of \( ... \) sequences 9 so that \1 to \9
 *	  can be used.
 *	- Fixed bug in \( processing that allowed invalid references.
 *	- Re-implemented main(): made parameter processing more regular
 *	  and added diagnostics.
 *	- Made getchar() discard NULs; made '\0' the internal representation
 *	  of newline.
 *	- Fixed bug in getchar() that sometimes caused the next command after
 *	  an error to be flushed.
 *	- Re-wrote putline() and getline().
 *	- Made tempfile I/O errors generate error 'T'.
 *	- Made process number in tempfile name decimal.
 *	- In global command, made empty command list equivalent to "p".
 *	  Also, in the command list, made a sequence of backslashes preceding
 *	  a newline escape that newline while preserving all but the last
 *	  backslash.  Previously, only even-length sequences of backslashes
 *	  could result.
 *	- Fixed bug causing global applied to a, c, or i to print prompts.
 *	- Made a, c, and i commands take "immediate" data.
 *	- Re-implemented delete() and move() to recognize degenerate cases.
 *	  Made delete() of all lines reinitialize the buffer.
 *
 * #"# Modified 1979 December 21 by Rob Pike:
 *	- Made `+' what the old `\+' was in R.E.'s, for compatibility with
 *	  other ed's, and to make it used more.  Others may undo it, one day.
 *	- Made error FILERR, generated by i/o to an inaccessible file,
 *	  print `?o file' instead of `?o' as its diagnostic.
 *	- Made all 'o' errors into 'r' or 'w' according to whether the error
 *	  was for read or write (for i/o errors only, not file open/create).
 *	- Added Version 7 support code (e.g. for lseek) which can be
 *	  activated by #define V7 V7
 *	- Clarified the use of ldiv via #define PDP11
 *	- Macro'd the strange uses of *addr++;
 *	- Changed the rescue file name to ed.hup
 *	- Made P a synonym for p
 *	- Changed file creation mode to 0666
 *	- Removed the 'e' flag and restructured the format of lines
 *	  printed by 'l'.
 *	- Deleted the '&' address; added the 'b' command.
 *	- Made ed send back a sensible return code.
 */

/*
 * The "buffer" data structure:
 *
 * The buffer is the current text being edited.  The text is made up of a
 * sequence of lines stored in the "tempfile".  An array of "line descriptors",
 * in memory, contains a pointer to each line in the tempfile.
 *
 * The tempfile is accessed in blocks of BLKSIZE bytes.  Independent of blocks,
 * lines are stored in as many bytes as needed, aligned on four-byte
 * boundaries.  Newlines are replaced by NULs.  The tempfile has a maximum size
 * of 128k bytes.  Once written, a block is never changed; the last (partial)
 * block is kept in "obuf".
 *
 * The array of line descriptors is subscripted by the line number; the first
 * line is a dummy, line zero.  A line address is a pointer into this array.
 * The array is grown when needed, using sbrk(II).  When a line is added or
 * deleted, entries for following lines must be moved.  The following
 * diagram shows the array, and where various pointers point into it.
 * The area between dol and endcore is allocated, but not yet used.
 *
 *		*---------------*+++++++*
 *	    zero|one	     dol|	|endcore
 *		*---------------*+++++++*
 *
 * A line descriptor, an "int", contains two fields:  a file offset and a
 * flag bit.  The file offset (the high-order fifteen bits), when multiplied
 * by four, is the offset in bytes of the start of the line in tempfile.
 * The precision and range of the file offset causes length and alignment
 * limitations on tempfile.  The flag bit is used to indicate lines to be
 * processed in a g or v command.
 */

#define FALSE	0
#define TRUE	1
#define	SIGHUP	1
#define	SIGINTR	2
#define	SIGQUIT	3
#define	SIGBPIPE 13
#define	FNSIZE	64
#define BLKSIZE	512
#define	LBSIZE	512
#define RHSIZE	256
#define	ESIZE	128
#define	GBSIZE	256
#define	LDCHUNK	512
#define	NBRA	9

/*  The following are, despite appearances, character constants.  */

#define Q	0177600		/* `quote' bit (e.g. \&) */
#define EMPTY	0177776
#define EOF	0177777
#define NL	'\0'
#define isEOF(c)  ((c)<0)
#define isEOForNL(c)  ((c)<=0)
#define	FILERR	Q

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
#define	CBRA	14
#define	CKET	15
#define CBOI	16
#define CEOI	17
#define CSPACE	18

#ifdef	PROMPTING
#define PROMPTFILE "/etc/prompt"
#endif

#define	inc(addr)	*addr++

int	onintr;		/* original value of signal(SIGINTR) */
int	onhup;		/* original value of signal(SIGHUP) */
int	onquit;		/* original value of signal(SIGQUIT) */
int	lock	/*0*/;	/* lock preventing interrupts */
int	(*pending)()	/*0*/;	/* what to do when unlocking */

#ifdef V7
#include <setjmp.h>
jmp_buf	env;
#define	setexit()	setjmp(env)
#define	reset()		longjmp(env)
#endif

char	linebuf[LBSIZE];/* line used by getline() and putline() */
char	genbuf[LBSIZE];	/* secondary line buffer */
char	rhsbuf[RHSIZE];	/* buffer for substitute and xform */
long	count;		/* I/O character count & result of = command */

char	peekc	EMPTY;	/* if not EMPTY, next result of getchar() */
char	lastc;		/* last result of getchar() */
char	*globp	/*0*/;	/* if nonzero, pointer to getchar() input string */
char	rdcom[] { 'r', EOF };	/* read command issued by main() */

char	line[70];	/* buffer for terminal output (puts() and putchar()) */
char	*linp	line;	/* current position */
int	pflag	/*0*/;	/* true if current command has "p" suffix */
int	listf;		/* flag indicating "l" command type printing */
int	col;		/* if listf, current output column */
char	prompt	'*';	/* command prompting character */

char	savedfile[FNSIZE] /*"\0"*/;	/* saved (default) filename */
char	RESCFILE[] "ed.hup";		/* filename used by rescue() */
char	file[FNSIZE];			/* current filename */
int	io	-1;	/* file descriptor of open file, else -1 */
char	*nextip;	/* current position of getfile() in input buffer */
int	ninbuf;		/* characters remaining in getfile()'s input buffer */

char	tfname[] "/tmp/e000000";	/* name of tempfile */
int	tfile;		/* file descriptor of tempfile */
char	ibuff[BLKSIZE];	/* tempfile input buffer */
int	iblock;		/* number of block in ibuff */
char	obuff[BLKSIZE];	/* tempfile's last (unwritten) block */
int	oblock;		/* block number of obuff */
int	ooff;		/* offset of next byte in obuff */
int	rcode; /*0*/	/* return code, the ASCII value of the last error */

int	*zero;		/* pointer to line 0 */
int	*one;		/* pointer to line 1 (initial value of sbrk(0)) */
int	*dot;		/* pointer to . */
int	*dol;		/* pointer to $ */
int	*endcore;	/* current value of sbrk(0) (>dol) */
int	*addr1;		/* pointer to first addressed line */
int	*addr2;		/* pointer to last addressed line */

int	names['z'-'a'+1];	/* mark table (contains line descriptors) */
int	old_line;		/* descriptor of last line replaced */
int	new_line;		/* descriptor of last replacement line */

char	expbuf[ESIZE];		/* buffer for compiled regular expression */
int	expgood	/*0*/;		/* flag indicating if compiled exp is good */
int	circfl;			/* flag indicating if expression is anchored */
char	*next_old;		/* in s command: next char of original line */
char	*first_new;		/* start of lines for getmem() */
char	*next_new;		/* end of lines for getmem() */
char	*loc1;			/* in linebuf: first matched character */
char	*loc2;			/* in linebuf: character after last matched */
int	nbra;			/* number of \('s in current expression */
char	*braslist[NBRA];	/* starting points of bracketed exps */
char	*braelist[NBRA];	/* ending points of bracketed exps */

int	dflag	/*0*/;	/* dualcase search flag */
int	nflag	/*0*/;	/* noisy mode flag */
int	prflag;		/* prompting flag */
int	vflag	1;	/* verbose mode flag */
int	pagesize 18;	/* pagesize for b */

struct	optab {	/* option name table */
	char	opname;		/* name of option */
	char	*opaddr;	/* associated address */
};

struct	optab	modetab[] {
	'd',	&dflag,
	'n',	&nflag,
	'p',	&prflag,
	'v',	&vflag,
	'\0',	0
};

/*
 * The structure used by stty, gtty
 */
struct {
	char ispeed, ospeed;
	char erase, kill;
	int mode;
}
	ttybuf {0, 0, '\b', '\b', 0};
#define MONOMODE 0000004

#define S2741
char *ttyty	/*0*/;	/* terminal type (needed for xform) */
char monocase[] "`|~{}";
char esc_monocase[] "'!^()";
#define dualcase (monocase+5)
#ifdef S2741
char correspondence[] "|^\\`[]{}~";
#define ebcd (correspondence+2)
#endif

struct	optab	termtab[] {
	'd',	dualcase,
	'm',	monocase,
#ifdef S2741
	'c',	correspondence,
	'e',	ebcd,
#endif
	'\0',	0
};

/* declare forwards */

extern char getchar();
extern int *address();
extern char *getline();

main(argc, argv)
int argc;
char **argv;
{
	register char *p1, *p2;
	register int n;
	static char c;
	extern interrupt(), rescue();

	onintr = signal(SIGINTR, 1);
	onquit = signal(SIGQUIT, 1);
	onhup = signal(SIGHUP, 1);
	while (--argc>0 && *(p1 = *++argv) == '-')
		if (p1[1] == '\0')
			vflag = FALSE;
		else if (p1[1]=='q' && p1[2]=='\0')
			signal(SIGQUIT, 0);
		else
			puts("?i");
#ifdef	PROMPTING
	if ((prflag=vflag) && (n=open(PROMPTFILE,0)) >= 0) {
#ifdef V7
		if (lseek(n, geteuid()*2L, 0) >= 0
#endif
#ifndef V7
		if (seek(n,(getuid()&0377)*2,0) >= 0
#endif
		&&  read(n,&c,1) == 1
		&&  c=='n')
			prflag = FALSE;
		close(n);
	}
#endif
	if (argc>0) {
		if (argc != 1)
			puts("?i");
		p2 = savedfile;
		do ; while ((*p2++ = *p1++) && p2<savedfile+FNSIZE);
		if (*--p1) {
			savedfile[0] = '\0';
			puts("?l");
		} else
			globp = rdcom;
	}
	if (gtty(0,&ttybuf)>=0)
		ttyty = ttybuf.mode&MONOMODE? monocase : dualcase;
	endcore = one = sbrk(0);
	zero = one-1;
	n = getpid();
	for (p1 = &tfname[sizeof tfname - 1]; *--p1 != 'e';) {
		*p1 = n%10 + '0';
		n =/ 10;
	}
	clearbuf();
	tfile = open(tfname, 2);
	if (onhup != 1)
		signal(SIGHUP, rescue);
	if (onintr != 1)
		signal(SIGINTR, interrupt);
	setexit();
	commands();
	unlink(tfname);
	exit(rcode);
}

commands()
{
	register int *a;
	register char c, lastsep;
	int (*g)();
	extern int *choose_get();	/* actually int (*choose_get())() */

	for (;;) {
	if (pflag) {
		addr1 = addr2 = dot;
		print();
	}
	listf = 0;
	if(prflag && !globp)
		write(1, &prompt, 1);
	c = NL;
	for (addr1 = a = 0;;) {
		lastsep = c;
		a=address(a);
		switch (c=getchar()) {
		case ';':
			if (a)
				dot = a;
		case ',':
			if (a == 0)
				error('a');
			addr1=a;
			a=0;
			continue;
		case '*':
			addr1 = a;
			addr2 = a;	/* trick setall() */
			setall();
			a = dol;
			continue;
		}
		break;	/* default of switch falls out of it & for */
	}
	addr2 = a;
	if (addr1==0)
		addr1 = a;
	if (addr1>a)	/* includes case where a==0 */
		error('a');

	switch(c) {

	case 'a':
		setdot();
		append(choose_get(), addr2);
		continue;

	case 'b':
		setdot();
		nonzero();
		bcom();
		continue;

	case 'c':
		c = getchar();
		if (c == 'd') {
			noaddr();
			filename('\1');
			if (chdir(file)<0)
				error('d');
			if (savedfile[0] != '/')
				savedfile[0]='\0';
		} else {
			peekc=c;
			setdot();
			nonzero();
			g=choose_get();
			delete();
			append(g, addr1-1);
		}
		continue;

	case 'd':
		setdot();
		nonzero();
		newline();
		delete();
		continue;

	case 'e':
		noaddr();
		if ((peekc=getchar()) != ' ')
			error('f');
		filename('\0');
		clearbuf();
		doread();
		continue;

	case 'f':
		noaddr();
		filename('\0');
		listf++;
		puts(savedfile);
		continue;

	case 'g':
		global(TRUE);
		continue;

	case 'i':
		setdot();
		nonzero();
		append(choose_get(), addr2-1);
		continue;

	case 'j':
		if (a==0) {
			addr1 = a = dot;
			inc(a);
			addr2 = a;
			}
		nonzero();
		join();
		continue;

	case 'k':
		c = getchar();
		if (c<'a' || 'z'<c)
			error('k');
		newline();
		setdot();
		nonzero();
		names[c-'a'] = *addr2 | 01;
		continue;

	case 'm':
		move(FALSE);
		continue;

	case 'o':
		noaddr();
		option();
		continue;

	case NL:
		if (a==0) {
			a = dot;
			inc(a);
			addr1 = a;
			addr2 = a;
		}
		if (lastsep == ';')
			addr1 = a;
		/* fall through */
	case 'l':
	case 'p':
	case 'P':
		peekc = c;
		newline();
		print();
		continue;

	case 'q':
		newline();
		/* fall through */
	case EOF:
		noaddr();
		return;

	case 'r':
		filename(savedfile[0]);
		doread();
		continue;

	case 's':
		if (globp==0)
			pflag = nflag;
		setdot();
		nonzero();
		substitute(globp);
		continue;

	case 't':
		move(TRUE);
		continue;

	case 'u':
		noaddr();
		newline();
		undo();
		continue;

	case 'v':
		global(FALSE);
		continue;

	case 'w':
		setall();
		nonzero();
		/* fall through */
	case 'W':
		filename(savedfile[0]);
		putfile();
		continue;

	case 'x':
		pflag = nflag;
		setdot();
		nonzero();
		newline();
		xform();
		continue;

	case '=':
		setall();
		newline();
		count = addr2-zero;
		putd();
		putchar(NL);
		continue;

	case '!':
		Unix();
		continue;

	default:
		error('x');
	}
	}
}

/*
 *	address decoder
 *
 *	deflt is the default result.  It is returned if no address is found.
 *	If deflt!=0, the address is decoded as if it were preceded by <deflt>.
 *	Thus, if deflt were ., the address "5" would be decoded as ".5".
 */

int *address(deflt)
int *deflt;
{
	register int sign, *a;
	int opcnt, nextopand, *b;
	register char c;

	nextopand = -1;
	sign = 1;
	opcnt = 0;
	a = dot;
	if (deflt) {
		opcnt++;
		a = deflt;
	}
	do {
		do c = getchar(); while (c==' ' || c=='\t');
		if ('0'<=c && c<='9') {
			peekc = c;
			if (!opcnt)
				a = zero;
			a =+ sign*getnum();
		} else switch (c) {
		case '$':
			a = dol;
			/* fall through */
		case '.':
			if (opcnt)
				error('a');
			break;
		case '\'':
			c = getchar();
			if (opcnt || c<'a' || 'z'<c)
				error('a');
			a = zero;
			do *a++; while (a<=dol && names[c-'a']!=(*a|01));
			break;
		case '?':
			sign = -sign;
			/* fall through */
		case '/':
			compile(c);
			b = a;
			for (;;) {
				a =+ sign;
				if (a<zero)
					a = dol;
				if (a>dol)
					a = zero;
				if (execute(a))
					break;
				if (a==b)
					error('/');
			}
			break;
		default:
			if (nextopand == opcnt) {
				a =+ sign;
				if (a<zero || dol<a)
					continue;	/* error('$'); */
			}
			if (c!='+' && c!='-' && c!='^') {
				peekc = c;
				if (opcnt==0)
					a = 0;
				return (a);
			}
			sign = 1;
			if (c!='+')
				sign = -sign;
			nextopand = ++opcnt;
			continue;
		}
		sign = 1;
		opcnt++;
	} while (zero<=a && a<=dol);
	error('$');
}

setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
}

setall()
{
	if (addr2==0) {
		addr1 = one;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
}

noaddr()
{
	if (addr2)
		error('a');
}

nonzero()
{
	if (addr1<=zero || addr2>dol)
		error('$');
}

newline()
{
	register char c;

	if ((c=getchar())=='p' || c=='P' || c=='l') {
		pflag++;
		if (c=='l')
			listf++;
		c = getchar();
	}
	if (c!=NL)
		error('x');
}

int getnum()
{
	register int r, c;

	r = 0;
	while ((c=getchar())>='0' && c<='9')
		r = r*10 + c - '0';
	peekc = c;
	return (r);
}

filename(temp)
char temp;
{
	register char c, *p1, *p2;

	count = 0;
	p1 = file;
	p2 = savedfile;
	c = getchar();
	/*
	 * The following checks for EOF because main() issues an r command
	 * to read the file named in the ed command's arg by assigning
	 * rdcom to globp.
	 */
	if (isEOForNL(c)) {
		if (*p2=='\0')
			error('f');
		do ; while (*p1++ = *p2++);
	} else {
		if (c!=' ')
			error('f');
		do c = getchar(); while (c == ' ');
		do {
			if (c<=' ' || c=='\177' || p1==file+FNSIZE-1)
				error('f');
			*p1++ = c;
		} while ((c=getchar()) != NL);
		*p1++ = '\0';
		if (temp=='\0') {
			p1 = file;
			lock++;
			do ; while (*p2++ = *p1++);
			unlock();
		}
	}
}

int *choose_get()	/* actually int (*choose_get())() */
{
	register char c;
	extern int gettty(), getmem();

	c=getchar();
	if (c=='p' || c=='l') {
		pflag++;
		if (c=='l')
			listf++;
		c=getchar();
	}
	if (c==NL)
		return (gettty);
	if (c!=' ') {
		if (c!='\t')
			error('x');
		peekc = c;
	}
	first_new = linebuf;
	gettty();	/* discard result */
	return (getmem);
}

/*
 *	compile right-hand-side: load /replacement/ into rhsbuf
 *		(used in join and substitute commands).
 */

comprhs(seof)
register char seof;
{
	register char c;
	register char *p;

	p = rhsbuf;
	do {
		if (isEOForNL(c = getchar()))
			error('x');
		if (c=='\\') {
			c = getchar();
			if (c == 'n')
				c = NL;
			c =| Q;
		}
		*p++ = c;
		if (p >= &rhsbuf[RHSIZE])
			error('l');
	} while (c != seof);
	*--p = '\0';
}

/*
 *	On HANGUP, save buffer contents in <savedfile><rescsuf>.
 */

rescue(){
	register char *p1, *p2, *fs;

	signal(SIGHUP,1);
	if (lock) {
		pending = rescue;
		return;
	}
	exfile();
	if (zero != dol) {
		addr2 = 0;
		p1=file;
		p2=RESCFILE;
		do ; while(*p1++ = *p2++);
		putfile();
	}
	exit();
}

/*
 *	On INTR, generate error '?'
 */

interrupt()
{
	signal(SIGINTR, interrupt);
	if (lock) {
		pending = interrupt;
		return;
	}
	linp=line;
	putchar(NL);
	lastc = NL;
	error('?');
}

/*
 * Unlock: exit a critical section, invoking any pending signal routines.
 */

unlock()
{
	register int (*p)();

	p = pending;
	pending = 0;
	lock = 0;
	if (p)
		(*p)();
}

error(code)
char code;
{
	unlock();
	pflag=0;
	listf=0;
	putchar('?');
	putchar(code);
	if(code&FILERR){
		putchar(' ');
		puts(file);
	} else
		putchar(NL);
	rcode=code;
	peekc = lastc;
	do ; while (!isEOForNL(getchar()));
	globp = 0;
#ifdef V7
	lseek(0, 0L, 2);
#endif
#ifndef V7
	seek(0, 0, 2);
#endif
	exfile();
	reset();
}

char getchar()
{
	register char c;

	if ((c=peekc) != EMPTY)
		peekc = EMPTY;
	else if (globp) {
		if (isEOF(c = *globp++))
			globp = 0;
	} else {
		do {
			c = EOF;
		} while (read(0,&lastc,1)>0 && (c=lastc&0177)=='\0');
		if (c == '\n')
			c = NL;
	}
	return(lastc=c);
}

int gettty()
{
	register char *p, *gp;

	p = linebuf;
	gp = globp;
	do {
		if (p >= &linebuf[LBSIZE])
			error('l');
	} while (!isEOForNL(*p++ = getchar()));
	next_new = p;
	if (isEOF(*--p)) {
		*p = '\0';
		if (p!=linebuf)
			error('n');
		globp=gp;	/* if in global, restore EOF */
		return (FALSE);
	}
	return (linebuf[0]!='.' || p!=linebuf+1);
}

int getfile()
{
	register char c, *lp, *fp;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0) {
				if (ninbuf < -1)
					error('r');
				if (lp!=linebuf)
					error('n');
				return(FALSE);
			}
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE])
			error('l');
		if (c = *fp++ & 0177) {
			*lp++ = c;
			count++;
		}
	} while (c != '\n');
	*--lp = NL;
	nextip = fp;
	return(TRUE);
}

exfile()
{
	if (io != -1) {
		close(io);
		io = -1;
		if (vflag) {
			putd();
			putchar(NL);
		}
	}
}

doread()
{
	extern int getfile();

	if ((io = open(file, 0)) < 0)
		error('o'|FILERR);
	setall();
	ninbuf = 0;
	append(getfile, addr2);
	exfile();
}

putfile()
{
	register int *a;
	register char *fp, *lp;

	if ((io = creat(file, 0666)) < 0)
		error('o'|FILERR);
	setall();
	a = addr1;
	if (a==zero)
		inc(a);
	fp = genbuf;
	while (a <= addr2) {
		lp = getline(*a++);
		do {
			if (fp >= genbuf+LBSIZE){
				if (write(io, genbuf, fp-genbuf) < 0)
					error('w');
				fp=genbuf;
			}
			count++;
		} while (*fp++ = *lp++);
		fp[-1] = '\n';
	}
	if (write(io, genbuf, fp-genbuf)<0)
		error('w');
	exfile();
}

int getmem()
{
	register char *p1, *p2;

	p1 = linebuf;
	if ((p2 = first_new) == next_new)
		return(FALSE);
	do ; while (*p1++ = *p2++);
	first_new = p2;
	return(TRUE);
}

/*
 *	Append text after line a.  Return number of lines added.
 *
 *	f	(a function) either loads a line to be appended into linebuf
 *		and returns TRUE, or returns FALSE indicating end of text.
 */

int append(f, rdot)
int (*f)();
register int *rdot;
{
	register int *a1, *a2;
	int nline, tl;

	nline = 0;
	for (;;) {
		dot = rdot;
		if (!(*f)())
	break;
		a1 = dol;
		inc(a1);
		if (a1 >= endcore) {
			if (sbrk(LDCHUNK*(sizeof *endcore)) == -1)
				error('c');
			endcore =+ LDCHUNK;
		}
		tl = putline();
		nline++;
		lock++;
		dol = a1;
		a2 = a1;
		inc(a2);
		inc(rdot);
		while (a1 > rdot)
			*--a2 = *--a1;
		*rdot = tl;
		unlock();
	}
	return(nline);
}

/*
 * Browse command
 */
bcom()
{
	register forward, n;
	static bformat;	/*0*/
	forward=1;
	if((peekc=getchar())!=NL){
		if (peekc=='-' || peekc=='+') {
			if(peekc=='-')
				forward=0;
			getchar();
		}
		if((n=getnum())>0)
			pagesize=n;
	}
	newline();
	if (pflag)
		bformat=listf;
	else
		listf=bformat;
	if (forward) {
		addr1=addr2;
		if((addr2+=pagesize)>dol)
			addr2=dol;
	} else {
		if((addr1=addr2-pagesize)<one)
			addr1=one;
	}
	print();
}

/*
 *	Replace a line, updating marks.
 */

replace(la,ld)
register int *la;
register int ld;
{
	register int *p;

	ld =| *la & 01;
	*la =| 01;
	p = names;
	do {
		if (*p++ == *la)
			*--p = ld|01;	/* infinite loop if *la==(ld|01) */
	} while (p<=names+'z'-'a');
	old_line = *la;
	*la = ld;
	new_line = ld =| 01;
}

/*
 *	delete lines addr1 through addr2.
 *
 *	addr1 must not be zero.
 *	If addr1==one and addr2==dol, the buffer is cleared:
 *	there must be no references to the buffer pending.
 */

delete()
{
	register int *a1, *a2, *a3;

	a1 = addr1;
	a2 = addr2;
	inc(a2);
	a3 = dol;
	lock++;
	dot = a1-1;
	if (a2<=a3) {
		dot = a1;
		do *a1++ = *a2++; while (a2<=a3);
	} else if (a1==one)
		clearbuf();
	*--a1;
	dol = a1;
	unlock();	/* if clearbuf called, will not be locked */
}

clearbuf()
{
	register int *p;

	lock++;
	iblock = -1;
	ooff = oblock = 0;
	close(creat(tfname, 0600));
	dot = dol = p = zero;
	p =+ LDCHUNK+1;
	if (p<endcore) {
		endcore = p;
		brk(p);
	}
	new_line = 0;
	p=names;
	do *p++ = 0; while (p<=names+('z'-'a'));
	unlock();
}

char *opsearch(tabp, c)
register struct optab *tabp;
register char c;
{
	while (tabp++->opname != c)
		if (tabp->opname == '\0')
			error('x');
	return (tabp[-1].opaddr);
}

option()
{
	register char c;
	register int r, *p;	/* some uses are char, not int */
	char k;

	/* note: parse before changing state */
	c = getchar();
	if (c=='t') {
		p = opsearch(termtab, getchar());
		r = ttybuf.erase;
		k = ttybuf.kill;
		if (!isEOForNL(c=getchar())) {
			r=c;
			if (!isEOForNL(c=getchar())) {
				k = c;
				c = getchar();
			}
		}
		peekc = c;
		newline();
		ttyty = p;
		ttybuf.erase = r;
		ttybuf.kill = k;
	} else {
		p = opsearch(modetab, c);
		r = 0;
		c = getchar();
		if (c!='r') {
			if (c!='s')
				error('x');
			r++;
		}
		newline();
		*p = r;
	}
}

Unix()
{
	register int onbpipe, *a;
	register char *p;
	static int pfd[2];
	extern interrupt();

	if (a=addr1) {
		nonzero();
		pipe(pfd);
	}
	gettty();
	if (fork()==0) {
		signal(SIGINTR, onintr);
		signal(SIGHUP, onhup);
		signal(SIGQUIT, onquit);
		if (a) {
			close(0);
			dup(pfd[0]);
			close(pfd[0]);
			close(pfd[1]);
		}
		execl("/bin/sh", "sh", "-c", linebuf, savedfile, 0);
		puts("No shell!");
		exit();
	}
	signal(SIGINTR, 1);
	if (a) {
		onbpipe = signal(SIGBPIPE, 1);
		close(pfd[0]);
		do {
			p = getline(*a++);
			do ; while (!isEOForNL(*p++));
			*--p = '\n';
		} while (write(pfd[1],linebuf,p =- linebuf-1)>=0 && a<=addr2);
		close(pfd[1]);
		signal(SIGBPIPE, onbpipe);
	}
	wait(pfd);
	if (onintr!=1)
		signal(SIGINTR, interrupt);
	if (vflag)
		puts("!");
}

/*
 *	Get a line into linebuf from tempfile address tl,
 *	and return address of linebuf.
 */

char *getline(tl)
int tl;
{
	register char *bp, *lp;
	register int nl;
	extern int read();

	lp = linebuf;
	nl = -((tl<<1) & 0774);
	tl = (tl>>8) & 0377;
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
			bp =- nl;
			nl =+ BLKSIZE;
		}
		nl--;
	} while (*lp++ = *bp++);
	return(linebuf);
}

/*
 *	Put a line from linebuf into tempfile, and return tempfile address.
 */

int putline()
{
	register char *op, *lp;
	register int r;
	extern int write();

	lp = linebuf;
	r = (oblock<<8) + ((op=ooff)>>1);	/* ooff may be 512! */
	op =+ obuff;
	lock++;
	do {
		if (op >= obuff+BLKSIZE) {
			/* delay updating oblock until after blkio succeeds */
			blkio(oblock, op=obuff, write);
			oblock++;
			ooff = 0;
		}
	} while (*op++ = *lp++);
	ooff = (((op-obuff)+3)&~3);
	unlock();
	return (r);
}

blkio(b, buf, iofcn)
char *buf;
int (*iofcn)();
{
	if (b>=255
#ifdef V7
	|| (lseek(tfile, b*512L, 0)<0)
#endif
#ifndef V7
	|| (seek(tfile,b,3)<0)
#endif
	|| ((*iofcn)(tfile,buf,512)!=512))
		error('T');
}

global(k)
{
	register char c, *gp;
	register int *a;
	char globuf[GBSIZE];

	if (globp)
		error('g');
	setall();
	nonzero();
	compile(getchar());
	gp = globuf;
	while ((c = getchar()) != NL) {
		if (isEOF(c))
			error('x');
		while (c=='\\') {
			c = getchar();
			if (c!=NL)
				*gp++ = '\\';
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE-2])
			error('l');
	}
	if (gp==globuf)
		*gp++ = 'p';
	*gp++ = NL;
	*gp++ = EOF;
	for (a=one; a<=dol; inc(a)) {
		*a =& ~01;
		if (a>=addr1 && a<=addr2 && execute(a)==k)
			*a =| 01;
	}
	for (a=one; a<=dol; inc(a)) {
		if (*a & 01) {
			*a =& ~01;
			dot = a;
			globp = globuf;
			commands();
			a = zero;
		}
	}
}

substitute(inglob)
{
	register int *a, m;
	register char *p;
	int n, gsubf;
	extern int getmem();

	n = getnum();
	m = getchar();
	compile(m);
	comprhs(m);
	gsubf = FALSE;
	if ((peekc=getchar()) == 'g') {
		gsubf++;
		peekc = EMPTY;
	}
	newline();
	for (a = addr1; a <= addr2; inc(a)) {
		if (execute(a)) {
			next_old=linebuf;
			first_new=next_new=genbuf;
			m=n;
			do {
				if (--m <= 0) {
					dosub();
					if (!gsubf)
			break;
				}
				/* can't find something at EOL twice */
				if (*loc2=='\0')
			break;
				/* can't match same location twice */
				if (loc1==loc2)
					loc2++;
			} while (execute(0));
			if (m<=0) {
				inglob =| TRUE;
				p=next_old;
				do ; while (*p++);
				place(next_old,p);
				getmem();	/* discard result */
				replace(a,putline());
				m=append(getmem,a);
				a =+ m;
				addr2 =+ m;
			}
		}
	}
	if (!inglob)
		error('s');
}

dosub()
{
	register char c, *p;
	register int n;

	place(next_old,loc1);
	next_old=loc2;
	p=rhsbuf;
	while (c = *p++) {
		if (c=='&')
			place(loc1,loc2);
		else if (0<=(n=c-('1'+Q)) && n<nbra)
			place(braslist[n],braelist[n]);
		else {
			*next_new++ = c =& ~Q;
			if (next_new >= genbuf+LBSIZE)
				error('l');
		}
	}
}

place(l1, l2)
register char *l1, *l2;
{
	register char *sp;

	sp = next_new;
	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			error('l');
	}
	next_new = sp;
}

move(cflag)
{
	register int *adt, *ad1, *ad2;
	extern int getcopy();

	setdot();
	nonzero();
	if ((adt = address(0))==0)
		error('x');
	newline();
	ad1 = addr1;
	ad2 = addr2;
	lock++;
	if (cflag) {
		ad1 = dol;
		append(getcopy, ad1);
		inc(ad1);
		ad2 = dol;
	}
	if (ad1<=adt && adt<ad2)
		error('m');
	dot = adt;
	inc(ad2);
	inc(adt);
	if (adt<=ad1) {
		dot =+ ad2-ad1;
		addr2 = ad2;
		ad2 = ad1;
		ad1 = adt;
		adt = addr2;
	}
	reverse(ad1, ad2);
	reverse(ad2, adt);
	reverse(ad1, adt);
	unlock();
}

reverse(a1, a2)
register int *a1, *a2;
{
	register int t;

	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

int getcopy()
{
	if (addr1 > addr2)
		return(FALSE);
	getline(*addr1++);
	return(TRUE);
}

compile(eof)
char eof;
{
	register char c, *ep, *penultep;
	char *lastep, *bracketp, bracket[NBRA];

	if (eof == NL)
		error('x');
	if ((c = getchar()) == eof) {
		if (!expgood)
			goto cerror;
		return;
	}
	expgood = FALSE;
	ep = expbuf;
	lastep = 0;
	bracketp = bracket;
	nbra = 0;
	circfl = 0;
	peekc = c;
	if (c=='^') {
		peekc = EMPTY;
		circfl++;
	}
	for (;;) {
		c = getchar();
		if (c==eof) {
			if (bracketp!=bracket)
				goto cerror;
			*ep++ = CEOF;
			expgood = TRUE;
			return;
		}
		if (ep >= &expbuf[ESIZE-5])
			goto cerror;
		penultep = lastep;
		lastep = ep;

		switch (c) {

		case '\\':
			switch (c=getchar()) {
			case '(':
				if (nbra >= NBRA)
					goto cerror;
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			case ')':
				if (bracketp <= bracket)
					goto cerror;
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
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
			case NL:
				goto cerror;
			default:
				*ep++ = CCHR;
				*ep++ = c;
				continue;
			}

		case '.':
			*ep++ = CDOT;
			continue;

		case NL:
			goto cerror;

		case '[':
			*ep++ = CCL;
			penultep = ep;
			*ep++ = 0;
			if ((c=getchar()) == '^') {
				c = getchar();
				ep[-2] = NCCL;
			}
			do {
				if (isEOForNL(c) || ep>=&expbuf[ESIZE-3])
					goto cerror;
				*ep++ = c;
				c = getchar();
				if (c=='-') {
					c = getchar();
					if (isEOForNL(c) || c==']') {
						peekc = c;
						c = '-';
					} else {
						if (*--ep>c)
							goto cerror;
						*ep++ =| Q;
						*ep++ = c;
						c = getchar();
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
			if ((peekc=getchar()) == eof) {
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
	error('p');
}

/*
 * Execute the previously compiled pattern.
 *
 * addr is the address of the line to be searched (i.e. *addr is its
 *	disk address).  If addr==0, the line is assumed to already be
 *	in linebuf and the search continues from loc2.
 *
 * loc1 is set to the position of the start of the matched segment.
 *
 * loc2 is set to the position after the matched segment.
 *
 * returns TRUE iff successful.
 */

int execute(aaddr)
int *aaddr;
{
	register char *p1, *p2;
	register int *addr;

	if ((addr=aaddr) == 0) {
		if (circfl)
			return(0);
		p1 = loc2;
	} else {
		if (addr==zero)
			return(0);
		p1 = getline(*addr);
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

join()
{
	register int *a;
	register char *p, *q;

	rhsbuf[0] = '\0';
	if ((peekc=getchar()) == '/')
		comprhs(getchar());
	newline();
	first_new = p = genbuf;
	for (a=addr1;;) {
		for (q=getline(*a++); *q;) {
			*p++ = *q++;
			if (p >= genbuf+LBSIZE)
				error('l');
		}
		if (a>addr2)
	break;
		for (q=rhsbuf; *q;) {
			*p++ = *q++ & 0177;
			if (p >= genbuf+LBSIZE)
				error('l');
		}
	}
	*p++ = '\0';
	next_new = p;
	delete();
	append(getmem,addr1-1);
}

undo()
{
	register int *a;

	a = zero;
	do {
		inc(a);
		if (a>dol)
			error('u');
	} while ((*a|01) != new_line);
	replace(a,old_line);
	dot=a;
}

int next_col(nc,cp,input)
register int nc;
char *cp;
int input;
{
	register char c, *p;

	c = *cp;
	p=ttyty;
	if (c=='\t')
		nc =| 07;
	else if (c<' ' || c=='\177') {
#ifdef S2741
		nc++;
		if (p==dualcase || p==monocase || c=='\b' || c=='\r')
#endif
			error('b');	/* invalid character in x data */
	} else {
		if (input) {
			if (p==monocase) {
				if (c=='\\') {
					c=cp[1];
					p=esc_monocase;
					if ('a'<=c && c<='z')
						nc++;
				} else if ('A'<=c && c<='Z'
				    || c==ttybuf.erase || c==ttybuf.kill)
					nc++;
			} else if (c==ttybuf.erase || c==ttybuf.kill)
				nc++;
		}
		while (*p && *p!=c)
			p++;
		if (*p)
			nc++;
	}
	return (++nc);
}

xform()
{
	register char *i, *m, *o;
	int *la, insert, change, ic, mc;
	char *tf, *tl;
	char *saveg;

	if (ttyty==0)
		error('b');	/* what kind of terminal is this? */
	saveg=globp;	/* even in global, read from tty */
	globp=0;
	listf=0;
	for (la=addr1; la<=addr2; la++) {
		getline(*la);
		change=FALSE;
		dot=la;
		do {
			puts(linebuf);
			m=rhsbuf;
			while ((*m = getchar())!=NL) {
				if (isEOF(*m++))
					error('b');	/* unexpected EOF */
				if (m==rhsbuf+RHSIZE)
					error('l');	/* out of space */
			}
			if (m==rhsbuf)
				break;
			change++;
			i=linebuf;
			o=genbuf;
			do ; while (*o++ = *i++);
			if (i+m-rhsbuf > linebuf+LBSIZE)
				error('l');	/* out of space */
			i=genbuf;
			o=linebuf;
			m=rhsbuf;
			insert=FALSE;
			ic=0;
			mc=0;
			while (*i && *m && !insert) {
				if(*i=='\t' && *m!='#' && *m!='^' && *m!='$') {
					ic=next_col(ic,i,FALSE);
					tf=m;
					tl=m;
					do {
						if (*m!=' ' && *m!='\t') {
							if(*m=='%')
								*m=' ';
							tl=m+1;
						}
						mc=next_col(mc,m++,TRUE);
					} while (ic>mc && *m && *m!='#' &&
						 *m!='^' && *m!='$');
					if (ic>mc) {
						ic=mc;
						if (*m)
							tl=m;
					} else {
						if (tl==m)
							i++;
						else
							ic--;
					}
					while (tf!=tl)
						*o++ = *tf++;
				} else {
					mc=next_col(mc,m,TRUE);
					*o = *m;
					switch (*m++) {
					case ' ':
					case '\t':
						break;
					case '^':
						mc=ic;
						insert++;
						break;
					case '$':
						i="";
						break;
					case '#':
						ic=next_col(ic,i++,FALSE);
						while(*m=='#' && ic>mc)
						      mc=next_col(mc,m++,TRUE);
						if (ic!=mc)
							error('b');
						break;
					case '%':
						*o = ' ';
						/* fall through */
					default:
						o++;
						ic=next_col(ic,i++,FALSE);
					} /* switch */
				} /* else */
				for (;;) {
					if (ic>mc && *m) {
						if (*m!=' ' && *m!='\t')
							error('b');
						mc=next_col(mc,m++,TRUE);
					} else if (mc>ic && *i) {
						ic=next_col(ic,i,FALSE);
						*o++ = *i++;
					} else
						break;
				}
			} /* while */
			if (mc>ic && m[-1]=='\t')
				*o++ = '\t';
			if (insert && (*o++ = *m++) == '\0') {
				replace(la,putline());
				first_new = i;
				do ; while (*i++);
				next_new = i;
				append(getmem,la);
				la++;
				addr2++;
				change = FALSE;
			} else {
				while (*m)
					*o++ = *m++;
				do ; while (*o++ = *i++);
			}
		} while (insert || pflag);	/* exits by break too */
		if (change)
			replace(la,putline());
	} /* for */
	pflag=0;
	globp=saveg;
}

print()
{
	register int *a;

	setdot();
	nonzero();
	a = addr1;
	do {
		puts(getline(*a++));
	} while (a <= addr2);
	dot = addr2;
	pflag = 0;
}

#ifdef	PDP11
/*
 *	In version 6, ldiv() is in the library. In version 7, it is separate:
	.globl	_ldiv, _ldivr
	_ldiv:
		mov	2(sp), r0
		mov	4(sp), r1
		div	6(sp), r0
		mov	r1,_ldivr
		rts	pc
	.bss
	_ldivr:	.=.+2
 */
putd()
{
	register char r;
	extern int ldiv(), ldivr;

	/* the following pornography saves bundles of memory */
	count = ldiv(count,10);
	r = ldivr + '0';
	if (count)
		putd();
	putchar(r);
}
#endif
#ifndef	PDP11
putd()
{
	register r;
	r = count%10;
	count /= 10;
	if(count)
		putd(count);
	putchar('0'+r);
}
#endif

puts(sp)
register char *sp;
{
	col = 0;
	do {
		putchar(*sp);
	} while (*sp++);
}


putchar(c)
register char c;
{
	register char *lp;

	lp = linp;
	if (listf) {
		if (c==NL) {
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
	if (c==NL)
		c = '\n';
	*lp++ = c;
	if(c == '\n' || lp >= &line[(sizeof line)-2-4]) {
		write(1, line, lp-line);
		lp = line;
	}
	linp = lp;
}
