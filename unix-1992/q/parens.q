" Parens - show parenthesis level in addressed lines
zL#=0 yf'fi
	""?$
	y'Done
"fi
zA:\B
bAZ
b\zA
\zL,\zM tA0
g/^/.t. s/^/@@/
g/^@@/ s/^@@// s/[^()	]/./g
zI#:0
"Nextline
	$zD#a=0 yt'Done
	2"
	"loop
		zB/[()]/ yf'endloop
		zB=(
		yf'else
		"if lpar
			zI#+1%10
			s//\zI/
			y'endif
		"else (rpar)
			s//\zI/
			zI#+9%10
		"endif
		y`loop
	"endloop
	1,2p
	1,2d
	y`Nextline
"Done
zA)0 zB)0 zI)0 zL)0 zM)0
"
