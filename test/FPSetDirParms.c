/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test82()
{
int  dir;
char *name = "t82 test dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:test82: test set dir parameters\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
 	if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		failed();
		goto fin;
 	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
 	if (FPSetDirParms(Conn, vol, DIRDID_ROOT , "", bitmap, &filedir)) {
		failed();
		goto fin;
	}

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test84()
{
int  dir;
char *name = "t84 no delete dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:test84: test dir set no delete attribute\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 		FAIL (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
		if (ntohl(AFPERR_OLOCK) != FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
			failed();
			return;
		}
		filedir.attr = ATTRBIT_NODELETE;
 		FAIL (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test88()
{
int  dir = 0;
char *name = "t88 error setdirparams";
char *name1 = "t88 error setdirparams file";
char *name2 = "t88 error enoent file";
char *ndir = "t88 no access";
char *rodir = "t88 read only access";
int  ofs =  3 * sizeof( u_int16_t );
int pdir = 0;
int rdir = 0;
struct afp_filedir_parms filedir;
u_int16_t bitmap = (DIRPBIT_ATTR)| (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);
u_int16_t vol = VolID;
DSI *dsi;
unsigned int ret;

	if (!Conn2) 
		return;
	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:test88: test error setdirparam\n");
    
	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, ndir))) {
		return;
	}
	if (!(rdir = read_only_folder(vol, DIRDID_ROOT, rodir) ) ) {
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, ndir, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
		failed();
		goto fin;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		failed();
		goto fin;
	}
	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		failed();
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        bitmap = (1 << DIRPBIT_ATTR);
		filedir.attr = ATTRBIT_INVISIBLE| ATTRBIT_SETCLR ;
        ret = FPSetDirParms(Conn, vol, DIRDID_ROOT , rodir, bitmap, &filedir);
		if (not_valid(ret, AFPERR_ACCESS, 0)) {
			failed();
		}
 		bitmap = (1<<DIRPBIT_CDATE) |  (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE);
 		FAIL (FPSetDirParms(Conn, vol, DIRDID_ROOT , rodir, bitmap, &filedir))

 		bitmap = (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);
 		FAIL (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, DIRDID_ROOT , ndir, bitmap, &filedir)) 

 		FAIL (ntohl(AFPERR_BADTYPE) != FPSetDirParms(Conn, vol, dir , name1, bitmap, &filedir)) 
 		FAIL (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, dir , name2, bitmap, &filedir)) 
	}
fin:
	delete_folder(vol, DIRDID_ROOT, ndir);

	if (rdir) {
		delete_folder(vol, DIRDID_ROOT, rodir);
	}
	FAIL (dir && FPDelete(Conn, vol,  dir , name1))

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ------------------------- */
STATIC void test107()
{
int  dir;
int  pdir;
char *name = "t107 test dir";
char *ndir = "t107 no access";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
u_int16_t bitmap2 = (1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
u_int16_t vol2;
int uid;
int ret;

u_int16_t vol = VolID;
DSI *dsi;
DSI *dsi2;

	if (!Conn2) 
		return;
	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:t107: test dir\n");

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, ndir))) {
		return;
	}
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, DIRDID_ROOT , name))) {
		nottested();
		goto fin;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    uid = filedir.uid;

	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = uid;
        /* change owner */
		FAIL (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , name, bitmap2, &filedir)) 
	}

	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , ndir, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = uid;

		FAIL (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , ndir, bitmap2, &filedir))
	}
	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = uid;
 		ret = FPSetDirParms(Conn2, vol2, DIRDID_ROOT , "", bitmap2, &filedir);
		if (not_valid(ret, /* MAC */AFPERR_ACCESS, 0)) {
			failed();
		}
        filedir.access[0] = 0; 
        filedir.access[1] = 7; 
        filedir.access[2] = 7; 
        filedir.access[3] = 7; 
 		ret = FPSetDirParms(Conn2, vol2, DIRDID_ROOT , "", bitmap2, &filedir); 
		if (not_valid(ret, /* MAC */AFPERR_ACCESS, 0)) {
			failed();
		}
	}

	FAIL (FPDelete(Conn2, vol2,  DIRDID_ROOT , name))
	FAIL (FPCloseVol(Conn2,vol2))
fin:
	delete_folder(vol, DIRDID_ROOT, ndir);
}


/* ------------------------- */
STATIC void test189()
{
int  dir;
char *name = "t189 error setdirparams";
char *name1 = "t189 error setdirparams file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:test189: test error setdirparam\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		nottested();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
 	FAIL (ntohl(AFPERR_BADTYPE) != FPSetDirParms(Conn, vol, dir , name1, bitmap, &filedir))

	FAIL (FPDelete(Conn, vol,  dir , name1))

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test193()
{
int  dir;
char *name = "t84 no delete dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1 << DIRPBIT_ACCESS);
u_int16_t vol = VolID;
DSI *dsi;
int ret;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms:test84: user permission set\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		ret = FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir);
		if (not_valid(ret, /* MAC */AFPERR_PARAM, 0)) {
			failed();
		}

    	filedir.access[0] = 0; 
 		FAIL (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ----------- */
void FPSetDirParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetDirParms page 255\n");
    test82();
    test84();
    test88();
    test107();
    test189();
    test193();
}

