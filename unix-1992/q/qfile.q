" Qfile: load user's qfile into buffer ~ for editing
" if z| set, (i.e. via \ zrqfile -) load default
" (typically, qfiles zap themselves after execution)
bZZ
ovr
z|=
yf'else
	zF{QEDFILE}
	zF=
	yt'else
		f \zF
		y'fi
"else
	f /usr/rob/qed/q/qfile
"fi
r
ocr ovs
zF:
""Do \cbZ when ready
"
