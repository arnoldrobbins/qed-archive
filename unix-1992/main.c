/*% cc -c -O %
 */
#include "vars.h"
#define SIGHUP	1
#define SIGINTR	2
#define SIGQUIT	3
#define	UP	1
#define DOWN	0
char lchars[]="pPlL";
struct buffer *curbuf = buffer;
char	*linp = line;
int	appflag = 0;
int	pagesize = PAGESIZE;
int	prflag = 0;	/* prflag==1 ==> much code to get it right. use the startup buffer */
int	lastttyc = '\n';
int	gflag = 0;
int	tflag = 0;
int	vflag = 0;	/* initialization code turns it on */
int	uflag = 's';
int	dflag = 0;
int	initflag = 1;
int	*option[] = {
	&cflag,	&dflag,	&eflag,	&iflag,	&prflag,&tflag,	&vflag,
};
char	opcs[] = "cdeipTv";
int	tfile = -1;
struct	sgttyb ttybuf = { 0, 0, '\b', '\b', /*0*/};
char	QEDFILE[]="QEDFILE";
int	(*pending)();

rescue()
{
	/* Save in qed.hup:[ab]q on hangup */
	signal(SIGHUP,1);
	if (lock) {
		pending = rescue;
		return;
	}
	startstring();
	copystring("qed.hup");
	setstring(FILEBUF);
	savall();
	exit(SIGHUP);
}
char *filea()
{
	register struct string *sp;
	register int i, d;
	char c;

	sp = &string[FILEBUF];
	startstring();
	d = 0;
	i = 0;
	while((c=sp->str[i]) != '\0'){
		addstring(c);
		i++;
		if(c == '/')
			d = i;
	}
	if((i-d) > 12)		/* file name is >= 13 characters long */
		string[NSTRING].str -= (i-(d+12));	/* truncate string */
	copystring(":aq");
	setstring(FILEBUF);
	return(sp->str);
}

char *fileb()
{
	register struct string *sp;

	sp = &string[FILEBUF];
	sp->str[sp->len-2] = 'b';
	return(sp->str);
}
savall()
{
	register fi;

	syncbuf();
	addr1 = buffer[0].zero + 1;
	addr2 = buffer[NBUFS-1].dol;
	if(addr1 > addr2){
		error('$');
		return;
	}
	if((io = creat(filea(), 0644)) < 0)
		error('o'|FILERR);
	putfile();
	exfile();
	if((fi = creat(fileb(), 0644)) < 0)
		error('o'|FILERR);
	write(fi, (char *)buffer, sizeof buffer);
	write(fi, strarea, sizeof strarea);
	lock++;
	shiftstring(DOWN);
	write(fi, (char *)string, sizeof string);
	shiftstring(UP);
	unlock();
	close(fi);
}
restor()
{
	register i, t;
	register struct buffer *b;
	int fi;
	int getfile();
	curbuf = buffer;
	i = (int)curbuf->zero;
	if((t = open(filea(), 0)) < 0){
		lastc = '\n';
		error('o'|FILERR);
	}
	initio();
	init();
	io = t;
	ninbuf = 0;
	append(getfile, dol);
	exfile();
	if((fi = open(fileb(),0)) < 0)
		error('o'|FILERR);
	lock++;
	if(read(fi,(char *)buffer,sizeof buffer) != sizeof buffer
		|| read(fi, strarea, sizeof strarea) != sizeof strarea
		|| read(fi, (char *)string, sizeof string) != sizeof string)
		error('R');
	close(fi);
	shiftstring(UP);
	/*
	 * This code is incredibly sleazy and machine dependent for the 11.
	 * Sorry. There is no good reason why we don't have a shiftbuf():
	 * feel free to write one (but lock before you call it!).
	 */
	t = (int)buffer[0].zero;
	if(i != t){
		i -= t;
		i >>= 1;
		for(b = buffer;b <= &buffer[NBUFS-1];b++){
			b->zero += i;
			b->dot += i;
			b->dol += i;
		}
	}
	newbuf(0);
	error(0);	/* ==> error, but don't print anything. calls unlock() */
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
	if(iflag){
		unlink(tfname);
		exit(SIGINTR);
	}
	linp=line;
	putchar('\n');
	lastc = '\n';
	error('?');
}

