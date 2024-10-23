#
/*%cc -s -O %
 */
char copr[] "Copyright (c) 1976 Thomas S. Duff";
/*
 * args -- copy standard input to arglist
 */
#define NARGS	256
#define NCHARS	512
char *args[NARGS];
char chars[NCHARS];
main(argc, argv)
char *argv[];
{
	register c, i;
	register char *s;
	char *argptr;
	extern int fout;
	if(argc>NARGS)
		goto Error;
	for(i=1;i!=argc;i++)
		args[i]=argv[i];
	s=chars;
	argptr=chars;
	while((c=getchar())!='\0'){
		if(s==chars+NCHARS)
				goto Error;
		if(c=='\n'){
			if(i==NARGS)
				goto Error;
			args[i++]=argptr;
			*s++='\0';
			argptr=s;
		}
		else
			*s++=c;
	}
	args[i]=0;
	shexecv(args[1], &args[1]);  /* rrg77.11.27 -- new command protocol */
	fout = 2;
	printf("Can't exec %s\n", args[1]);
	exit(1);
Error:
	fout = 2;
	printf("Too many args\n");
	exit(1);
}
