/*% cc -c -O %
 */
#include "vars.h"
extern noaddr;
setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error('$');
}
setall()
{
	if (noaddr) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}
setnoaddr()
{
	extern noaddr;
	if (noaddr == FALSE)
		error('$');
}
nonzero()
{
	if (addr1<=zero || addr2>dol)
		error('$');
}
