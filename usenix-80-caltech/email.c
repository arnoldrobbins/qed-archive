/*% cc -O %
 *	startup for qed email system
 */
char	buf[512];
main(){
	register int omfid;
	register int mfid;
	register int bytes;
	/* chdir to home directory */
	gethdir(getuid(),buf);
	check(chdir(buf),buf);
	/* open or creat .mail */
	if((mfid = open(".mail",1)) == -1){
		mfid = check(creat(".mail",0600),".mail");
	}
	/* seek to end of .mail */
	seek(mfid,0,2);
	/* open for reading .mail & read appending to .omail */
	if((omfid = open(".omail",0)) != -1){
		while(bytes = read(omfid,buf,512))
			write(mfid,buf,bytes);
		close(omfid);
		unlink(".omail");
	}
	close(mfid);
	link(".mail",".omail");
	unlink(".mail");
	creat(".mail",0600);
	execl("/bin/qed","qed","-","-x","/usr/qed/q/email.qed",".omail");
}
check(cbit,mess) int cbit; char *mess;
{
	if(cbit >= 0) return(cbit);
	perror(mess);
	exit();
}
