" Right justify addressed lines (default to (1,$))
zL#=\zM yf'fi
	1,$zL#r
"fi
\zL,\zMs/^[ 	]*//
\zL,\zMs/[ 	]*$//
zW:0
\zL,\zM g/^/ zC#l#<\czW yt zW:\czC
zW#>35 yf zW:35
zD:                                   |
zD)\zW
\zL,\zM s/^/\zD/
" Turn spaces into periods
zD+14
\zL,\zM s/^ *\(\zD\)$/\1/
zD-14
\zL,\zM s/^\zD//
zL:\N zM:\N zC:\N zD:\N zW:\N
