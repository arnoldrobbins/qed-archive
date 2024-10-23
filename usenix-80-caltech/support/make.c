#
char copr[] "Copyright (c) 1976 Thomas S. Duff";
/*
 * If you have the new C preprocessor, and wish to use it to
 * preprocess makefiles, you should define the symbol CPP
 */
#define	CPP	Hi, mom!
#define	HUP	1
#define	QUIT	3
#define	INTR	2
#define NDEP	1024
#define NFILES	512
/*
 * The earliest time is the
 * time at which the big bang occurred.
 */
#define	BIGBANG (-2147483648L)
struct file{
	char *fname;
	long int mdate;
	int level;
	int mark;
}file[NFILES];
struct dep{
	struct file *ante;
	struct file *cons;
	char *action;
}dep[NDEP];
struct dep *enddep dep;
struct statbuf{
	char minor;
	char major;
	int inumber;
	int flags;
	char nlinks;
	char uid;
	char gid;
	char size0;
	int size1;
	int addr[8];
	long int actime;
	long int modtime;
}statbuf;
int peekc;
char tempname[] "/tmp/makez99999";
int maxlevel;
int debug;
int eflag;
int oflag;
long now;
main(argc, argv)
char *argv[];
{
	register i;
	register char **ap;
#ifdef CPP
	char *av[64];
#endif
	char *infile;
	extern interrupt();
	int status;
	if(signal(INTR, interrupt)==1)
		signal(INTR, 1);
	if(signal(HUP, interrupt)==1)
		signal(HUP, 1);
	if(signal(QUIT, interrupt)==1)
		signal(QUIT, 1);
	i=mktemp(tempname, 0600);
	if(i<0)
		err("can't create temp file");
	close(i);
#ifdef CPP
	ap=av;
	*ap++="cpp";
#endif
	infile=0;
	for(;;){
		--argc;
		argv++;
		if(argc==0)
			break;
		if(argv[0][0]!='-'){
			if(infile)
				err("multiple makefiles not allowed");
			infile = argv[0];
		}
		else switch(argv[0][1]){
		case 'v':
		case 'V':
			debug++;
			break;
		case 'e':
			eflag++;
			break;
		case 'o':
			oflag++;
			break;
#ifdef CPP
		case 'P':
		case 'D':
		case 'E':
		case 'U':
		case 'I':
			*ap++=argv[0];
			break;
#endif
		default:
			err("bad flag %s", argv[0]);
		}
	}
#ifdef CPP
	if(infile==0)
		*ap++="makefile";
	else
		*ap++=infile;
	*ap++=tempname;
	*ap=0;
	if(fork()==0){
		for(i=3;i!=15;i++)
			close(i);
		execv("/lib/cpp", av);
	}
	else
		wait(&status);
	time(&now);
	input(tempname);
#else
	input(infile?infile:"makefile");
#endif
	resolve();
	leave(0);
}
input(fname)
char fname[];
{
	extern fin;
	register struct dep *d;
	register c;
	fin=open(fname, 0);
	if(fin<0){
		perror(fname);
		leave(1);
	}
	for(d=dep;d!=dep+NDEP;d=enddep){
		/* RCP Aug/79 -- ':' at start of line signifies comment */
		while(any(c=getc(), " \t#:\n"))
			if(c=='#'||c==':')
				do
					c=getc();
				while(c!='\n' && c!='\0');
		peekc=c;
		if((d->cons=getfile())==0)
			break;
		if((d->ante=getfile())==0)
			break;
		if((d->action=getstr())==0)
			break;
		if(debug)
			printf("cons: %s ante: %s action: %s\n",
				d->cons->fname,
				d->ante->fname,
				d->action);
		enddep++;
	}
	if(enddep==dep+NDEP && getstr()!=0)
		err("too many dependencies");
	close(fin);
	fin=0;
}
#define	ARMAGIC	0177545
long getmdate(name){
	register char *s, *t;
	register f;
	long mdate;
	char arch[512];
	int magic;
	struct{
		char a_name[14];
		long a_date;
		char a_uid;
		char a_gid;
		int a_mode;
		long a_size;
	}arbuf;
	long d;
	if(any(':', name)){
		s=name;
		t=arch;
		while(*s!=':')
			*t++ = *s++;
		*t='\0';
		s++;
		if((f=open(arch, 0))<0)
			return(BIGBANG);
		if(read(f, &magic, 2)<0 || magic!=ARMAGIC)
			goto NoLuck;
		while(read(f, &arbuf, sizeof arbuf)==sizeof arbuf){
			d=arbuf.a_date;
			arbuf.a_name[14]=0;	/* zaps part of a_date */
			if(eq(arbuf.a_name, s)){
				close(f);
				return(d);
			}
			lseek(f, (arbuf.a_size+1)&~1L, 1);
		}
	NoLuck:
		close(f);
		return(BIGBANG);
	}
	else if(stat(name, &statbuf)<0)
		return(BIGBANG);
	else
		d=statbuf.modtime;
	return(d);
}
getfile(){
	char name[512];
	register char *s;
	register c;
	register struct file *p;
	s=name;
	do;while(any(c=getc(), " \t"));
	while(c && !any(c, " \t\n")){
		*s++=c;
		c=getc();
	}
	*s='\0';
	if(c=='\0')
		return(0);
	peekc=c;
	p=lookup(name);
	if(any(':', name)){
		if(++enddep==dep+NDEP)
			err("too many dependencies");
		s=name;
		while(*s!=':')
			s++;
		*s='\0';
		enddep->ante=p;
		enddep->cons=lookup(name);
		enddep->cons->level = -1;
		enddep->action="";
		if(debug)
			printf("Implied cons: %s ante: %s\n",
				enddep->cons->fname, enddep->ante->fname);
	}
	p->level = -1;
	return(p);
}
getc(){
	register c;
	if(peekc){
		c=peekc;
		peekc=0;
		return(c);
	}
	return(getchar());
}
getstr(){
	char str[512];
	register char *s;
	register c;
	s=str;
	do;while(any(c=getc(), " \t"));
	while(c && c!='\n'){
		if(c=='\\' && (c=getc())!='\n'){
			peekc=c;
			c='\\';
		}
		*s++=c;
		c=getc();
	}
	*s='\0';
	if(c)
		return(copy(str));
	else
		return(0);
}
any(c, s)
register char c, *s;
{
	while(*s)
		if(*s++==c)
			return(1);
	return(0);
}
err(m){
	extern fout;
	fout=2;
	printf("Make error: %r", &m);
	printf(".\n");		/* Is there a bug in %r? */
	flush();
	leave(1);
}
interrupt(){
	leave(2);
}
leave(status){
	unlink(tempname);
	exit(status);
}
copy(str)
char str[];
{
	register char *s, *t, *result;
	s=str;
	if((result=sbrk(length(s)+1)) == -1)
		err("out of core");
	t=result;
	do;while(*t++ = *s++);
	return(result);
}
lookup(str)
char str[];
{
	register struct file *f, *hash;
	register char *s;
	s=str;
	hash=gethash(s);
	f=hash;
	do{
		if(f->fname==0){
			f->fname=copy(s);
			f->mdate=getmdate(s);
			if(f->mdate>now)
				printf("%s: modified %s", str, ctime(&f->mdate));
			return(f);
		}
		if(eq(s, f->fname))
			return(f);
		if(++f==file+NFILES)
			f=file;
	}while(f!=hash);
	err("too many files");
}
gethash(s)
register char *s;
{
	register h;
	h=0;
	while(*s)
		h =+ *s++;
	return(file+h%NFILES);
}
eq(as, at)
char as[], at[];
{
	register char *s, *t;
	s=as;
	t=at;
	while(*s)
		if(*s++ != *t++)
			return(0);
	return(*t=='\0');
}
int shbuf[259];
resolve(){
	register n;
	register struct dep *d;
	register char **ap;
	long l, m;
	char av[5];
	int status;
	calclevel();
	if(fcreat(tempname, shbuf)<0)
		err("temp file lost");
	for(n=1;n<=maxlevel;n++){
		for(d=dep;d!=enddep;d++)
			if(d->cons->level==n){
				/*
				 * Should read:
				 * if(d->ante->mdate>=d->cons->mdate)
				 * but there's a bug in the C compiler.
				 */
				l=d->ante->mdate;
				m=d->cons->mdate;
				if(l>=m
				&& d->action[0])
					appendaction(d);
			}
		for(d=dep;d!=enddep;d++)
			if(d->cons->level==n){
				l=d->ante->mdate;
				m=d->cons->mdate;
				if(l>=m)
					if(d->action[0])
						d->cons->mdate=now;
					else
						d->cons->mdate=d->ante->mdate;
				else if(debug){
					printf("No update %s %D\n",
						d->ante->fname,
						d->ante->mdate);
					printf("\t%s%D\n", d->cons->fname,
						d->cons->mdate);
				}
			}
	}
	fflush(shbuf);
	close(shbuf[0]);
	ap=av;
	*ap++="sh";
	if(eflag)
		*ap++="-e";
	if(oflag)
		*ap++="-o";
	*ap++=tempname;
	*ap=0;
	if(fork()==0)
		execv("/bin/sh", av);
	else
		wait(&status);
}
mktemp(name, mode)
char *name;
{
	register char *p;
	register i, n;
	extern ldivr;
	p=name;
	while(*p)
		p++;
	n=getpid();
	for(i=0;i!=5;i++){
		n=ldiv(0, n, 10);
		*--p='0'+ldivr;
	}
	--p;
	for(*p='a';*p<='z';++*p)
		if(stat(name, &statbuf)<0)
			if((n=creat(name, mode))>=0)
				return(n);
	return(-1);
}
appendaction(d)
struct dep *d;
{
	int f, pid;
	register char *s, *arg1, *arg2;
	char *t;
	arg1=d->ante->fname;
	if(any(':', arg1))
		do;while(*arg1++!=':');
	arg2=d->cons->fname;
	if(any(':', arg2))
		do;while(*arg2++!=':');
	s=d->action;
	if(debug)
		printf("exec: ");
	while(*s){
		if(*s=='$' && (s[1]=='1' || s[1]=='2')){
			if(s[1]=='1')
				t=arg1;
			else
				t=arg2;
			s=+2;
			if(debug)
				printf("%s", t);
			while(*t)
				putc(*t++, shbuf);
		}
		else{
			if(debug)
				putchar(*s);
			putc(*s++, shbuf);
		}
	}
	if(debug)
		printf("\n");
	putc('\n', shbuf);
}
length(s)
register char *s;
{
	register i;
	i=0;
	while(*s++)
		i++;
	return(i);
}
calclevel(){
	register struct file *f;
	for(f=file;f!=file+NFILES;f++)
		if(f->fname)
			follow(f, 0);
}
follow(af, alevel)
struct file *af;
{
	register struct file *f;
	register level;
	register struct dep *d;
	f=af;
	/* RCP Aug/79 -- print out the goddamned name! */
	if(f->mark)
		err("circularity on %s", f->fname);
	level=alevel;
	if(f->level>=level)
		return;
	f->level=level;
	f->mark++;
	if(level>maxlevel)
		maxlevel=level;
	for(d=dep;d!=enddep;d++)
		if(d->ante==f)
			follow(d->cons, level+1);
	f->mark=0;
}
