/*% cc -c -O %
 */
#include "vars.h"
#define	strfree string[NSTRING].str
char *strstart;

length(s)
	register char *s;
{
	register char *t;
	if((t=s)==0)
		return(0);
	do;while(*t++);
	return(t-s-1);
}
startstring(){
	strstart=strfree;
}
addstring(c){
	if(strfree==strchars+NSTRCHARS)
		strcompact();
	*strfree++ = c;
}
dropstring(){
	--strfree;
}
cpstr(a, b)
	register char *a, *b;
{
	do;while (*b++ = *a++);
}
shiftstring(up){	/* A highly machine-dependent routine */
	register struct string *sp;
	for(sp=string; sp<=string+NSTRING; sp++)
		if(up)
			sp->str += (int)strarea;
		else
			sp->str -= (int)strarea;
}
clearstring(z){
	string[z].len = 0;
	string[z].str = nullstr;
}
copystring(s)
	register char *s;
{
	while(*s)
		addstring(*s++);
}
eqstr(a, b)
	register char *a, *b;
{
	while(*a)
		if(*a++ != *b++)
			return(FALSE);
	return(*b=='\0');
}
/*
 * dupstring duplicates a string.
 * Because we may strcompact(), we do it first if necessary.
 */
dupstring(z)
{
	if(strfree+string[z].len > strchars+NSTRCHARS)
		strcompact();	/* if insufficient, will get error when we copystring() */
	copystring(string[z].str);
}
setstring(n){
	addstring('\0');
	if((string[n].len = length(strstart)) == 0)
		string[n].str = nullstr;
	else
		string[n].str = strstart;
	if(strfree >= strchars + NSTRCHARS)
		strcompact();
}
strcompact(){
	register struct string *cursor;
	struct string *thisstr;
	register char *s, *t;
	lock++;
	s=strchars;
	for(;;){
		t=strchars+NSTRCHARS;
		for(cursor=string;cursor!=string+NSTRING;cursor++)
			if(s<=cursor->str && cursor->str<t){
				t = cursor->str;
				thisstr = cursor;
			}
		if(t==strchars+NSTRCHARS)
			break;
		thisstr->str=s;
		do;while(*s++ = *t++);
	}
	t=strstart;
	strstart=s;
	while(t!=strfree)
		*s++ = *t++;
	strfree=s;
	if(s==strchars+NSTRCHARS){
		strfree=strstart;
		error('Z');
	}
	unlock();
}