/*
 * Unlock: exit a critical section, invoking any pending signal routines.
 */

unlock()
{
	register (*p)();

	p = pending;
	pending = 0;
	lock = 0;
	if (p)
		(*p)();
}

char cleanup[] = "ba z~:\n";
char setvflag[] = "ov?";
char boot1[] = "G/^[";
char boot2[] = "].+\t./r\n";

main(argc, argv)
char **argv;
{
	register char *p1;
	register i;
	char buf;
	int rvflag;
	char *startup=(char *)0;

	argv++;
	onquit = signal(SIGQUIT, 1);
	onhup = signal(SIGHUP, 1);
	onintr = signal(SIGINTR, 1);
	rvflag = 1;
	for(i=0;i!=NSTRING;i++){
		string[i].str = nullstr;
		string[i].len=0;
	}
	/* initialize strfree */
	string[NSTRING].str = strchars;
	while(argc > 1 && **argv=='-'){
		switch(argv[0][1]){
		casedefault:
		default:
			rvflag = 0;
			break;
		case 'q':
		/* allow debugging quits? */
			signal(SIGQUIT, 0);
			break;
		case 'i':
		/* allow interrupt quits? */
			iflag++;
			break;
		case 'e':
		/* Exit on error? */
			eflag++;
			break;
		case 'x':
			if(argc == 2)
				goto casedefault;
			startup = argv[1];
			argv++;
			--argc;
		}
		argv++;
		--argc;
	}
	gtty(0,&ttybuf);
	if(startup==0)
		startup = getenv(QEDFILE);
	fendcore = (int *)sbrk(0);
	curbuf = &buffer[0];
	init();
	if (onhup != 1)
		signal(SIGHUP, rescue);
	if (onintr != 1)
		signal(SIGINTR, interrupt);
	/*
	 * Build the initialization code in register z~
	 */
	if(startup){
		startstring();
		copystring(startup);
		setstring(FILE(NBUFS-1));
		p1 = "b~ r\n\\b~\n";
	} else
		p1 = "";
	startstring();
	copystring(p1);
	setvflag[2] = "rs"[rvflag];
	copystring(setvflag);
	/*
	 * z~ now has startup-buffer initialization; prepare the file list
	 * and generate a GLOBUF to read them in
	 */
	if(--argc > 0) {
		if(argc >= 53)	/* buffer names a-zA-Z */
			puts("?i");
		else {
			copystring(boot1);
			for(i=0; i<argc; i++)	/* argument files only */
				addstring(bname[i]);
			copystring(boot2);
			copystring(cleanup);
			setstring(NBUFS-1);
			buf = 0;
			while(argc > 0) {
				startstring();
				copystring(*argv);
				setstring(FILE(buf++));
				--argc;
				argv++;
			}
		}
	}
	else{
		copystring(cleanup);
		setstring(NBUFS-1);
	}
	pushinp(STRING, NBUFS-1, FALSE);
	setexit();
	lastttyc = '\n';
	commands();
	unlink(tfname);
	exit(lasterr);
}

