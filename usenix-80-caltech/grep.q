" Grep for z| (possibly set by caller) in all buffers
z|=
yf'fi
	""pattern:" z|:\l
"fi
G/^[a-zA-Z]/ g/\z|/ ""\cB:" P
