" Buffer to columnate a buffer of file names, etc.
" takes a range of lines; defaults to 1,$
zB:\B
zL#=\zM yf 1,$zL#r
bZ Z
b\zB
\zL,\zM tZ0
" Calculate the width W of the widest line
zW#:0
g/^/ zM#l zM#>\czW yf zW#:\czM
zW#>35 yf'fi
	*p
	yo
"fi
" L = width of column; C = # of columns
zC#:70 zL#:\zW zL#+1 zC#/\zL
" now make each line have W spaces
zA:                                   |
zA)\zW
*s/$/\zA/
" Turn the spaces into periods
zA+14
*s/\(\zA.\).*/\1/
" C = number of columns.
" L = 1st line of join
" M = last line of join
$zL#a $zM#a zL#/\zC zL#*\zC zL#+1
zL#=\zM yf'else
	zM#-1 zL#-\zC
	y'fi
"else
	zL#>\zM yf'else
		zL#-\zC
		y'fi
	"else
		\zL,\zM j/ /
		zM#:\zL
		zM#-1
		zL#-\zC
"fi
"do
	zL#>0 yf'od
		\zL,\zM j/ /
		zL#-\zC
		zM#-\zC
	y`do
"od
*p
b\zB
" Clean up the variables
zA:\N zB:\N zC:\N zI:\N zL:\N zM:\N zW:
