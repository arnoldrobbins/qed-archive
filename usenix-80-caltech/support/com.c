/*% cc -O %
 *
 * com file ...
 * looks for the sequence `/*%' in a file, and sends the rest of the
 * line off to the shell, after replacing any instances of a `%' character
 * with the filename.  Com is mainly useful so that information about
 * how to compile programs can be stored with the program.
 */
main(argc, argv)
char *argv[];
{
	register i;
	if(argc==1){
		printf("Usage: com file ...\n");
		exit(1);
	}
	for(i=1;i!=argc;i++)
		make(argv[i]);
}
make(file)
char file[];
{
	char command[512];
	int ibuf[259];
	register char *s, *t;
	register c;
	if(fopen(file, ibuf)<0){
		perror(file);
		exit(1);
	}
	c=getc(ibuf);
	while(c>=0){
		if(c=='/'){
			if((c=getc(ibuf))=='*'
			&& (c=getc(ibuf))=='%'){
				s=command;
				do;while((c=getc(ibuf))==' ' || c=='\t');
				while(c>=0 && c!='\n'){
					if(c=='%'){
						t=file;
						while(*t)
							*s++ = *t++;
					}
					else
						*s++=c;
					c=getc(ibuf);
				}
				*s='\0';
				printf("%s\n", command);
				switch(fork()){
				case -1:
					printf("Try again later\n");
					exit(1);
				case 0:
					execl("/bin/sh",
						"sh", "-c", command, 0);
					printf("No shell!\n");
					exit(1);
				default:
					wait();
					close(ibuf[0]);
					return;
				}
			}
		}
		else
			c=getc(ibuf);
	}
	printf("%s: no command\n", file);
	close(ibuf[0]);
	return;
}
