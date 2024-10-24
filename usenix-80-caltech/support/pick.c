char copr[] "Copyright (c) 1976 Thomas S. Duff";
/*
 * pick -- choose which arguments to print
 */
main(argc, argv)
char *argv[];
{
	register i, c;
	register char *s;
	extern fin;
	fin=open("/dev/tty", 0);
	for(i=1;i!=argc;i++){
		s=argv[i];
		while(*s)
			write(2, s++, 1);
		write(2, " ", 1);
		if((c=getchar())=='y')
			printf("%s\n", argv[i]);
		else if(c=='x')
			exit(0);
		while(c!='\n' && c!='\0')
			c=getchar();
	}
}
