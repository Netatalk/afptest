/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test43()
{
char *name = "t43 dir";
char *name1= "t43 subdir";
u_int16_t vol = VolID;
int  dir = 0,dir1 = 0,dir2 = 0;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test43: move and rename folders\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		return;
	}
	dir1  = FPCreateDir(Conn,vol, DIRDID_ROOT , name1);
	if (!dir1) {
		nottested();
		goto fin;
	}

	dir2  = FPCreateDir(Conn,vol, dir1 , name1);
	if (!dir2) {
		nottested();
		goto fin;
	}

	if (FPMoveAndRename(Conn, vol, dir1, dir, name1, name1)) {
		failed();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  dir, name1, 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		failed();
		goto fin;
	}
	if (!FPGetFileDirParams(Conn, vol,  dir1, name1, 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		failed();
		goto fin;
	}
fin:
	FAIL (dir2 && FPDelete(Conn, vol,  dir2, "")) 
	FAIL (dir1 && FPDelete(Conn, vol,  dir1, "")) 
	FAIL (dir && FPDelete(Conn, vol,  dir, "")) 
}

/* -------------------------- */
STATIC void test77()
{
int fork;
int dir;
char *name = "t77 Move open fork other dir";
char *name1 = "t77 dir";
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPMoveAndRename:t77: Move an open fork in a different folder\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		failed();
		goto fin;
	}

	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) 
	FAIL (FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  dir , name))
	FAIL (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol, DIRDID_ROOT , name)) 
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))
}

/* ------------------------------ */
STATIC void test123()
{
int  dir;
int  dir1 = 0,dir2 = 0, dir3 = 0,dir4 = 0,dir5 = 0;
char *name = "t123 dir1";
char *name1 = "t123 dir1_1";
char *name2 = "t123 dir1_2";
char *name3 = "t123 dir1_3";
char *dest  = "t123 dest";
char *dest1  = "t123 dest_1";
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test123: Move And Rename dir with sibling\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		goto fin;
	}
	if (!(dir1 = FPCreateDir(Conn,vol, dir , name1))) {
		nottested();
		goto fin;
	}
	if (!(dir2 = FPCreateDir(Conn,vol, dir , name2))) {
		nottested();
		goto fin;
	}
	if (!(dir3 = FPCreateDir(Conn,vol, dir , name3))) {
		nottested();
		goto fin;
	}
	if (!(dir4 = FPCreateDir(Conn,vol, DIRDID_ROOT, dest))) {
		nottested();
		goto fin;
	}
	if (!(dir5 = FPCreateDir(Conn,vol, dir4 , dest1))) {
		nottested();
		goto fin;
	}

	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) 
	FAIL (ntohl(AFPERR_CANTMOVE) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "new")) 
	FAIL (FPMoveAndRename(Conn, vol, dir2, dir4, "", "")) 

fin:
	FAIL (dir5 && FPDelete(Conn, vol,  dir5 , "")) 
	FAIL (dir3 && FPDelete(Conn, vol,  dir3 , ""))
	FAIL (dir2 && FPDelete(Conn, vol,  dir2 , ""))
	FAIL (dir1 && FPDelete(Conn, vol,  dir1 , ""))
	FAIL (dir4 && FPDelete(Conn, vol,  dir4 , ""))
	FAIL (dir && FPDelete(Conn, vol,  dir , "")) 

}

/* -------------------------- */
STATIC void test138()
{
int  dir;
char *name = "t138 file";
char *name1 = "t138 dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test138: Move And Rename \n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}

	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) 
	FAIL (FPMoveAndRename(Conn, vol, dir, DIRDID_ROOT, name, "")) 

	if (FPGetFileDirParams(Conn, vol,  dir , "", 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.access[1] = 3; 
        filedir.access[2] = 3; 
        filedir.access[3] = 3; 
 		FAIL (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) 
	}
	FAIL (ntohl(AFPERR_ACCESS) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) 
    filedir.access[1] = 7; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	FAIL (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
fin:
	FAIL (FPDelete(Conn, vol,  dir , ""))
}

/* ----------- */
void FPMoveAndRename_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename page 223\n");
    test43();
    test77();
    test138();
}

