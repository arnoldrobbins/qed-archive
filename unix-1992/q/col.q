" Buffer to columnate a buffer of file names, etc.
" takes a range of lines; defaults to 1,$
zB:\B
zL#=\zM yf 1,$zL#r
bZ Z
b\zB
\zL,\zM tZ0
" Calculate the width W of the widest line
zW#:0
g/^/ zM#n>\czW yf zW#:\czM
zW#>35 yf'fi
	1,$p
	yo
"fi
" L = width of column; C = # of columns
zN#:70 zL#:\zW+1 zN#/\zL
" now make each line have W spaces
zA:                                   |
zA)\zW
1,$s/$/\zA/
" Turn the spaces into periods
zA+14
1,$s/\(\zA.\).*/\1/
" C = number of columns.
" L = 1st line of join
" M = last line of join
$zM#a $zL#a/\zN*\zN+1
zL#=\zM yf'else
	zM#-1 zL#-\zN
	y'fi
"else
	zL#>\zM yf'else
		zL#-\zN
		y'fi
	"else
		\zL,\zM j/ /
		zM#:\zL-1
		zL#-\zN
"fi
"do
	zL#>0 yf'od
		\zL,\zM j/ /
		zL#-\zN
		zM#-\zN
	y`do
"od
1,$p
b\zB
" Clean up the variables
zA)0 zB)0 zI)0 zL)0 zM)0 zN)0 zW)0
