" Split a line at a numbered blank.
" prints out line with blanks numbered, asks where to split
zL#=0 yf'fi
	""?$
	y'out
"fi
zA:\B
bAZ
b\zA
\zLmA0
zz/ /
yt'fi
	""no blanks in line
	y'abort
"fi
.t.
s/[^ 	]/./g
zI:`
h26f s/ /\cz+I/
zI-1 zI=a
yf'fi
	zI#:1
	y'Doit
"fi
1,$p
""[a-\zI]? "
zz:\'l
zzC
zzn
zC#=0 yf'fi
	""No action taken
	y'abort
"fi
zC#>1 yt'bad
zz<a
yt'bad
zz<\zI
yt'notbad
"bad
	""invalid range
	y'abort
"notbad
" convert \ zz to a number
zI#:0
ht zI#+1 zz-1 zz<a
				"Doit
1s\zI/ /\\N/
1,.p
1,.m\zA.
y'out
"abort
	1m\zA.
"out
bAZ
b\zA
zA:
zI'A zL'A zM'A
yo
"
