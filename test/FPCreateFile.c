/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test51()
{
u_int16_t bitmap;
int fork = 0;
char *name1 = "t51 file";
char *name2 = "t51 other file.txt";
char *name3 = "t51 dir";
int  dir;
int  did;
char *rodir = "t51 read only access";
int rdir = 0;
u_int16_t vol = VolID;
DSI *dsi;
int ret;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPCreateFile:test51:  Create file with errors\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (!(rdir = read_only_folder(vol, DIRDID_ROOT, rodir) ) ) {
		return;
	}

	bitmap = (1<< DIRPBIT_DID);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, rodir, 0, bitmap)) {
		failed();
		goto fin;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT , rodir);
	if (not_valid(ret, /* MAC */AFPERR_EXIST, AFPERR_BADTYPE)) {
		failed();
	}

	FAIL (ntohl(AFPERR_ACCESS) != FPCreateFile(Conn, vol,  0, did , name1)) 

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name2))

	FAIL (ntohl(AFPERR_EXIST) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name2)) 

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name2,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		failed();
		goto fin;
	}

	if (ntohl(AFPERR_EXIST) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name2)) {
		failed();
		FAIL (FPCloseFork(Conn,fork))
	}
	if (ntohl(AFPERR_BUSY) != FPCreateFile(Conn, vol,  1, DIRDID_ROOT , name2)) {
		failed();
	}
	FAIL (FPCloseFork(Conn,fork))

	FAIL ( ntohl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  1, did , "folder/essai")) 

	/* ----------- */
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name3))) {
		failed();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  1, dir , name2)) {
		failed();
		goto fin;
	}

	FAIL (FPCreateFile(Conn, vol,  1, dir , name2)) 
fin:
	if (rdir) {
		delete_folder(vol, DIRDID_ROOT, rodir);
	}
	FAIL (FPDelete(Conn, vol,  dir , name2)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name2))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name3))
	
}

/* ----------- */
void FPCreateFile_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCreateFile page 138\n");
    test51();
}

