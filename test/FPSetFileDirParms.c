/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test98()
{
int  dir;
char *name = "t98 error setfildirparams";
char *name1 = "t98 error setfildirparams file";
char *ndir = "t98 no access";
char *rodir = "t98 read only access";
int  ofs =  3 * sizeof( u_int16_t );
int pdir = 0;
int rdir = 0;
int ret;
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) 
					| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS)
					| (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileDirParms:t98: test error setfildirparam\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, ndir))) {
		return;
	}
	if (!(rdir = read_only_folder(vol, DIRDID_ROOT, rodir) ) ) {
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, ndir, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
		(1<<DIRPBIT_UID) | (1 << DIRPBIT_GID)| (1 << DIRPBIT_ACCESS))) {
		failed();
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, rodir, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
		(1<<DIRPBIT_UID) | (1 << DIRPBIT_GID)| (1 << DIRPBIT_ACCESS))) {
		failed();
		goto fin;
	}
	
	FAIL (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name)))
	FAIL (FPCreateFile(Conn, vol,  0, dir , name1))

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		bitmap = (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE);
 		FAIL (FPSetFilDirParam(Conn, vol, DIRDID_ROOT , rodir, bitmap, &filedir)) 
 		ret = FPSetFilDirParam(Conn, vol, DIRDID_ROOT , ndir, bitmap, &filedir);
		if (not_valid(ret, /* MAC */0, AFPERR_ACCESS)) {
			failed();
		}
 		FAIL (FPSetFilDirParam(Conn, vol, dir , name1, bitmap, &filedir)) 
 		FAIL (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT, name1, bitmap, &filedir))
 		FAIL (FPSetFilDirParam(Conn, vol, DIRDID_ROOT, name, bitmap, &filedir)) 
	}

fin:
	delete_folder(vol, DIRDID_ROOT, ndir);
	if (rdir) {
		delete_folder(vol, DIRDID_ROOT, rodir);
	}
	FAIL (FPDelete(Conn, vol,  dir , name1))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ----------- */
void FPSetFileDirParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileDirParms page 258\n");
    test98();
}

