/*% cc -c -O %
 */
#include "vars.h"
char	*nextip;
/*
 * newfile: read a new file name (if given) from command line.
 *
 *	nullerr: if true, error if file is null.
 *	savspec: decides if string[savedfile] is set.
 *		If savspec==SAVEIFFIRST, only save if deffile is null.
 *	deffile: default file name.
 *
 *	return value: string[FILEBUF]!=deffile
 *		      (unless SAVEIFFIRST && deffile null ==> return FALSE)
 *	side effects:
 *		can set string[savedfile]
 *		always sets string[FILEBUF]
 *		zeroes count
 */
newfile(nullerr, savspec, deffile)
	char *deffile;
{
	register char c;

	count = 0L;
	cpstr(deffile, genbuf);	/* in case we strcompact() */
	startstring();
	c = getchar();
	if(c == '\n')
		copystring(genbuf);
	else {
		if(c != ' ')
			error('f');
		do  c = getchar();  while(c == ' ');
		while(posn(c, " \t\n") < 0){
			if(c<' ' || c=='\177')
				error('f');
			addstring(c);
			c = getchar();
		}
	}
	setstring(FILEBUF);
	if(nullerr && string[FILEBUF].str[0]=='\0')
		error('f');
	if(savspec==SAVEALWAYS || (savspec==SAVEIFFIRST && genbuf[0]=='\0')){
		startstring();
		dupstring(FILEBUF);
		setstring(savedfile);
		if(savspec==SAVEIFFIRST && genbuf[0]=='\0')
			return(FALSE);
	}
	return(!eqstr(genbuf, string[FILEBUF].str));
}
exfile()
{
	close(io);
	io = -1;
	if (vflag) {
		putd();
		putchar('\n');
	}
}
getfile()
{
	register c;
	register char *lp, *fp;
	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0) {
				if(ninbuf < -1)
					error('r');
				if(lp != linebuf)
					error('N');
				return(EOF);
			}
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE])
			error('l');
		if ((*lp++ = c = *fp++ ) == 0) {
			lp--;
			continue;
		}
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}
putfile()
{
	int *a1;
	register char *fp, *lp;
	register nib;
	nib = 512;
	fp = genbuf;
	a1 = addr1;
	if(a1 == zero)
		*a1++;
	while(a1 <= addr2){
		lp = getline(*a1++, linebuf);
		for(;;){
			if (--nib < 0) {
				if(write(io, genbuf, fp-genbuf) < 0)
					error('w');
				nib = 511;
				fp = genbuf;
			}
			count++;
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	}
	write(io, genbuf, fp-genbuf);
}
#define SIGHUP	1
#define SIGINTR	2
#define SIGQUIT	3
#define	SIGBPIPE 13
Unix(type)
	char type;
{
	register pid, rpid;
	register char *s;
	int *a;
	int savint;
	int onbpipe;
	int retcode;
	char unixbuf[512];
	int	piper[2];
	if(type == '!') 
		setnoaddr();
	else {
		if(type == '>')
			nonzero();
		count = 0;
		if(pipe(piper) == -1){
			lastc = '\n';
			error('|');
		}
	}
	for(s=unixbuf;(*s=getchar())!='\n' && *s!=EOF;s++){
		if(s>unixbuf+512)
			error('l');
	}
	*s='\0';
	if ((pid = fork()) == 0) {
		signal(SIGHUP, onhup);
		signal(SIGQUIT, onquit);
		if(type == '<'){
			close(1);
			dup(piper[1]);
		}else if(type == '>'){
			close(0);
			dup(piper[0]);
		}
		if (type != '!') {
			close(piper[0]);
			close(piper[1]);
		}
		execl("/bin/sh", "sh", "-c", unixbuf, 0);
		exit(-1);
	}
	if(pid == -1){
		puts("Can't fork");
		error('!');
	}
	savint = signal(SIGINTR, 1);
	if(type == '<') {
		close(piper[1]);
		io = piper[0];
		ninbuf = 0;
		append(getfile,addr2);
		exfile();
	} else if(type == '>') {
		onbpipe = signal(SIGBPIPE, 1);
		close(piper[0]);
		a=addr1;
		do{
			s=getline(*a++, linebuf);
			do; while(*s++);
			*--s='\n';
			if (write(piper[1],linebuf,s=- linebuf-1)<0){
				puts("?o");
				break;
			}
		}while (a<=addr2);
		close(piper[1]);
		signal(SIGBPIPE, onbpipe);
	}
	while ((rpid = wait(&retcode)) != pid && rpid != -1);
	truth = (retcode>>8)&0377;
	signal(SIGINTR, savint);
	if(vflag)
		puts("!");
}
