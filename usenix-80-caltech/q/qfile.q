" Qfile: load user's qfile into buffer ~ for editing
" if z| set, (i.e. via \ zrqfile -) load default
" (typically, qfiles zap themselves after execution)
bZZ
ovr
z|=
yf'else
	<qedfile
	ok/^Is: /
	yf'else
	s///
	ok/./
	yf'else
	f \bZ
	*d
	y'fi
"else
	f /usr/qed/qfile
"fi
r
ocr ovs
""Do \cbZ when ready
"
