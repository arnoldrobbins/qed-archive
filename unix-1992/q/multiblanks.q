" Multiblanks - convert multiple blank lines to single blank lines
" Address defaults to 1,$
zL#=\zM yf'fi
	1,$zL#r
"fi
" By adding blank at end, get all trailing blank lines deleted!
\zMa 
" Precompile the pattern for efficiency
/^$/"
\zL,\z#+M g// a \cN hf d zP//
zP)0 zL)0 zM)0
