"\l
""Command: "
y'X\l
	""Invalid command. Valid commands are:
	""   help      keep      next      old       p         qed
	""   quit      reply     restart   save      send      summ[arize]
	""   forward
	y2 
"Xn
	n
	y2
"Xqed
	yo
"Xhelp
	by $zA#a#=0 yf'fi
		ovr
		e /usr/rob/q/email.help
		ovs
	"fi
	*p
	y2
"Xrestart
	bb 0,$d bc Z bd Z be Z
	bae .omail
	y2
"Xsaveall
	bb $zB#a#=0 yt'fi
		1,$me$
	"fi
	ba $zA#a#=0 yt'fi
		1,$me$
		zB#+\zA
	"fi
	zB#=0 yf'else
		""Nothing to save
		y'fi
	"else
		""\zB lines saved
	"fi
	y'Xnext
"Xsave
	bb $zB#a#=0 yf'else
		""Nothing to save
		y'fi
	"else
		1,$me$
		""Saved
	"fi
	y'Xnext
"Xkeep
	bb $zA#a#=0 yf'else
		""Nothing to keep
		y'fi
	"else
		1,$mc$
		""Kept
	"fi
" Do automatic next
"Xnext
	bb 0,$d ba $zA#a#=0 yf'fi
		""Nothing left
		y2
	"fi
	ok/^From/
	yt'fi
		""No `From' Line
		y2
	"fi
	1"
	ok//-
	zA#c#=0 yf $zA#a
	1,\zAmb0
	bb 1b+10
	$zA#a .zB#a#=\zA yt ""....
	y2
"Xquit
	bc $zA#a#=0 yt 1,$ma0
	bb $zA#a#=0 yt 1,$ma0
	ovr
	ba $zA#a#=0 yf'else
		!cat /dev/null >.omail
		y'fi
	"else
		1,$w .omail
	"fi
	be $zA#a#=0 yt'fi
		>cat >>mbox
	"fi
	q
"Xold
	ba $zA#a
	ovr
	<cat mbox
	ovs
	$zB#a#-\za 
	"" \zB lines read
	y2
"Xp
	bb
	$zA#a#=0 yf'fi
		""Nothing to print
		y2
	"fi
	1
	zA#=1 yt2
	"do
		+b+20
		.zB#a#=\zA yt2
		zZ:n
		""More (y/n) "
		zZ=\l
	yf`do
	y2
"Xreply
	bd 0,$d
	zZ:
	bb $zA#a#<1 yt'fi
		1"
		zZ/ [a-z]+/
		zZ(1
	"fi
	zA#<2 yt'else
		bb 2td0
		.g/^/ s/\_/&/g
		zA#c#<5 yt'fi
			s5/\_/%%%/
			s/%%%.*//
		"fi
		s/.*/Re: your message "&..."/
		y'fi
	"else
		bd
		""Re:"
		a Re: \l
	"fi
	""Prepare text. Type \cbx and do 'send' when ready.
	yo
"Xnote
	bd 0,$d
	zZ:
	""Re: "
	a Re: \l
	""Prepare text. Type \cbx and do `send' when ready.
	yo
"Xsend
	bd
	""Reply currently going to \zZ
	""If to be changed enter name(s) (%% to abort send command) "
	zY:\l
	zY=%%
	yf'fi
		""Send aborted
		y2
	"fi
	zY=
	yt'fi
		 zZ:\zY
	"fi
	zZ=
	yf'fi
		""Must be someone
		y`Xsend
	"fi
	""sending to \zZ
	>mail \zZ
	""Save Reply (y/n)? "
	zY:\l
	zY=y
	yf2
	bd0a Reply sent to \zZ
	bb $zA#a#=0 yt'fi
		1,$me$
	"fi
	bd 1,$me$
	y2
"Xsumm
	zB#:0
	bc $zA#a#=0 yt'fi
		zB#+1
		""Keeping
		g/^From/.,+p
	"fi
	bb $zA#a#=0 yt'fi
		zB#+1
		""Currently got
		g/^From/.,+p
	"fi
	ba $zA#a#=0 yt'fi
		zB#+1
		""Still in mail
		g/^From/.,+p
	"fi
	zB#=0 yf ""All boxes empty
	y2 
