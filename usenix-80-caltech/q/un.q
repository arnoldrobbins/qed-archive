" un.q -- replace addressed lines of current buffer by result
"	  of passing them through pipeline
"	  Looks in z| for pipeline; if empty, prompts & reads from terminal
"	  Called as addr1, addr2 \ zrun; defaults to (1,$).
z|=
yf'fi
	""<> "
	z|:\l
"fi
zL#=\zM yf zL#:1 $zM#a
ovr
" generate random number for tempfile name
zN#? zN^/tmp/qed.
\zL,\zM > \z| > \zN
zT#t	" zT gets return status
\zMr \zN
!rm \zN
ovs
zT#=0 yt'else
	""Invalid status return - lines not deleted
	y'fi
"else
	\zL,\zMd
"fi
zL:\NzM:\NzN:\NzT:
""!\N
