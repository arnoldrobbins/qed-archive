" Split - split a line at a numbered blank
" prints out line with blanks numbered, asks where to split
zL#=0 yf'fi
	""?a
	y'out
"fi
zA:\B
bAZ
b\zA
\zLmA0
.t.
s/[^ 	]/./g
zC#:0
hf s/ /#\cz#+C!/
s/#[1-9]!/0&/g
s/0#/#0/g
.t.
-s/#//g
s/.!//g
+s/#.//g
s/!//g
*p
""? "
zz:\l
zC#:\zz		" don't put \ l directly into C because want to set numerically
		" (user may type junk after number; causes error)
zC#=0 yf'fi
	1m\zA
	y'out
"fi
1s\zC/ /\\N/
1,.p
1,.m\zA.
bAZ
b\zA
"out
zA:
zC'A zL'A zM'A
yo
"
