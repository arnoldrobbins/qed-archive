/*
 *	simulate QED's evil sbrk tricks within malloc
 *	D. P. Mitchell  94/12/09.
 */
#include "vars.h"
#include <stdio.h>
#define VSPACE 1000

static char *heap;
static char *breakValue;
static char *top;

#define   LDCHUNK 512	/* must be the same as in com.c! */

static
initHeap()
{

	if (heap == 0) {
		heap = (char *)malloc(LDCHUNK*sizeof(int *)*VSPACE);
		top = heap + LDCHUNK*sizeof(int *)*VSPACE;
		breakValue = heap;
	}
}

brk(char *endds)
{
	initHeap();
	if (endds >= heap && endds < top)
		breakValue = endds;
}

char *
sbrk(int incr)
{
	initHeap();
	breakValue += incr;
	if (breakValue >= top) {
		fprintf(stderr, "sbrk simulator failed!!\n");
		fprintf(stderr, "%d chunks allocated so far\n",
			(breakValue-heap)/LDCHUNK/sizeof(int*));
		exit(1);
	}
	return breakValue;
}
