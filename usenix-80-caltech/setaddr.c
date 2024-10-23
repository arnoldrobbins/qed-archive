/*% cc -c -O %
 */
#include "vars.h"
setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error('$');
}
setall()
{
	if (addr2==0) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}
setnoaddr()
{
	if (addr2)
		error('$');
}
nonzero()
{
	if (addr1<=zero || addr2>dol)
		error('$');
}
blanknl()
{
	register c;
	if((c=getchar())!=' ' && c!='\n')
		error('x');
}
