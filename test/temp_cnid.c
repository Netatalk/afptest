/* ------------------------- */
void test29()
{
char *name   = "foo";
char *name1  = "fo1";
struct stat st, st1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test29: test out of date cnid db, reused inode\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		FPCloseVol(Conn,vol);
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/.AppleDouble/");strcat(temp, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	strcpy(temp, Path);
	strcat(temp,"/");
	strcat(temp, name1);
	if (stat(temp, &st1) || st.st_ino != st1.st_ino) {
		fprintf(stderr,"Inode for %s: %d %d\n", Path, st.st_ino, st1.st_ino);
		fprintf(stderr,"\tNOT TESTED\n");
		sleep(2);
		FPDelete(Conn, vol,  DIRDID_ROOT, name1);
		return;
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		FPCloseVol(Conn,vol);
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name1, 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
	         0
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test30()
{
char *name   = "foo";
char *name1  = "foo1";
struct stat st, st1;
int f;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test30: test out of date cnid db, != inodes\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	strcpy(temp, Path);
	strcat(temp,"/");
	strcat(temp, name);
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}
    if ((f = open(temp, O_RDONLY)) < 0) {
		fprintf(stderr,"\tFAILED\n");
    }    

	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (stat(temp, &st1)) {
		fprintf(stderr,"\tNOT TESTED stat %s %s\n", temp, strerror(errno));
	}
	else if (st.st_ino == st1.st_ino) {
		fprintf(stderr,"\tNOT TESTED\n");
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	close(f);
}
