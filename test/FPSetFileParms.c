/* ----------------------------------------------
*/
#include "specs.h"

STATIC void test83()
{
char *name = "t83 test file setfilparam";
char *name1 = "t83 test enoent file";
char *ndir = "t83 dir";
int  dir;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t83: test set file setfilparam\n");

	if (!(dir =FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		return;
	}

	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
 		FAIL (htonl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) 
		FAIL (htonl(AFPERR_BADTYPE) != FPSetFileParams(Conn, vol, DIRDID_ROOT , ndir, bitmap, &filedir))
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
fin:
	FAIL (FPDelete(Conn, vol,  dir , "")) 
}

/* ------------------------- */
STATIC void test96()
{
char *name = "t96 invisible file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileParms:t96: test file's invisible bit\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
	sleep(4);
	
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name,bitmap, 0 )) {
		failed();
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
	fprintf(stderr,"Modif date file %x\n", filedir.mdate);
	sleep(5);
			
	filedir.attr = ATTRBIT_INVISIBLE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 

	bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0 )) {
		failed();
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
	fprintf(stderr,"Modif date file %x\n", filedir.mdate);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
end:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test118()
{
char *name = "t118 no delete file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileParms:t118: test file no delete attribute\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0 )) {
		nottested();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
		FAIL (ntohl(AFPERR_OLOCK) != FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
		filedir.attr = ATTRBIT_NODELETE;
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) 
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ------------------------- */
STATIC void test122()
{
char *name = "t122 setfilparam open fork";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int fork;
int fork1;
int ret;
int type = OPENFORK_RSCS;

u_int16_t bitmap =  (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t122: test setfilparam open fork\n");

	memset(&filedir, 0, sizeof(filedir));
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, type , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		/* wrong attrib (open fork set ) */
 		ret = FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir);
		if (not_valid(ret, /* MAC */AFPERR_PARAM, 0)) {
			failed();
		}
		bitmap =  (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir))

 		FAIL (htonl(AFPERR_BITMAP) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name, 0xffff, &filedir))
	}
	fork1 = FPOpenFork(Conn, vol, type , 0 ,DIRDID_ROOT, name, OPENACC_RD);
	if (fork1) {
		FAIL (FPCloseFork(Conn,fork1))
		failed();
	}

	FPCloseFork(Conn,fork);
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ----------- */
void FPSetFileParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetFileParms page 262\n");
    test83();
    test96();
    test118();
    test122();
}

