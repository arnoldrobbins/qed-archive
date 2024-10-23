/*% cc -s -O % -o qedfile; echo "chmod 6755 qedfile when ready"
 */
#define	LSIZE	32
#ifdef	V7
#define	seek(a, b, c)	lseek(a, (long) b, c)
#endif
char	temp[LSIZE];
char	qfile[] "/etc/qedfile";
main(argc,argv) int argc; char **argv;
{
	register int fid;
	int user;
	register int i;
	if((fid = open(qfile,2)) == -1){
		if((fid = creat(qfile,0644)) == -1){
			perror(qfile);
			exit(1);
		}
		close(fid);
		if((fid = open(qfile,2)) == -1){
			perror(qfile);
			exit(1);
		}
	}
#ifdef	V7
	user = geteuid();
#endif
#ifndef	V7
	user = (getuid() & 0377);
#endif
	--argc;
	argv++;
	seek(fid,user*LSIZE,0);
	if(read(fid,temp,32) != 32){
		printf("qedfile: beyond end of %s\n",qfile);
		exit(1);
	}
	if(argc)
		printf("Was: %s\n",temp);
	else{
		printf("Is: %s\n",temp);
		exit(0);
	}
	if(strlength(*argv) >= LSIZE){
		printf("qedfile: long file name %s\n",*argv);
		exit(1);
	}
	strcpy(*argv, temp);	/* temp = *argv */
	i = strlength(temp);
	while(i < LSIZE){
		temp[i++] = 0;
	}
	seek(fid,user*LSIZE,0);
	write(fid,temp,LSIZE);
	exit(0);
}
strlength(pp) register char *pp; {
	register char *p;
	for (p = pp; *p++; );
	return (p - pp - 1);
}
strcpy(a, b)
	register char *a, *b;
{
	do; while(*b++ = *a++);
}
