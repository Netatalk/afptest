/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test13()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
char *name = "t13 file";
int ret = 0;
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test13: delete open file same connection\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		nottested();
		goto fin;
	}

	FAIL (FPWrite(Conn, fork, 0, 2000, Data, 0))

	ret = FPDelete(Conn, vol,  DIRDID_ROOT , name);

	if (!ret) {
		failed();
	}
	FAIL (FPCloseFork(Conn,fork))

fin:
	FAIL (ret && FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test27()
{
char *name  = "t27 file";
char *name2 = "t27 dir";
u_int16_t vol = VolID;
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete:test27: delete not empty dir\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name2);
	if (!dir) {
		nottested();
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {
		nottested();
	}

	FAIL (htonl( AFPERR_DIRNEMPT) !=FPDelete(Conn, vol,  DIRDID_ROOT , name2))

	FAIL (FPDelete(Conn, vol,  dir , name));
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name2))
}

/* -------------------------- */
STATIC  void test74()
{
int fork;
u_int16_t bitmap = 0;
u_int16_t vol2;
char *name = "t74 Delete File 2 users";
int type = OPENFORK_DATA;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);
u_int16_t vol = VolID;
DSI *dsi2;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete:test74: Delete File 2 users\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		


	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}

	FAIL (FPSetForkParam(Conn, fork, len , 50))

	FAIL (htonl(AFPERR_BUSY)!= FPDelete(Conn2, vol2,  DIRDID_ROOT, name))
	FAIL (FPCloseFork(Conn,fork))
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
	FAIL (FPCloseVol(Conn2,vol2))
}

/* ------------------------- */
STATIC void test90()
{
int  dir = 0;
char *name = "t90 dir";
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete:test90: delete a dir without access\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (!(dir = no_access_folder(vol, DIRDID_ROOT, name))) {
		return;
	}
	FAIL (ntohl(AFPERR_ACCESS) != FPDelete(Conn, vol,  DIRDID_ROOT , name))
	delete_folder(vol, DIRDID_ROOT, name);
}

/* -------------------------- */
STATIC void test172()
{
u_int16_t bitmap = 0;
char *tname = "test172";
char *name = "test172.txt";
char *name1 = "newtest172.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int tdir;
int fork;
int dir;
unsigned int ret;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete:test172: did error did=<deleted> name=test172 name\n");

	memset(&filedir, 0, sizeof(filedir));
	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, tname);
	if (!tdir) {
		nottested();
		return;
	}
	if (FPDelete(Conn, vol,  tdir , "")) { 
		nottested();
		return;
	}

    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , tdir, tname,OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		failed();
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	FAIL (htonl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  0, tdir, tname)) 
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		FAIL (htonl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, tdir, tname, bitmap, &filedir)) 
	}
	/* -------------------- */
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	
	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, tdir , vol, DIRDID_ROOT, tname, name1)) {
		failed();
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	FAIL (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, tdir, name, tname)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
	
	/* -------------------- */
	if ((get_vol_attrib(vol) & VOLPBIT_ATTR_FILEID) ) {
		ret = FPCreateID(Conn,vol, tdir, tname);
		if (htonl(AFPERR_NOOBJ) != ret && htonl(AFPERR_PARAM) != ret ) {
			failed();
		}
	}
	/* -------------------- */
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) 

	ret = FPExchangeFile(Conn, vol, tdir,dir, tname, name1);
	if (ntohl(AFPERR_NOOBJ) != ret) {
		if (Quirk && ret == htonl(AFPERR_PARAM)) 
			fprintf(stderr,"\tFAILED (IGNORED) not always the same error code!\n");
		else {
			failed();
		}
	}
	FAIL (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, tdir, name, tname)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	FAIL (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, tdir, tname, bitmap, &filedir)) 
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		failed();
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, tdir, tname);
    if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		failed();
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, tdir, tname, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		failed();
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		FAIL (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, tdir, tname, bitmap, &filedir)) 
 	}
 	/* ---------------- */
	FAIL (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, tdir, tname, name1)) 
 	/* ---------------- */
	FAIL (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  tdir, tname)) 
 	/* ---------------- */
	FAIL (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, tdir, DIRDID_ROOT, tname, name1)) 

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	FAIL (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, tdir, name, tname)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  tdir, tname, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		failed();
	}
	/* ---- desktop.c ---- */
	FAIL (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol, tdir, tname, "Comment")) 
	FAIL (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol, tdir, tname)) 
	FAIL (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol, tdir, tname)) 
	
}

/* -------------------------- */
STATIC void test196()
{
char *name = "test196";
char *name1 = "test196/test196";
u_int16_t vol = VolID;
u_int16_t vol2;
int tdir;
int tdir1 = 0;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap = (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);

	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete:test196: delete a folder in a deleted folder\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (Exclude) {
		fprintf(stderr, "\tFAILED (not run kill 1.6.x servers)\n");
		failed_nomsg(); 
		return;
	}
	
	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, name);
	if (!tdir) {
		nottested();
		return;
	}

	tdir1  = FPCreateDir(Conn,vol,tdir, name);
	if (!tdir1) {
		nottested();
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol,  tdir1 , "", 0, bitmap)) {
		failed();
		goto fin;
	}
	bitmap = (1 << DIRPBIT_ACCESS);
	FAIL (FPGetFileDirParams(Conn, vol,  tdir , "", 0, bitmap))
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    filedir.access[1] = 7; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	FAIL ( FPSetDirParms(Conn, vol, tdir , "", bitmap, &filedir))

	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		goto fin;
	}
	if (FPDelete(Conn2, vol2,  DIRDID_ROOT , name1)) { 
		nottested();
		FPDelete(Conn, vol, tdir1 , "");
		tdir1 = 0;
	}
	if (FPDelete(Conn2, vol2,  DIRDID_ROOT , name)) { 
		nottested();
		FPDelete(Conn, vol, tdir , "");
		tdir1 = 0;
	}	
	FPCloseVol(Conn2,vol2);

	FAIL (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol, tdir , ""))
	FAIL (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol, tdir1 , ""))
	tdir = tdir1 = 0;

fin:
	FAIL (tdir && FPDelete(Conn, vol, tdir , ""))
	FAIL (tdir1 && FPDelete(Conn, vol, tdir1 , ""))
}

/* ----------- */
void FPDelete_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete page 143\n");
    test13();
	test27();
	test74();
	test172();
	test196();
}

