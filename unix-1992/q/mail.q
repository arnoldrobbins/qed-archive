" bA contains mail file
" \Fa is uid
" zF is mail file
" zM is current message number
" zN is number of messages
" zX is modified if buffer has been modified
" zY is number of lines in original mail file
" zZ is number of characters in original mail file
zF:/usr/spool/mail/\Fa
bA E \zF
zZ'C
$zY#a
g/^From /zN#+1
zN#=0 yf'}
	""No mail
	Q
"}
" add an extra From line to make things easy
$a From 
zM'N zM#-1
"Loop
""? "
bA
za:\'l
bZZ a \za
bA
za=
yf'Nope
	zM#+1>\zN yf'Print
	""at end of mailbox
	zM:\zN
	y`Loop
"Nope
za=d
yf'Nope
	zx:0
	g/^From /zx#+1=\zM yto
	.,/^From /-d
	zX:modified
	zN#-1=0 yt'Quit
	zM#>\zN yf zM'N
	y`Loop
"Nope
za=h
yf'Nope
	" h command: headers
	zx:0
	g/^From .+/ ""\cz#+x	"p
	y`Loop
"Nope
bZ zx/^m .+/ bA yf'Nope
	bBZ bA
	zx:0
	g/^From /zx#+1=\zM yto 
	.,/^From /-tB0
	0a Forwarded by \Fa
	zas/m //
	>mail \za
	y`Loop
"Nope
za=p
yf'Nope
	zM#>0 yt zM:1
	yt'Print
"Nope
za=q
yf'Nope
	"Quit
	zX=modified
	yt'}
		" no changes; just quit
		zx{HOME} !touch \zx/.../readmail
		Q
	"}
	bA $d " remove that extra From
	bBZ E \zF
	zC#=\zZ yf'}
		" nobody touched it; ok to put file back
		"Write
		bA w \zF
		zx{HOME} !touch \zx/.../readmail
		Q
	"}
	""new mail arrived
	" if the file is shorter this will intentionally crap out and
	" drop you into qed
	\zY+1,$tA$
	y`Write
"Nope
za=r
yf'Nope
	zx:0
	g/^From /zx#+1=\zM yto 
	za. zas/From \([^ ]*\) .*/\1/
	bBZ
	a To: \za
	"{
		zx:\'l
		zx=.
		yt'Sendit
		zx=.e
		yt'Editit
		a \zx
		y`{
	"}
"Nope
za=R
yf'Nope
	bBZ bA
	zx:0
	g/^From /zx#+1=\zM yto 
	.,/^From /-tB0
	1" " za. zas/From \([^ ]*\) .*/\1/
	1c To: \za
    "Editit
	zx#P
	w /tmp/m\zx
	!qed /tmp/m\zx
	E /tmp/m\zx
	!rm -f /tmp/m\zx
    "Sendit
	1" " za. d zas/To: // za=
		yf'}
		""reply aborted
		y`Loop
	"}
	>mail \za
	yf	""reply saved in bB
	y`Loop
"Nope
bZ zx/^s.*/ bA yf'Nope
	zxs/s \(.*\)/\1/ yt zx{HOME} zx:\zx/mbox
	zy:0
	g/^From /zy#+1=\zM yto 
	.,/^From /- W \zx
	y`Loop
"Nope
za=x
yf'Nope
	zx{HOME} !touch \zx/.../readmail
	Q
"Nope
bZ zx/^[0-9]+/ bA yf'Nope
	" message chosen by number
	za#>\zN yf'}
		""only \zN messages
		y`Loop
	"}
	zM'a
	y'Print
"Nope
bZ zx/^-[0-9]*/ bA yf'Nope
	" back up some number of messages
	za#:-\za=0 yf za:1
	zx#:\zN-\za<1 yf'}
		""beyond beginning of mailbox
		y`Loop
	"}
	zM'x
	y'Print
"Nope
bZ zx/^!.*/ bA yf'Nope
	\za
	y`Loop
"Nope
	y`Loop

"Print
	zx:0
	g/^From /zx#+1=\zM yto 
	.,/^From /- > cat
	y`Loop

