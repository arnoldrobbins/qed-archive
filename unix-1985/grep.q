" Grep for z| (possibly set by caller) in all buffers
z|=
yf'fi
	""pattern:" z|:\l
"fi
G/^[a-zA-Z]/ g/\z|/ ""\cB:" P

grepf.q         430682255   0     1     100644  164       `
" Grep for z| (possibly set by caller) in all buffers
" Prints file name as well as buffer name
z|=
yf'fi
	""pattern:" z|:\l
"fi
G/^[a-zA-Z]/ g/\z|/ ""\cB(\cf):" P
