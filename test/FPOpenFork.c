/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- 
FIXME
*/
STATIC void test14()
{
u_int16_t bitmap = 0;
int fork;
u_int16_t vol = VolID;
char *name = "t14 file";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test14: get data fork open attribute same connection\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA, bitmap, DIRDID_ROOT, name, OPENACC_RD);
	if (!fork) {
		nottested();
		goto fin;
	}

	bitmap = (1 <<  FILPBIT_ATTR);
	if (FPGetForkParam(Conn, fork, bitmap)) {
		nottested();
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0)) {
	
	}
	FAIL (FPCloseFork(Conn,fork))
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0)) {
	}

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))

}

/* -------------------------- 
 FIXME need to check open attrib
*/

STATIC void test15()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork;
char *name = "t15 file";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test15: get resource fork open attribute\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	if (!fork) {
		nottested();
		goto fin;
	}
	bitmap = 1;
	if (FPGetForkParam(Conn, fork, bitmap)) {
		nottested();
	}
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
	FPCloseFork(Conn,fork);
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))

}

/* -------------------------- */
STATIC void test16()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork = 0;
int fork2 = 0;
char *name = "t16 file";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork:test16: open deny mode/ fork attrib\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		failed();
		goto fin;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	if (!fork2) {
		failed();
		goto fin;
	}

	bitmap = 1;
	FAIL (FPGetForkParam(Conn, fork, bitmap))
	FAIL (FPCloseFork(Conn,fork))
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
		
}

/* -------------------------- */
STATIC void test17()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork = 0;
int fork2 = 0;
int fork3 = 0;
char *name = "t17 file";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork:test17: open deny mode/ fork attrib\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name, 0);
	if (!fork) {
		failed();
	}	
	FAIL (fork && FPCloseFork(Conn,fork))

	/* -------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,  OPENACC_WR );
	if (!fork) {
		failed();
	}

	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (fork3) {
		failed();
	}

	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (fork3 && FPCloseFork(Conn,fork3))
	fork3 = 0;
	/* -------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name, 
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (!fork) {
		failed();
		goto fin;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name, OPENACC_RD);
	if (!fork2) {
		failed();
		goto fin;
	}
	bitmap = 1;
	FAIL (FPGetForkParam(Conn, fork, bitmap))
	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (fork3) {
		failed();
	}
fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork3 && FPCloseFork(Conn,fork3))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* -------------------------- */
STATIC void test18()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork;
int fork2 = 0;
int fork3 = 0;
char *name = "t18 file";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork:test18: open deny mode/ fork attrib\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (!fork) {
		failed();
		goto fin;
	}
	/* success */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	bitmap = 1;
	if (!fork2) {
		failed();
		goto fin;
	}

	FAIL (FPGetForkParam(Conn, fork, bitmap)) 
	FAIL (FPCloseFork(Conn,fork))
	
	/* success */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA, bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork3) {
		failed();
	}
fin:
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork3 && FPCloseFork(Conn,fork3))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* -------------------------- */
STATIC void test19()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork;
int fork2 = 0;
int fork3 = 0;
char *name = "t19 file";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork:test19: open deny mode/ fork attrib\n");


	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	/* success */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_DWR );
	if (!fork) {
		failed();
		goto fin;
	}

	/* success */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_DWR );
	if (!fork2) {
		failed();
		goto fin;
	}

	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_WR);
	if (fork3) {
		failed();
		goto fin;
	}

	bitmap = 1;
	/* success */
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}

	/* success */
	if (FPCloseFork(Conn,fork)) {
		fork = 0;
		failed();
		goto fin;
	}
	fork = 0;
	
	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_WR);
	if (fork3) {
		failed();
	}
fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork3 && FPCloseFork(Conn,fork3))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ------------------------- */
STATIC void test20()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int fork = 0;
int fork2 = 0;
char *name = "t20 file";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test20: open file read only and read write\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD );

    if (!fork) {
		failed();
		goto fin;
    }        
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_WR );

    if (!fork2) {
		failed();
		goto fin;
    }        

	FAIL (FPWrite(Conn, fork2, 0, 10, Data, 0 /*0x80 */))
fin:
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* ----------------------- */
STATIC void test39()
{
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
char *ndir = "t39 dir";
int  dir;
char *nf1  = "t39 file.txt";
char *nf2  = "t39 file2";
char *name = "t39 dir/t39 file2";
char *name1 = "t39 dir//t39 file.txt";
char *name2 = "t39 dir///t39 file.txt";
int  fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test39: cname path folder + filename\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nf1)) {
		nottested();
		goto fin;
	}
	if (FPCreateFile(Conn, vol,  0, dir , nf2)) {
		nottested();
		goto fin;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name, OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		
	FAIL (FPCloseFork(Conn,fork))

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name1, OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		
	FAIL (FPCloseFork(Conn,fork))

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name2, OPENACC_RD);
	if (fork) {
		failed();
		FAIL (FPCloseFork(Conn,fork))
	}
fin:	
	FAIL (FPDelete(Conn, vol,  dir , nf2))
	FAIL (FPDelete(Conn, vol,  dir , ""))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , nf1))
}

