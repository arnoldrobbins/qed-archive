#include <misc.h>  /* This file transformed from ec.h */ 

#define choicefile "/etc/prompt"

program
	{
	int desc;
	char c;

	desc = open(choicefile, read_write);
	if( desc is error ){
		/* */
		printf("Can't open %s\n", choicefile);
		exit(0);
	/*fi*/};

	seek(desc, (getuid()&0377)*2, 0);

	if( argc is 1 ){
		/* */
		read(desc, &c, 1);
		printf("%c\n", c);
	}else if( argc is 2 and ( *argv[1] is 'y' or *argv[1] is 'n' ) ){
		/* */
		write(desc, argv[1], 1);
	}else{
		printf("Usage:  prompt [y] [n]\n");
	/*fi*/};
	};
