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

/* ------------------------- */
STATIC void test230()
{
int  dir = 0;
char *name = "t230 file";
char *ndir = "t230 dir";
u_int16_t vol = VolID;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;
int fork;

DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileDirParms:t230: set unix access privilege\n");
	if (Conn->afp_version < 30) {
		test_skipped(T_AFP3);
		return;
	}

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UNIXPRIV)) {
		test_skipped(T_UNIX_PREV);
	    return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {
		nottested();
		goto fin;
	}
	bitmap = (1<< DIRPBIT_PDINFO) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	         (1<< DIRPBIT_UNIXPR);

	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
	    failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (!(S_ISDIR(filedir.unix_priv))) {
			fprintf(stderr, "FAILED %o not a dir\n", filedir.unix_priv);
			failed_nomsg();
		}
		bitmap = (1<< DIRPBIT_UNIXPR);
		filedir.unix_priv &= ~S_IWUSR;
 		FAIL (FPSetFilDirParam(Conn, vol, dir , "", bitmap, &filedir)) 
 		FAIL (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap))

		filedir.unix_priv &= ~(S_IWUSR |S_IWGRP| S_IWOTH);
 		FAIL (FPSetFilDirParam(Conn, vol, dir , "", bitmap, &filedir)) 
 		FAIL (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap))
		
 		FAIL (!FPDelete(Conn, vol,  dir , name))

 		/* open fork read write in a read only folder */
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir, name,OPENACC_WR | OPENACC_RD);
		if (!fork) {
			failed();
		}
		else {
			FPCloseFork(Conn, fork);
		}

		fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,dir, name,OPENACC_WR | OPENACC_RD);
		if (!fork) {
			failed();
		}
		else {
			FPCloseFork(Conn, fork);
		}
	}

	bitmap = (1 <<  FILPBIT_PDINFO) | (1<< FILPBIT_PDID) | (1<< FILPBIT_FNUM) |
		(1 << DIRPBIT_UNIXPR);

	if (FPGetFileDirParams(Conn, vol, dir, name, bitmap, 0)) {
	    failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
		bitmap = (1<< DIRPBIT_UNIXPR);
		filedir.unix_priv &= ~S_IWUSR;
 		FAIL (FPSetFilDirParam(Conn, vol, dir , name, bitmap, &filedir)) 

		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir, name,OPENACC_WR | OPENACC_RD);
		if (fork) {
			failed();
			FPCloseFork(Conn, fork);
		}
		fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,dir, name,OPENACC_WR | OPENACC_RD);
		if (fork) {
			failed();
			FPCloseFork(Conn, fork);
		}
	
	}

	/* ----------------- */
	bitmap = (1<< DIRPBIT_PDINFO) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	         (1<< DIRPBIT_UNIXPR);
	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
	    failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		bitmap = (1<< DIRPBIT_UNIXPR);
		filedir.unix_priv |= S_IWUSR;
 		FAIL (FPSetFilDirParam(Conn, vol, dir , "", bitmap, &filedir)) 
	}
	
	FAIL (FPDelete(Conn, vol,  dir , name))
fin:	
	FAIL (FPDelete(Conn, vol,  dir , ""))
}

/* ----------- */
void FPSetFileDirParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileDirParms page 258\n");
    test98();
    test230();
}

