" Left justify addressed lines (default to (1,$))
" (Terminating blanks are appended to bring all lines out to same width)
zL#=\zM yf'fi
	1,$zL#r
"fi
\zL,\zMs/^[ 	]*//
\zL,\zMs/[ 	]*$//
zW:0
\zL,\zM g/^/ zI#n<\czW yt zW:\czI
zW#>35 yf zW:35
zD:                                   |
zD)\zW
\zL,\zM s/$/\zD/
" Turn spaces into periods
zD+14
\zL,\zM s/^\(\zD\) *$/\1/
zD-14
\zL,\zM s/\zD$//
zL)0 zM)0 zI)0 zD)0 zW)0