int	noaddr;
commands()
{
	register int *a;
	register c, lastsep;
	int getfile(), gettty();
	int r;
	int changed;
	long locn, lseek();
	int startline;

	for (;;) {
	startline = (lastttyc == '\n' && peekc == 0);
	cprflag=prflag;
	c = '\n';
	for (addr1=0;;) {
		lastsep = c;
		a=address();
		c=getchar();
		if (c!=',' && c!=';')
			break;
		if (lastsep==',')
			error('a');
		if (a==0) {
			a = zero+1;
			if (a>dol)
				--a;
		}
		addr1 = a;
		if (c==';')
			dot = a;
	}
	if (lastsep!='\n' && a==0)
		a=dol;
	if((addr2=a) == 0) {
		addr2=dot;
		noaddr = TRUE;
	} else
		noaddr = FALSE;

	if(addr1 == 0)
		addr1 = addr2;

	cprflag=FALSE;
	switch(c){
	case 'a':
		setdot();
		setapp();
		append(gettty, addr2);
		continue;
	case 'b':
		if(posn((c=nextchar()), bname)<0){	/* browse command */
			setdot();
			nonzero();
			bcom();
			continue;
		}
		c = getaz('b');
		setnoaddr();
		chngbuf(c);
		continue;
	case 'c':
		setdot();
		nonzero();
		setapp();
		append(gettty, addr2);
		a = dot-(addr2-addr1+1);
		delete();
		dot = a;
		continue;
	case 'd':
		if(posn(nextchar(),"\377\npPlL \t") < 0)
			error('x');
		delete();
		continue;
	case 'E':
	case 'e':
		setnoaddr();
		if(c=='e' && !eok && cflag){
			eok=TRUE;
			error('e');
		}
		newfile(TRUE, SAVEALWAYS, "");
		delall();
		addr1 = zero;
		addr2 = zero;
		modified();	/* In case file open causes error */
		goto caseread;
	case 'f':
		setnoaddr();
		if((c = getchar()) != '\n'){
			ungetchar(c);
			if(newfile(FALSE, SAVEALWAYS, string[savedfile].str))
				modified();
			if(vflag)
				ncom('f');
		}
		else
			ncom('f');
		continue;
	case 'g':
		global(TRUE);
		continue;
	case 'G':
		globuf(TRUE);
		continue;
	case 'h':
		setnoaddr();
		if(nextchar()=='\n')
			error('x');
		if('0'<=peekc && peekc<='9')
			until(TRUE, getnum());
		else
			until(FALSE, 0);
		continue;
	case 'i':
		setdot();
		nonzero();
		setapp();
		append(gettty, addr2-1);
		continue;
	case 'j':
		setdot();
		if (addr1 == addr2 && lastsep == '\n'){
			addr1--;
			if(addr1 <= zero)
				error('$');
		}
		nonzero();
		join();
		continue;
	case 'k':
		c = getaz(c);
		setdot();
		nonzero();
		names[c] = *addr2 | 01;
		continue;
	case 'm':
		move(FALSE);
		continue;
	case 'n':
	case 'N':
		ncom(c);
		continue;
	case 'o':
		setnoaddr();
		c = getchar();
		r=posn(c, opcs);
		if(r >= 0)
			setreset(option[r]);
		else switch(c) {
		case 'B':
			if(nextchar() == '\n')
				clearstring(BROWSE);
			else {
				startstring();
				while((c=getchar()) != '\n')
					addstring(c);
				copystring("\\N");
				setstring(BROWSE);
			}
			break;
		case '?':
			if ((r = posn(getchar(), opcs)) < 0)
				error('O');
			settruth(*option[r]);
			break;
		case 'q':
			c = getchar();
			if(c=='s' || c =='r')
				signal(SIGQUIT, c=='r');
			else
				error('x');
			break;
		case 'u':
			c = getchar();
			if(c == 'r')
				uflag = 0;
			else if(posn(c, "slu") >= 0)
				uflag = c;
			else
				error('x');
			break;
		case 'b':
			if((r=getnum()) > 0)
				pagesize = r;
			if(posn(nextchar(), lchars) >=0)
				bformat = getchar();
			break;
		default:
			error('x');
		}
		continue;
	case '\n':
		if (a==0) {
			if(stackp != &stack[0] || !startline)
				continue;
			if(*string[BROWSE].str){
				pushinp(BRWS, 0, FALSE);
				continue;
			}
			a = dot+1;
			addr2 = a;
			addr1 = a;
		}
		if (lastsep==';')
			addr1 = a;
		c = 'p';	/* fall through */
	case 'L':
	case 'l':
	case 'p':
	case 'P':
		display(c);
		continue;
	case EOF:
		return;
	case 'Q':
	case 'q':
		setnoaddr();
		if(c!=EOF && (!startline || getchar()!='\n'))
			error('x');
		if(c!='Q' && !qok){
			register struct buffer *bp;
			syncbuf();
			qok=TRUE;
			for(bp=buffer; bp<&buffer[NBUFS]; bp++)
				if(bp->cflag && (bp->dol>bp->zero ||
					string[FILE(bp-buffer)].str[0]))
					error('q');
		}
		unlink(tfname);
		exit(lasterr);
	case 'r':
		newfile(TRUE, SAVEIFFIRST, string[savedfile].str);
	caseread:
		if((io = open(string[FILEBUF].str, 0)) < 0){
			if(initflag){
				putchar('?');
				putchar('o');
				putchar(' ');
				puts(string[FILEBUF].str);
				continue;
			}
			lastc = '\n';
			error('o'|FILERR);
		}
		setall();
		changed = (zero!=dol);
		ninbuf = 0;
		append(getfile, addr2);
		if(eqstr(string[savedfile].str, string[FILEBUF].str))
			if(cflag = changed)	/* Assignment = */
				modified();
		/* else append got cflag right */
		exfile();
		continue;
	case 'R':
		setnoaddr();
		newfile(TRUE, SAVENEVER, "q");
		restor();
		continue;
	case 's':
		setdot();
		nonzero();
		substitute(stackp != &stack[0], -1);
		continue;
	case 'S':
		setnoaddr();
		newfile(TRUE, SAVENEVER, "q");
		savall();
		continue;
	case 't':
		move(TRUE);
		continue;
	case 'u':
		setnoaddr();
		undo();
		modified();
		continue;
	case 'v':
		global(FALSE);
		continue;
	case 'V':
		globuf(FALSE);
		continue;
	case 'W':
	case 'w':
		if(addr2==0 && dol==zero)
			error('$');
		setall();
		if(newfile(TRUE, SAVEIFFIRST, string[savedfile].str))
			changed = cflag;
		else
			changed = (addr1>(zero+1) || addr2!=dol);
		if(c=='w' || (io=open(string[FILEBUF].str,1))==-1){
		  Create:
			if ((io = creat(string[FILEBUF].str, 0666)) < 0)
				error('o'|FILERR);
		}else{
			if((locn=lseek(io, 0L, 2)) == -1L)
				goto Create;
			if(locn != 0L)	/* W on non-empty file */
				changed = TRUE;	/* PHEW! figured it out */
		}
		putfile();
		if(cflag = changed)	/* Assignment = */
			modified();
		exfile();
		continue;
	case 'x':
		setdot();
		nonzero();
		xform();
		continue;
	case 'y':
		jump();
		continue;
	case 'z':
		strcom(getaz('z'));
		continue;
	case 'Z':
		setnoaddr();
		if((c=getchar())!=' ' && c!='\n')
			error('x');
		delall();
		cflag=FALSE;
		clearstring(savedfile);
		continue;
	case '"':
		setdot();
		dot=addr2;
		comment();
		continue;
	case '=':
		setall();
		putdn((addr2-zero)&077777);
		continue;
	case '>':
	case '<':
	case '|':
		setall();
	case '!':
		Unix(c);
		continue;
	case '#':
		setnoaddr();
		allnums();
		continue;
	case '%':
		setnoaddr();
		allstrs();
		continue;
	}
	error('x');
	}
}
setreset(opt)
	register int *opt;
{
	register c;

	c = getchar();
	if(c!='s' && c!= 'r')
		error('x');
	*opt = (c=='s');
}
delall(){
	if(dol!=zero){
		addr1=zero+1;
		addr2=dol;
		delete();
	}
}
