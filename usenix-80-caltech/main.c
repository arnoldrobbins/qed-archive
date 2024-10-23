/*% cc -c -O %
 */
#include "vars.h"
#define SIGHUP	1
#define SIGINTR	2
#define SIGQUIT	3
#define XEQFILE	"/etc/qedfile"
#define XEQFILESIZE	32
#define	UP	1
#define DOWN	0
struct buffer *curbuf = buffer;
char	*linp	line;
int	appflag	0;
int	prflag	0;	/* prflag==1 ==> much code to get it right. use the startup buffer */
int	lastttyc '\n';
int	gflag	0;
int	tflag	0;
int	vflag	0;	/* initialization code turns it on */
int	uflag	's';
int	dflag	0;
int	initflag 1;
int	nflag;
char	nchars[] = "rspPlL";
int 	*option[] {
	&cflag,	&dflag,	&iflag,	&prflag,&tflag,	&vflag,
};
char 	opcs[] "cdipTv";
int	pagesize 21;
int	tfile	-1;
struct	sgttyb ttybuf { 0, 0, '\b', '\b', /*0*/};
char	correspondence[] "|^\\`[]{}~";
char	monocase[] "`|~{}";
char	esc_monocase[] "'!^()";
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
	exit();
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
	write(fi, buffer, sizeof buffer);
	write(fi, strarea, sizeof strarea);
	lock++;
	shiftstring(DOWN);
	write(fi, string, sizeof string);
	shiftstring(UP);
	unlock();
	close(fi);
}
restor()
{
	register i, t;
	register struct buffer *b;
	char	svfname[512];
	int fi;
	int getfile();
	curbuf = buffer;
	i = curbuf->zero;
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
	if(read(fi,buffer,sizeof buffer) != sizeof buffer
		|| read(fi, strarea, sizeof strarea) != sizeof strarea
		|| read(fi, string, sizeof string) != sizeof string)
		error('R');
	close(fi);
	shiftstring(UP);
	/*
	 * This code is incredibly sleazy and machine dependent for the 11.
	 * Sorry. There is no good reason why we don't have a shiftbuf():
	 * feel free to write one (but lock before you call it!).
	 */
	t = buffer[0].zero;
	if(i != t){
		i -= t;
		i >>= 1;
		for(b = buffer;b <= &buffer[NBUFS-1];b++){
			b->zero =+ i;
			b->dot =+ i;
			b->dol =+ i;
		}
	}
	newbuf(0);
	error(0);	/* ==> error, but don't print anything. calls unlock() */
}
filea()
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

fileb()
{
	register struct string *sp;

	sp = &string[FILEBUF];
	sp->str[sp->len-2] = 'b';
	return(sp->str);
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
		exit();
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
char bootstrap[] = "G/^[a-?].+\t./r\n";

main(argc, argv)
char **argv;
{
	register char *p1, *p2;
	register i;
	int xeqdesc;
	char buf;
	int rvflag;
	int startup;

	argv++;
	startup=0;
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
		case 'x':
			if(argc == 2)
				goto casedefault;
			startstring();
			copystring(argv[1]);
			setstring(FILE(NBUFS-1));
			startup++;
			argv++;
			--argc;
		}
		argv++;
		--argc;
	}
	if (gtty(0,&ttybuf)>=0)
		ttyty = ttybuf.sg_flags&LCASE? monocase : dualcase;
	if(startup == 0){
		xeqdesc = open(XEQFILE, 0);
		if( xeqdesc != -1 ) {
#ifdef	V7
			lseek(xeqdesc, (long)(geteuid()*XEQFILESIZE), 0);
#endif
#ifndef	V7
			seek(xeqdesc, ((getuid()&0377)*XEQFILESIZE), 0);
#endif
			read(xeqdesc, genbuf, XEQFILESIZE);
			startstring();
			copystring(genbuf);
			setstring(FILE(NBUFS-1));
			close(xeqdesc);
			if(genbuf[0])
				startup++;
		}
	}
	fendcore = sbrk(0);
	curbuf = &buffer[0];
	init();
	if (onhup != 1)
		signal(SIGHUP, rescue);
	if (onintr != 1)
		signal(SIGINTR, interrupt);
	/*
	 * Build the initialization code in register z~
	 */
	if(startup)
		p1 = "b~ r\n\\b~\n";
	else
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
			bootstrap[6] = bname[argc+1];	/* only read argument files */
			copystring(bootstrap);
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
	pushinp(STRING, NBUFS-1);
	setexit();
	lastttyc = '\n';
	commands();
	unlink(tfname);
}

