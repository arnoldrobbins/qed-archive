CFLAGS=-O
FILES=\
address.o\
blkio.o\
com.o\
getchar.o\
getfile.o\
glob.o\
main.o\
misc.o\
move.o\
pattern.o\
putchar.o\
setaddr.o\
string.o\
subs.o
a.out:	$(FILES)
	cc $(FILES)
$(FILES): vars.h
print:
	@pr vars.h address.c blkio.c com.c getchar.c getfile.c glob.c main.c\
	misc.c move.c pattern.c putchar.c setaddr.c string.c subs.c
