" take a line of the form
"	write(msg
" where msg is in double quotes, and create
"	write(msg, msglength);
z~/".*"/
k~
a \z~
s/^"// s/"$//
s/\\././g
z~#l
d
'~ s/$/, \z~);/p