commands()
{
	register int *a;
	register char c, lastsep;
	int getfile(), gettty();
	int sign;
	int r;
	int changed;
	int startline;

	for (;;) {
	startline = (lastttyc == '\n' && peekc == 0);
	cprflag=prflag;
	c = ' ';
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
			addr1=a;
			addr2=a;
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
		if(nextchar() == 'd'){
			setnoaddr();
			getchar();	/* clears 'd' */
			cd();
			continue;
		}
		setdot();
		nonzero();
		setapp();
		delete();
		append(gettty, addr1-1);
		continue;
	case 'd':
		/* allow `d-': idiomatic but nice */
		if(posn(nextchar(),"\377\npPlL- \t") < 0)
			error('x');
		delete();
		continue;
	case 'e':
		setnoaddr();
		if(nextchar()!=' ')
			error('f');
		delall();
		addr1 = zero;
		addr2 = zero;
		newfile(TRUE, SAVEALWAYS, "");
		cflag = TRUE;	/* In case file open causes error */
		goto caseread;
	case 'f':
		setnoaddr();
		if((c = getchar()) != '\n'){
			ungetchar(c);
			if(newfile(FALSE, SAVEALWAYS, string[savedfile].str))
				cflag = TRUE;
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
		if (addr1 == addr2 && lastsep == ' '){
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
		case 'b':
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
			truth = *option[r];
			break;
		case 'k':
			errok=TRUE;
			a=address(0);
			if(a)	/* address is ok */
				count=a-zero;
			else
				count=0;
			errok=FALSE;
			while(lastc!='\n')
				getchar();
			break;
		case 'n':
			c = getchar();
			if((r=posn(c, nchars)) >= 0)
				nflag = r;	/* NOTE: nflag != c */
			else
				error('x');
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
		default:
			error('x');
		}
		continue;
	case '\n':
		if(addr2==0){
			if(stackp != &stack[0] || !startline)
				continue;
			if(*string[BROWSE].str){
				pushinp(BRWS);
				continue;
			}
			addr2 = dot + 1;
		}
		if(addr1 == 0 || lastsep == ';')
			addr1 = addr2;
		c='p';	/* fall through */
	case 'L':
	case 'l':
	case 'p':
	case 'P':
		display(c);
		continue;
	case 'q':
		setnoaddr();
		if(!startline || getchar()!='\n')
			error('x');
		unlink(tfname);
		exit();
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
			cflag = changed;
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
		substitute(stackp != &stack[0]);
	Case_noisy:
		if(nflag>1 && stackp== &stack[0]){	/* nflag>1 ==> a display char */
			if(posn(nextchar(), lchars) >= 0)
				c = getchar();
			else
				c = nchars[nflag];
			addr1=addr2=dot;
			display(c);
		}
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
		goto Case_noisy;
	case 'v':
		global(FALSE);
		continue;
	case 'V':
		globuf(FALSE);
		continue;
	case 'w':
		setall();
		nonzero();
	case 'W':
		setall();
		if(newfile(TRUE, SAVEIFFIRST, string[savedfile].str))
			changed = cflag;
		else
			changed = (addr1>(zero+1) || addr2!=dol);
		if((io = creat(string[FILEBUF].str, 0644)) < 0)
			error('o'|FILERR);
		putfile();
		cflag = changed;
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
		cflag = FALSE;
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
		setall();
	case '!':
		Unix(c);
		continue;
	case '#':
		allnums();
		continue;
	case '%':
		allstrs();
		continue;
	case EOF:
		return;
	}
	error('x');
	}
}
cd()
{
	register i;

	newfile(TRUE, SAVENEVER, "");
	if(chdir(string[FILEBUF].str) == -1)
		error('d'|FILERR);
	for(i=0; i<NBUFS; i++)
		if(string[FILE(i)].str[0] != '/')
			buffer[i].cflag = TRUE;
	if(string[savedfile].str[0] != '/')
		cflag = TRUE;		/* current buffer has its own cflag */
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
