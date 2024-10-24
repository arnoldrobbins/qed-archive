/*%cc -s -n -O % -lS
/*
 * c - columnate
 *
 * c [-] [-LINEWIDTH] [-wNWORDS] [-t] [file ...]
 * - suppresses break on colon
 * -LINEWIDTH sets width of line in which to columnate (default 80)
 * -wNWORDS sets maximum number of words required (default 2048)
 * -t suppresses expanding multiple blanks into tabs
 *
 */
char copr[] = "Copyright (c) 1980 Robert C. Pike";
#include <stdio.h>
#define	WIDTH	80
#define	NWORD	2048
int linewidth = WIDTH;
int wordspace = NWORD;
int colonflag = 1;
int tabflag = 1;
char *cbuf, *cbufp;
char **word;
FILE *file;

main(argc, argv)
	char *argv[];
{
	FILE *fopen();
	register i;
	char buf[BUFSIZ];

	if(argc > 1){
		while(*argv[1] == '-'){
			--argc; argv++;
			switch(argv[0][1]){
			case '\0':
				colonflag = 0;
				break;
			case 'w':
				wordspace = atoi(&argv[0][2]);
				if(wordspace <= 0)
					wordspace = NWORD;
				break;
			case 't':
				tabflag = 0;
				break;
			default:
				linewidth = atoi(&argv[0][1]);
				if(linewidth <= 0)
					linewidth = WIDTH;
				break;
			}
		}
	}
	setbuf(stdout, buf);
	word = sbrk(wordspace * (sizeof *word));
	if(word == -1)
		error("no word space");
	cbuf = cbufp = sbrk(0);
	if(argc == 1){
		file = stdin;
		readbuf();
	}else{
		colonflag = 0;
		for(i=1; i<argc; i++){
			file = fopen(*++argv, "r");
			if(file == NULL)
				printf("c: can't open %s\n", *argv);
			else{
				readbuf();
				fclose(file);
			}
		}
	}
	columnate();
	exit(0);
}
error(s)
	char *s;
{
	fprintf(stderr, "c: %s\n", s);
	exit(1);
}
#define	ALLOCSIZE	1024
int maxwidth = 0;
int nchars = ALLOCSIZE;
int nwords = 0;
getword(p)
	register char *p;
{
	register w;
	w = 0;
	do{
		/*
		 * This is where core allocation happens.
		 * Note that we don't check the return value of
		 * sbrk except for error; it is assumed to all
		 * synchronize properly.  Sleazy but fast.
		 */
		if(nchars++ >= ALLOCSIZE){
			if(sbrk(ALLOCSIZE) == -1)
				error("out of memory");
			nchars = 0;
		}
		if((*p = getc(file)) <= 0)
			return(0);
		w++;
	}while(*p++ != '\n');
	*--p = 0;
	return(w);
}
readbuf(){
	register char *p;
	register w;
Restart:
	p = cbufp;
	while(w = getword(p)){		/* assignment = */
		/*
		 * if the last char was a colon; break
		 */
		if(colonflag && *(p+w-2) == ':'){
			columnate();
			nwords = 0;
			maxwidth = 0;
			cbufp = cbuf;
			putchar('\n');
			while(*p)
				putchar(*p++);
			putchar('\n');
			goto Restart;
		}
		if(w > maxwidth)
			maxwidth = w;
		if(nwords == wordspace-1)
			error("too many words");
		word[nwords++] = p;
		p += w;
	}
	cbufp = p;
}
int words_per_line;
int nlines;
int col;
int tabcol;
int endcol;
int maxcol;
columnate(){
	register char *p;
	register i, j;
	words_per_line = linewidth / maxwidth;
	nlines = (nwords+words_per_line-1)/words_per_line;
	for(i=0; i<nlines; i++){
		col = 0;
		endcol = 0;
		for(j=0; i+j<nwords; ){
			endcol += maxwidth;
			p = word[i+j];
			while(*p){
				putchar(*p++);
				col++;
			}
			if(i+(j+=nlines) < nwords){
				tabcol = (col|07)+1;
				if(tabflag)
					while (tabcol <= endcol) {
						putchar('\t');
						col = tabcol;
						tabcol += 8;
					}
				while (col != endcol) {
					putchar(' ');
					col++;
				}
			}
		}
		putchar('\n');
	}
}
