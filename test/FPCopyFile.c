/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test71()
{
int fork;
int dir;
int dir1;
u_int16_t bitmap = 0;
char *name  = "t71 Copy file";
char *name1 = "t71 new file name";
char *name2 = "t71 dir";
char *ndir = "t71 no access";
int pdir = 0;

u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCopyFile:test71: Copy file\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, ndir))) {
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		nottested();
		return;
	}

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) 

	/* sdid bad */
	FAIL (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, name, name1))

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	/* cname unchdirable */
	FAIL (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, ndir, name1)) 
	/* second time once bar is in the cache */
	FAIL (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, ndir, name1)) 

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		failed();
		goto fin;
	}

	FAIL (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) 

	/* source is a dir */
	FAIL (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name2, name1)) 

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (fork) {
	
		FAIL (ntohl(AFPERR_DENYCONF) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1))
		FAIL (FPCloseFork(Conn,fork))
	}	
	else {
		failed();
	}
	/* dvol bad */
	FAIL (ntohl(AFPERR_PARAM) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol +1, dir, name, name1)) 

	/* ddid bad */
	FAIL (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT , vol, dir,  name, name1)) 

	/* ok */
	FAIL (FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name1))
fin:	
	delete_folder(vol, DIRDID_ROOT, ndir);
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name2))
}

/* ------------------------- */
STATIC void test158()
{
char *name  = "t158 old file name";
char *name1 = "t158 new file name";
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCopyFile:test158: copyFile dest exist\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		nottested();
	}

	/* sdid bad */
	FAIL (ntohl(AFPERR_EXIST) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))

}

/* ----------- */
void FPCopyFile_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCopyFile page 131\n");
    test71();
	test158();
}

