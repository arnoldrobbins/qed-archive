/*% cc -c -O %
 */
#include "vars.h"
move(copyflag)
{
	register int *adt, *ad1, *ad2;
	int fb, tb;
	int todot;
	int tonewdot;
	int totmved;
	int temp;
	int getcopy();

	setdot();
	nonzero();
	fb = curbuf - buffer;
	temp = getchar();
	tb = posn(temp, bname);
	if(tb >= 0)
		chngbuf(tb);
	else{
		ungetchar(temp);
		tb = fb;
	}
	if ((adt = address())==0){
		chngbuf(fb);
		error('x');
	}
	todot = adt - buffer[tb].zero;	/* syncbuf not needed! */
	chngbuf(fb);
	ad1 = addr1;
	ad2 = addr2;
	totmved = ad2 - ad1 + 1;
	lock++;
	if (copyflag) {
		tonewdot = addr2 - buffer[fb].zero;
		/*
		 * NOTE: in the copy command
		 *	copies of the lines are created using append
		 *	and then moved to the target position.
		 *	They are appended at the dollar of their
		 *	original buffer. (guarenteed to be higher address)
		 *	They are NOT appended at the target position
		 *	since, if the target position was lower than their
		 *	source position, getcopy would have to account
		 *	for the shift of the addresses due to the insert
		 *	of the copies.
		 */
		ad1 = dol;
		temp = cflag;
		append(getcopy, ad1++);
		cflag = temp;
		ad2 = dol;
	} else
		tonewdot = addr1 - buffer[fb].zero - 1;
	ad2++;
	adt = buffer[tb].zero + todot;
	chngbuf(tb);
	if (adt<ad1) {
		if ((++adt)!=ad1){
			reverse(adt, ad1);
			reverse(ad1, ad2);
			reverse(adt, ad2);
		}
	} else {
		if (adt++ >= ad2) {
			reverse(ad1, ad2);
			reverse(ad2, adt);
			reverse(ad1, adt);
		} else {
			if(ad2 != zero + 1){
				error('m');
			}
		}
	}
	fixup(fb, tb, totmved);
	buffer[fb].dot = buffer[fb].zero + tonewdot;
	if(copyflag == 0 && fb == tb && todot >= tonewdot){
		todot -= totmved;
	}
	if(!copyflag)
		buffer[fb].cflag = TRUE;
	modified();
	dot = buffer[tb].dot = buffer[tb].zero + todot + totmved;
	zero = buffer[tb].zero;
	dol = buffer[tb].dol;
	unlock();
}
fixup(from,to,tot) int from, to, tot;
{
	register int b;
	register int n;
	register int lo;
	int hi;
	if(to == from){
		return;
	}
	if(to < from){
		n = tot;
		lo = to;
		hi = from;
	} else {
		n = -tot;
		lo = from;
		hi = to;
	}
	buffer[lo].dol += n;
	for(b = lo;++b < hi;){
		buffer[b].zero += n;
		buffer[b].dot += n;
		buffer[b].dol += n;
	}
	buffer[hi].zero += n;
}
reverse(a1, a2)
	register int *a1, *a2;
{
	register t;
	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}
getcopy()
{
	if (addr1 > addr2)
		return(EOF);
	getline(*addr1++, linebuf);
	return(0);
}
