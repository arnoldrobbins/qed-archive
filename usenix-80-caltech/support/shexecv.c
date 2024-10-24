/*
 * shexecv(command, arglist)
 *
 * Execute a command the way the Shell does.
 *
 * The given command is sought in the current directory,
 * then in the user's own "bin" directory, in "/bin", and in
 * "/usr/bin".  If this fails an attempt is made to execute the
 * specified command name as a command file in the current
 * directory.
 *
 * This standard search sequence can be modified in the usual
 * way with the use of a ".vars" file; this is described in sh(I).
 *
 * Normally does not return; returns -1 if the command cannot
 * be executed.
 *
 * Copyright (c) 1978  R. Ronald Gomes.
 *
*/

#define READ	0
#define ERROR	-1
#define cycle	for(;;)

#define SHNAME	"/bin/sh"
#define ENOEXEC	8

struct inode
	{
	char junk1[4];
	int flags;
	char junk2[30];
	};

#define FMT	060000
#define TFILE	000000

shexecv(command, arglist)
char *command;
char *arglist[];
	{
	char searchstring[256], *newarg[256], buf[256];
	char *searchp, *bufp;
	char **newargp, **argp;
	int fd;
	struct inode statbuf;

	_gtsrch(searchstring);
	searchp = searchstring;
	do
		{
		bufp = buf;
		while((*searchp != '\0') && (*searchp != ':'))
			*bufp++ = *searchp++;
		copystr(command, bufp);
		_texec(buf, arglist);
		}
	while(*searchp++ != '\0');
	/*
	 * Last chance -- command file in current directory.
	*/
	if((fd = open(command, READ)) >= 0)
		{
		fstat(fd, &statbuf);
		close(fd);
		/*
		 * Make sure it's a plain file with execute bits off.
		*/
		if((statbuf.flags & (FMT | 011)) == TFILE)
			{
			newarg[0] = SHNAME;
			newarg[1] = command;
			newargp = &newarg[2];
			argp = &arglist[1];
			while(*newargp++ = *argp++);
			execv(newarg[0], newarg);
			write(2, "No shell!\n", 10);
			exit(1);
			}
		}
	return(-1);
	}

_gtsrch(searchstring)
char *searchstring;
	{
	int varfd, n;
	char *cp, c;
	char homedir[150], varf[256];

	copystr(":/bin/:/usr/bin/", searchstring);
	if(gethdir(getuid() & 0377, homedir) == ERROR)
		return;
	concatn(searchstring, ":", homedir, "/bin/:/bin/:/usr/bin/", 0);
	concat(varf, homedir, "/.vars");
	if((varfd = open(varf, READ)) < 0)
		return;
	cycle
		{
		if(read(varfd, &c, 1) != 1)
			break;
		if(c != 'S')
			while(((n = read(varfd, &c, 1)) == 1) && (c != '\n'));
		else
			{
			cp = searchstring;
			while((read(varfd, &c, 1) == 1) && (c != '\n'))
				*cp++ = c;
			if(cp != searchstring)
				*cp = '\0';
			break;
			}
		if(n == 0)
			break;
		}
	close(varfd);
	}

_texec(file, arg)
char *file;
char *arg[];
	{
	char *newarg[256];
	char **newargp, **argp;
	extern int errno;

	execv(file, arg);
	if(errno != ENOEXEC)
		return;
	newarg[0] = SHNAME;
	newarg[1] = file;
	newargp = &newarg[2];
	argp = &arg[1];
	while(*newargp++ = *argp++);
	execv(newarg[0], newarg);
	write(2, "No shell!\n", 10);
	exit(1);
	}
