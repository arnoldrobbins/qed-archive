" Compute length of message in a Write
" take a line of the form
"	write(msg
" where msg is in double quotes, and create
"	write(msg, msglength);
zz/".*"/
yt'fi
	""No quoted string
	yo
"fi
zzs/"\(.*\)"/\1/
k~
a \zz
s/\\././g
z~#n
d
'~ s/$/, \z~);/p