/* ------------------------- */
STATIC void test48()
{
u_int16_t bitmap = 0;
int fork;
char *ndir = "t48 dir";
int  dir;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test48: open fork a folder\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, ndir, OPENACC_RD);

	if (fork) {
		failed();
		FAIL (FPCloseFork(Conn,fork))
	}
    else if (ntohl(AFPERR_BADTYPE) != dsi->header.dsi_code) {
		failed();
	}

	FAIL (FPDelete(Conn, vol,  dir , ""))
}

/* ---------------------------- */
static void test_denymode(char *name, int type)
{
int fork;
int fork1;
int fork2;
int fork3;
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
DSI *dsi;
DSI *dsi2;

	dsi = &Conn->dsi;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork) {
		failed();
	}

	fork2 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork2) {
		failed();
	}
	fork3 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork3) {
		failed();
	}
	
	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (fork1 || dsi->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
		failed();
		if (fork1) {
			FAIL (FPCloseFork(Conn,fork1))
		}
	}
	if (fork3) {
		FAIL (FPCloseFork(Conn,fork3))
	}
	
    if (Conn2) {
		u_int16_t vol2;

		dsi2 = &Conn2->dsi;
		vol2  = FPOpenVol(Conn2, Vol);
		if (vol2 == 0xffff) {
			nottested();
		}

		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
		if (fork1 || dsi2->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
			failed();
			if (fork1) FPCloseFork(Conn2,fork1);
		}
		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
		if (!fork1) {
			failed();
		}
		else {
			FAIL (FPCloseFork(Conn2,fork1))
		}
		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
		if (fork1 || dsi2->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
			failed();
			if (fork1) FPCloseFork(Conn2,fork1);
		}
		FAIL (FPCloseVol(Conn2,vol2))
	}
	FAIL (FPCloseFork(Conn,fork))
	if (fork2) FPCloseFork(Conn,fork2);
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* -------------------------- */
STATIC void test81()
{
char *name = "t81 Denymode RF 2users";

    if (!Conn2) {
    	return;
    }
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test81: Deny mode 2 users RF\n");
		test_denymode(name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test81: Deny mode 2 users DF\n");
	name = "t81 Denymode DF 2users";
	test_denymode(name, OPENFORK_DATA);
}

/* ------------------------- */
STATIC void test116()
{
char *name = "t116 no write file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
int fork;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;


    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:t116: test file's no-write attribute\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name,bitmap, 0 )) {
		nottested();
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
			
	filedir.attr = ATTRBIT_NOWRITE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		failed();
		goto end;
 	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);

	if (fork || dsi->header.dsi_code != ntohl(AFPERR_OLOCK)) {
		failed();
	}
	if (fork) {
		FPCloseFork(Conn,fork);
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_RD);

	if (!fork) {
		failed();
	}
	else {
		FAIL (FPCloseFork(Conn,fork))
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0xffff ,DIRDID_ROOT, name, OPENACC_RD);

	if (fork || dsi->header.dsi_code != ntohl( AFPERR_BITMAP)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		FAIL (FPCloseFork(Conn,fork))
	}
end:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* -------------------------- */
STATIC void test145()
{
u_int16_t bitmap = 0;
int fork;
char *name = "t145 file.txt";
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork:test145: open RF mode 0\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, 0);
	if (!fork) {
		failed();
	}	
	if (FPCloseFork(Conn,fork)) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* ------------------------- */
STATIC void test151()
{
u_int16_t bitmap = 0;
char *name1  = "t151 file";
int fork;
int i;
int maxf;
int ok = 0;
static int forkt[2049];
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test151: too many open files\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)){ 
		nottested();
		return;
	}

	maxf = 0;
	for (i = 0; i < 2048; i++) {
		forkt[i] = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1 ,OPENACC_RD|OPENACC_DWR);
		if (!forkt[i]) {
			maxf = i;
			ok = 1;
			if (dsi->header.dsi_code != ntohl(AFPERR_NFILE)) {
				failed();
			}
			break;
		}
	}
	if (!ok) {
		nottested();
		maxf = 2048;
	}
	for (i = 0; i < maxf; i++) {
		if (forkt[i] && FPCloseFork(Conn, forkt[i])) {
			failed();
		}
	}
	if (!ok) {
		goto end;
	}
	for (i = 0; i < maxf; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1 ,OPENACC_RD|OPENACC_WR);
		if (!fork) {
			failed();
			break;
		}
		if (fork && FPCloseFork(Conn, fork)) {
			failed();
			break;
		}
	}
end:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name1))
}

/* -------------------------- */
STATIC void test190()
{
int fork = 0;
int fork2 = 0;
char *name = "t190 file";
char *name1 = "t190 new name";
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPOpenFork::test190: deny mode after file moved\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		nottested();
		goto fin;
	}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		failed();
		goto fin;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA, 0 ,DIRDID_ROOT, name1, OPENACC_WR);
	if (fork2) {
		failed();
		goto fin;
	}

    if (ntohl(AFPERR_DENYCONF) != dsi->header.dsi_code) {
		failed();
		goto fin;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS, 0 ,DIRDID_ROOT, name1, OPENACC_WR);
	if (!fork2) {
		failed();
		goto fin;
	}
fin:
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) 
}

/* ----------- */
void FPOpenFork_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork page 230\n");
    
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test39();
    test48();
    test81();
    test116();
    test145();
    test151();
    test190();
}

