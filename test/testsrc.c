/* ----------------------------------------------
*/

#include "afpclient.h"
#include "test.h"
extern int Noadouble;

u_int16_t vol;
DSI *dsi, *dsi2; 

#include <signal.h>  

static void alarm_handler()
{
	fprintf(stderr,"\tFAILED\n");
	exit(1);
}

/* ----------------------- */
void test1(void)
{
	sleep(2);
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test1: GetSrvInfo\n");

	if (FPGetSrvrInfo(Conn))
		fprintf(stderr,"\tFAILED\n");
	
	if (FPGetSrvrParms(Conn))
		fprintf(stderr,"\tFAILED\n");
}

/* ------------------------- */
void test3(void)
{
static char *vers = "AFP2.2";
static char *uam = "No User Authent";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test3: Guest login\n");

    if (FPopenLogin(Conn, vers, uam, "", "")) {
		fprintf(stderr,"\tFAILED\n"); 
    }
    else {
		FPLogOut(Conn);
    }   
}

/* ------------------------- */
void test5()
{
u_int16_t bitmap = 0;
int fork, fork1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test5: data fork\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}

	if (FPWrite(Conn, fork1, 8000, 2048, Data, 0 /*0x80 */)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}
	FPFlushFork(Conn, fork1);

	if (FPRead(Conn, fork, 0, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

	if (FPWrite(Conn, fork1, 0, 100, Data, 0x80 )) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}
	FPFlush(Conn, vol);
	FPCloseFork(Conn,fork1);
	FPCloseFork(Conn,fork);
	/* ----------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

	if (ntohl(AFPERR_ACCESS) != FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_DFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 10, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}
	
	if (FPWrite(Conn, fork1, 100, 20, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

	if (FPRead(Conn, fork, 110, 10, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin2;
	}

fin2:
	FPCloseFork(Conn,fork1);

fin1:
	if (FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

fin:
	if (FPDelete(Conn, vol,  DIRDID_ROOT , "toto")) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test6()
{
int  dir;
char *name = "Newé dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test6: create dir\n");

	if (!FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test11()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test11: delete attrib protected dir\n");

	if (!FPDelete(Conn, vol,  DIRDID_ROOT , "Network Trash Folder")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test12()
{
u_int16_t bitmap = 0;
int fork = 0;
int fork1;
int fork2;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test12: deny mode\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_RD | OPENACC_DWR);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_WR | OPENACC_DWR);
	if (fork2) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork2);
		goto fin;
	}
	
	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_RD | OPENACC_DWR);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}
	FPCloseFork(Conn,fork1);

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_WR | OPENACC_DWR);
	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_RD | OPENACC_DRD);
	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		goto fin;
	}

	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_RD | OPENACC_DRD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_RD );
	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, 
				"Network Trash Folder/Trash Can Usage Map",OPENACC_WR );
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}
	FPCloseFork(Conn,fork1);
fin:
	FPCloseFork(Conn,fork);
}

/* ------------------------- */
void test13()
{
u_int16_t bitmap = 0;
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test13: delete open file same connection\n");
//	FPDelete(Conn, vol,  DIRDID_ROOT , "toto");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPWrite(Conn, fork, 0, 2000, Data, 0 /*0x80 */);
#if 0
		FPWrite(Conn, fork, 0, 9000, Data, 0 /*0x80 */);
		FPFlushFork(Conn, fork);
				
//		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
#endif
//	FPRead(Conn, fork, 0, 10000, Data);
	if (!FPDelete(Conn, vol,  DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	FPCloseFork(Conn,fork);

	if (FPDelete(Conn, vol,  DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* -------------------------- */
void test14()
{
u_int16_t bitmap = 0;
int fork;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test14: get data fork open attrib\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA, bitmap, DIRDID_ROOT, name, OPENACC_RD);
	bitmap = (1 <<  FILPBIT_ATTR);
	FPGetForkParam(Conn, fork, bitmap);
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
	FPCloseFork(Conn,fork);
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
}

/* -------------------------- */
void test15()
{
u_int16_t bitmap = 0;
int fork;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test15: get ressource fork open attrib\n");
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	bitmap = 1;
	FPGetForkParam(Conn, fork, bitmap);
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
	FPCloseFork(Conn,fork);
	FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0);
}

/* -------------------------- */
void test16()
{
u_int16_t bitmap = 0;
int fork;
int fork2;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test16: open deny mode/ fork attrib\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	bitmap = 1;
	FPGetForkParam(Conn, fork, bitmap);
	FPCloseFork(Conn,fork);
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (fork ) FPCloseFork(Conn,fork);
	if (fork2) FPCloseFork(Conn,fork2);
		
}

/* -------------------------- */
void test17()
{
u_int16_t bitmap = 0;
int fork;
int fork2;
int fork3;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test17: open deny mode/ fork attrib\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name, 0);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}	
	FPCloseFork(Conn,fork);

	/* -------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,  OPENACC_WR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (fork3) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (fork ) FPCloseFork(Conn,fork);
	if (fork3) FPCloseFork(Conn,fork3);

	/* -------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name, 
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS  , bitmap ,DIRDID_ROOT, name, OPENACC_RD);
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	bitmap = 1;
	FPGetForkParam(Conn, fork, bitmap);
	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (fork3) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (fork ) FPCloseFork(Conn,fork);
	if (fork2) FPCloseFork(Conn,fork2);
	if (fork3) FPCloseFork(Conn,fork3);
}

/* -------------------------- */
void test18()
{
u_int16_t bitmap = 0;
int fork;
int fork2;
int fork3;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test18: open deny mode/ fork attrib\n");

	/* success */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}


	/* success */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
	bitmap = 1;
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	/* success */
	if (FPGetForkParam(Conn, fork, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* success */
	FPCloseFork(Conn,fork);
	
	/* success */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork3) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPCloseFork(Conn,fork2);
	FPCloseFork(Conn,fork3);
}

/* -------------------------- */
void test19()
{
u_int16_t bitmap = 0;
int fork;
int fork2 = 0;
int fork3 = 0;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test19: open deny mode/ fork attrib\n");


	/* success */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_DWR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	/* success */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_DWR );
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_WR);
	if (fork3) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	bitmap = 1;
	/* success */
	if (FPGetForkParam(Conn, fork, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	/* success */
	if (FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}
	fork = 0;
	
	/* fail */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,OPENACC_WR);
	if (fork3) {
		fprintf(stderr,"\tFAILED\n");
	}
fin:
	if (fork) FPCloseFork(Conn,fork);
	if (fork2) FPCloseFork(Conn,fork2);
	if (fork3) FPCloseFork(Conn,fork3);
	
}

/* ------------------------- */
void test20()
{
u_int16_t bitmap = 0;
int fork;
int fork2;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test20: open file read only and read write\n");

	/* success */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD );

    if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
    }        
	/* success */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_WR );

    if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
    }        

	/* success */
	if (FPWrite(Conn, fork2, 0, 10, Data, 0 /*0x80 */)) {
		fprintf(stderr,"\tFAILED\n");
    }        

	/* success */
	if (FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
    }        

	/* success */
	if (FPCloseFork(Conn,fork2)) {
		fprintf(stderr,"\tFAILED\n");
    }        
}

/* ------------------------- */
void test21()
{
u_int16_t bitmap = 0;
int fork;
int fork2;
char *name = "tata";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test21: setting/reading fork len\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA /*_RSCS*/, bitmap ,DIRDID_ROOT, name,
						OPENACC_WR | OPENACC_RD); 
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	sleep(1);

	fork2= FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	sleep(1);

	FPRead(Conn, fork, 0, 18, Data);
	sleep(1);
	FPWrite(Conn, fork, 0, 100, Data, 0);

//	bitmap = 1 << FILPBIT_RFLEN;
	bitmap = 1 << FILPBIT_DFLEN;
	FPGetForkParam(Conn, fork, bitmap);
	FPGetForkParam(Conn, fork2, bitmap);
	
	FPFlushFork(Conn, fork);

	FPGetForkParam(Conn, fork, bitmap);
	FPGetForkParam(Conn, fork2, bitmap);

	FPCloseFork(Conn,fork);
	FPCloseFork(Conn,fork2);
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
#define BUF_S 3000
static char w_buf[BUF_S];
static char r_buf[BUF_S];

void test22()
{
int fork;
int fork1;
u_int16_t bitmap = 0;
char utf8name[20];
int i;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test22: AFP 3.0 FPWriteExt\n");

	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);
	                        
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 0, (1 << DIRPBIT_PDINFO ));

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

    /* > 2 Gb */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, "toto");
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		FPDelete(Conn, vol,  DIRDID_ROOT, "toto");
		return;
	}

	memset(w_buf, 'b', BUF_S);
	if (FPWrite_ext(Conn, fork, ((off_t)1 << 31) +20, 2000, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite_ext(Conn, fork1, ((off_t)1 << 31) +20, 1000, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPWrite_ext(Conn, fork, ((off_t)1 << 31) +1000 , 3000, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}	        

	if (FPWrite_ext(Conn, fork, 0 , 200, w_buf, 0x80 )) {
		fprintf(stderr,"\tFAILED\n");
	}	        

	if (FPWrite_ext(Conn, fork1, 0 , 200, w_buf, 0x80 )) {
		fprintf(stderr,"\tFAILED\n");
	}	        
	
	if (FPRead_ext(Conn, fork, 10, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else for (i = 0; i < 10000; i++) {
		if (Data[i] != 0) {
			fprintf(stderr,"\tFAILED Data != 0\n");
			break;
		}
	}
	
	if (FPRead_ext(Conn, fork, ((off_t)1 << 31) +20, 3000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else for (i = 0; i < 3000; i++) {
		if (Data[i] == 0) {
			fprintf(stderr,"\tFAILED Data == 0\n");
			break;
		}
	}

	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);
	                        
	FPCloseFork(Conn,fork);
	FPCloseFork(Conn,fork1);
	FPDelete(Conn, vol,  DIRDID_ROOT, "toto");

    /* ==========> 4 Gb ============= */

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "very big")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
    
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "very big",OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, "very big");
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "very big",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		FPDelete(Conn, vol,  DIRDID_ROOT, "very big");
		return;
	}

	if (FPWrite_ext(Conn, fork, ((off_t)1 << 32) +20, 2000, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite_ext(Conn, fork1, ((off_t)1 << 32) +20, 1000, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPRead_ext(Conn, fork, 10, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else for (i = 0; i < 10000; i++) {
		if (Data[i] != 0) {
			fprintf(stderr,"\tFAILED Data != 0\n");
			break;
		}
	}

	if (FPRead_ext(Conn, fork, ((off_t)1 << 32) +20, 1500, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else for (i = 0; i < 1500; i++) {
		if (Data[i] == 0) {
			fprintf(stderr,"\tFAILED Data == 0\n");
			break;
		}
	}

	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);
	                        
	FPCloseFork(Conn,fork);
	FPCloseFork(Conn,fork1);
	if (FPDelete(Conn, vol,  DIRDID_ROOT, "very big")) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ========= utf 8 name ======= */
	utf8name[0]  = 'a';
	utf8name[1]  = 0xcc;
	utf8name[2]  = 0x88;
	utf8name[3]  = 'o';
	utf8name[4]  = 0xcc;
	utf8name[5]  = 0x88;
	utf8name[6]  = 'u';
	utf8name[7]  = 0xcc;
	utf8name[8]  = 0x88;
	utf8name[9]  = '.';
	utf8name[10] = 't';
	utf8name[11] = 'x';
	utf8name[12] = 't';
	utf8name[13] = 0;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , utf8name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, utf8name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}	
}

/* ------------------------- */
void test23()
{
char *name = "imagemagick-5.4.3.tgz";
char *name1 = "Contents";
char *name2 = "PkgInfo";
int  dir,dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test23: AFP 3.0 FPEnumerate ext\n");

	FPGetSrvrInfo(Conn);
	FPGetSrvrParms(Conn);
	FPMapID(Conn, 1, 502); /* user to Mac roman */
	FPMapID(Conn, 2, 105); /* group to Mac roman */
	FPMapID(Conn, 3, 502); /* user to UTF8 */
	FPMapID(Conn, 4, 105); /* group to UTF8 */

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (dir) {
		dir1 = FPCreateDir(Conn,vol, dir , name1);
		if (dir1) {
			FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT));
			FPCreateFile(Conn, vol,  0, dir1 , name2);
			FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT));
			FPEnumerate_ext(Conn, vol,  dir1 , "", 
			                (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		        	            |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN),
	                        	(1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT)
		                     );
		}
		FPEnumerate_ext(Conn, vol,  dir , "", 
		                (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN) | (1 << FILPBIT_LNAME),
	                        (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT) | (1 << DIRPBIT_LNAME)
	                     );
	    if (dir1) {
			FPDelete(Conn, vol,  dir1, name2);
		}
		if (dir) {
			FPDelete(Conn, vol,  dir, name1);
			FPDelete(Conn, vol,  DIRDID_ROOT, name);
		}
	}
}

/* ------------------------- */
void test24()
{
char *name  = "Un nom long 0123456789 0123456789 0123456789 0123456789.txt";
char *name1 = "nouveau nom long 0123456789 0123456789 0123456789 0123456789";
char *name2 = "Contents";
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test24: mangled name\n");

	FPDelete(Conn, vol,  DIRDID_ROOT , name);
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (FPRename(Conn, vol, DIRDID_ROOT, name, "test1")) {fprintf(stderr,"\tFAILED\n");}
	if (FPRename(Conn, vol, DIRDID_ROOT, "test1", name)) {fprintf(stderr,"\tFAILED\n");}

	FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);



	if (FPRename(Conn, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name1, name);
	FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name2);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name1);
	FPDelete(Conn, vol,  dir , name1);
	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test25()
{
char *name = "imagemagick-5.4.3.tgz";
char *name1 = "Contents";
char *name2 = "PkgInfo";
int  dir,dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test25: FPEnumerate ext2\n");
	FPGetSrvrInfo(Conn);
	FPGetSrvrParms(Conn);

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	FPEnumerate_ext2(Conn, vol,  DIRDID_ROOT , "", 
			 	(1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		        	|(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN),
	                        (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT)
		         );

	dir1 = FPCreateDir(Conn,vol, dir , name1);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT));
	FPCreateFile(Conn, vol,  0, dir1 , name2);
	FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT));
	FPEnumerate_ext2(Conn, vol,  dir1 , "", 
			         (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		        	 |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN),
	                 (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT));
	FPEnumerate_ext2(Conn, vol,  dir , "", 
		             (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		             |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN) | (1 << FILPBIT_LNAME),
	                 (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT) | (1 << DIRPBIT_LNAME));
	FPDelete(Conn, vol,  dir1, name2);
	FPDelete(Conn, vol,  dir, name1);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test26()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test26: folder without right access\n");

    fprintf(stderr,"\tREMOVED\n");
    return;
    
	if (ntohl(AFPERR_ACCESS) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bar", 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<< FILPBIT_LNAME) | (1<< FILPBIT_FNUM ),
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

	if (ntohl(AFPERR_ACCESS) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bar", 0, 
	     (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	     (1 << DIRPBIT_ACCESS))
	   ) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test27()
{
char *name  = "Un nom long";
char *name2 = "Contents";
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test27: delete not empty dir\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name2);

	FPCreateFile(Conn, vol,  0, dir , name);


	if (!FPDelete(Conn, vol,  DIRDID_ROOT , name2)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	FPDelete(Conn, vol,  dir , name);
	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
}

/* ------------------------- */
void test28()
{
char *name  = "t28";
char *name1 = "Contents";
char *name2 = "foo";
int  dir;
int  dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test28: test search by ID\n");

	vol  = FPOpenVol(Conn, Vol);

	dir   = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseVol(Conn,vol);
		return;
	}

	dir1  = FPCreateDir(Conn,vol, dir , name1);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		FPCloseVol(Conn,vol);
		return;
	}

	FPCreateFile(Conn, vol,  0, dir1 , name2);

	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	if (FPEnumerate(Conn, vol,  dir1 , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, "t28/Contents/foo");
		FPDelete(Conn, vol,  DIRDID_ROOT, "t28/Contents");
		FPDelete(Conn, vol,  DIRDID_ROOT, "t28");
		FPCloseVol(Conn,vol);
		return;
	}

	FPDelete(Conn, vol,  dir1 , name2);
	FPDelete(Conn, vol,  dir , name1);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPCloseVol(Conn,vol);
}

static char temp[MAXPATHLEN];   
static char temp1[MAXPATHLEN];   

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
		fprintf(stderr,"Inode for %s: %lld %lld\n", Path, st.st_ino, st1.st_ino);
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
/* ------------------------- */
void test31()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test31: map ID\n");
	FPGetSrvrParms(Conn);
	FPMapID(Conn, 1, 502); /* user to Mac roman */
	FPMapID(Conn, 2, 105); /* group to Mac roman */
	FPMapID(Conn, 3, 502); /* user to UTF8 */
	FPMapID(Conn, 4, 105); /* group to UTF8 */
}

/* ------------------------- */
void test32()
{
char *name = "Contents";
char *name1 = "foo";
int  dir,dir1;
int  ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test32: dir deleted by someone else, access with ID\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPCreateFile(Conn, vol,  0, dir , name1);

	if (FPEnumerate(Conn, vol,  dir, "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPDelete(Conn, vol,  dir , name1);
	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble/.Parent");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble");
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	/* our curdir is in the deleted folder so no error! 
	   or it's a nfs exported volume
	*/
	ret = FPGetFileDirParams(Conn, vol,  dir, "", 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS));
		
	if (ret && ret != ntohl(AFPERR_NOOBJ)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT, "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol,  dir, "", 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	dir1  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  dir1, "", 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}

    /* dir and dir1 should be != but if inode reused they are the same */
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test33()
{
char *name = "Contents";
char *name1 = "foo";
int  dir,dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test33: dir deleted by someone else, access with name\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPCreateFile(Conn, vol,  0, dir , name1);

	if (FPEnumerate(Conn, vol, DIRDID_ROOT, name, 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPDelete(Conn, vol,  dir , name1);
	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble/.Parent");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED %s %s\n", temp, strerror(errno));
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble");
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	/* our curdir is in the deleted folder so no error! */
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT, "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	dir1  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

    /* dir and dir1 should be != but if inode reused they are the same */
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test34()
{
char *name = "essai permission";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test34: folder with --rwx-- perm\n");

    fprintf(stderr,"\tREMOVED\n");
	return;
	
	if (ntohl(AFPERR_ACCESS) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) 
	{
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<< FILPBIT_LNAME) | (1<< FILPBIT_FNUM ),
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

	if (ntohl(AFPERR_ACCESS) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, 
	     (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	     (1 << DIRPBIT_ACCESS))
	   ) 
	{
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
static char afp_cmd_with_vol[] = {
AFP_CLOSEVOL,			/* 2 */
#if 0
AFP_CLOSEDIR,			/* 3 */
#endif
AFP_COPYFILE, 			/* 5 */
AFP_CREATEDIR,			/* 6 */
AFP_CREATEFILE,			/* 7 */
AFP_DELETE,				/* 8 */
AFP_ENUMERATE,			/* 9 */
AFP_FLUSH,				/* 10 */

AFP_GETVOLPARAM,		/* 17 */
AFP_MOVE,				/* 23 */
AFP_OPENDIR,			/* 25 */
AFP_OPENFORK,           /* 26 */
AFP_RENAME,				/* 28 */
AFP_SETDIRPARAM,		/* 29 */
AFP_SETFILEPARAM, 		/* 30 */
AFP_SETVOLPARAM,		/* 32 */
AFP_GETFLDRPARAM,		/* 34 */
AFP_SETFLDRPARAM,		/* 35 */
AFP_CREATEID,			/* 39 */
AFP_DELETEID, 			/* 40 */
AFP_RESOLVEID,			/* 41 */
AFP_EXCHANGEFILE,		/* 42 */
AFP_CATSEARCH,			/* 43 */
AFP_OPENDT,				/* 48 */
AFP_GETICON,			/* 51 */
AFP_GTICNINFO,			/* 52 */
AFP_ADDAPPL,			/* 53 */
AFP_RMVAPPL,			/* 54 */
AFP_GETAPPL,			/* 55 */
AFP_ADDCMT,				/* 56 */
AFP_RMVCMT,				/* 57 */
AFP_GETCMT,				/* 58 */
AFP_ADDICON,			/* 192 */
};

void test35()
{
int i;
int ofs;
u_int16_t param = vol+1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test35: illegal volume\n");

	for (i = 0 ;i < sizeof(afp_cmd_with_vol);i++) {
		memset(dsi->commands, 0, DSI_CMDSIZ);
		dsi->header.dsi_flags = DSIFL_REQUEST;     
		dsi->header.dsi_command = DSIFUNC_CMD;
		dsi->header.dsi_requestID = htons(dsi_clientID(dsi));

		ofs = 0;
		dsi->commands[ofs++] = afp_cmd_with_vol[i];
		dsi->commands[ofs++] = 0;

		memcpy(dsi->commands +ofs, &param, sizeof(param));
		ofs += sizeof(param);
		
		dsi->datalen = ofs;
		dsi->header.dsi_len = htonl(dsi->datalen);
		dsi->header.dsi_code = 0; // htonl(err);
 
   		my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
		my_dsi_receive(dsi);
		
    	if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
			fprintf(stderr,"\tFAILED command %i\n", afp_cmd_with_vol[i]);
			return;
    	}
    }
}

/* ----------------------- */
static char afp_cmd_with_vol_did[] = {
	AFP_COPYFILE, 			/* 5 */
	AFP_CREATEDIR,			/* 6 */
	AFP_CREATEFILE,			/* 7 */
	AFP_DELETE,				/* 8 */
	AFP_OPENDIR,			/* 25 */
	AFP_OPENFORK,           /* 26 */
	AFP_RENAME,				/* 28 */
	AFP_SETDIRPARAM,		/* 29 */
	AFP_SETFILEPARAM, 		/* 30 */
	AFP_GETFLDRPARAM,		/* 34 */
	AFP_SETFLDRPARAM,		/* 35 */
	AFP_ADDAPPL,			/* 53 */
	AFP_RMVAPPL,			/* 54 */
	AFP_ADDCMT,				/* 56 */
	AFP_RMVCMT,				/* 57 */
	AFP_GETCMT,				/* 58 */
};

void test36()
{
int i;
int ofs;
u_int16_t param = vol;
char *name = "t36 dir";
int  dir;
int  did;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test36: no folder error ==> ERR_NOOBJ\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	did  = dir +1;

	for (i = 0 ;i < sizeof(afp_cmd_with_vol_did);i++) {
		memset(dsi->commands, 0, DSI_CMDSIZ);
		dsi->header.dsi_flags = DSIFL_REQUEST;     
		dsi->header.dsi_command = DSIFUNC_CMD;
		dsi->header.dsi_requestID = htons(dsi_clientID(dsi));

		ofs = 0;
		dsi->commands[ofs++] = afp_cmd_with_vol_did[i];
		dsi->commands[ofs++] = 0;

		memcpy(dsi->commands +ofs, &param, sizeof(param));
		ofs += sizeof(param);

		memcpy(dsi->commands +ofs, &did, sizeof(did));  /* directory did */
		ofs += sizeof(did);
		
		dsi->datalen = ofs;
		dsi->header.dsi_len = htonl(dsi->datalen);
		dsi->header.dsi_code = 0; // htonl(err);
 
   		my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
		my_dsi_receive(dsi);
		
    	if (ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
			fprintf(stderr,"\tFAILED command %i\n", afp_cmd_with_vol_did[i]);
    	}
    }

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------------------------- */
static char afp_cmd_with_vol_did1[] = {
		AFP_MOVE,				/* 23 */
		AFP_CREATEID,			/* 39 */
		AFP_EXCHANGEFILE,		/* 42 */
};

void test37()
{
int i;
int ofs;
u_int16_t param = vol;
char *name = "t37 dir";
int  dir;
int  did;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test37: no folder error ==> ERR_PARAM\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	did  = dir +1;

	for (i = 0 ;i < sizeof(afp_cmd_with_vol_did1);i++) {
		memset(dsi->commands, 0, DSI_CMDSIZ);
		dsi->header.dsi_flags = DSIFL_REQUEST;     
		dsi->header.dsi_command = DSIFUNC_CMD;
		dsi->header.dsi_requestID = htons(dsi_clientID(dsi));

		ofs = 0;
		dsi->commands[ofs++] = afp_cmd_with_vol_did1[i];
		dsi->commands[ofs++] = 0;

		memcpy(dsi->commands +ofs, &param, sizeof(param));
		ofs += sizeof(param);

		memcpy(dsi->commands +ofs, &did, sizeof(did));  /* directory did */
		ofs += sizeof(did);
		
		dsi->datalen = ofs;
		dsi->header.dsi_len = htonl(dsi->datalen);
		dsi->header.dsi_code = 0; // htonl(err);
 
   		my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
		my_dsi_receive(dsi);
		
    	if (ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
			fprintf(stderr,"\tFAILED command %i code ! %d\n", afp_cmd_with_vol_did1[i],
			      ntohl(dsi->header.dsi_code));
    	}
    }

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ----------------------- */
void test38()
{
u_int16_t bitmap = (1<< DIRPBIT_DID);
char *name = "new";
int  did;
int  fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test38: enumerate folder with no write access\n");

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	FPEnumerate(Conn, vol,  DIRDID_ROOT , name, 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

	FPEnumerate(Conn, vol,  did , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

	bitmap = 0;
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , did, "toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, did, "toto.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	FPCloseFork(Conn,fork);

}

/* ----------------------- */
void test39()
{
u_int16_t bitmap = 0;
char *name = "test/2.txt";
char *name1 = "test//test.txt";
char *name2 = "test///test.txt";
int  fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test39: cname path folder + filename\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name, OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		
	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name1, OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		
	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap, DIRDID_ROOT, name2, OPENACC_RD);
	if (fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		
	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS);

	if (!FPGetFileDirParams(Conn, vol,  DIRDID_ROOT_PARENT, "disk1", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT_PARENT, Vol, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}

/* ------------------------- */
void test40()
{
char *name  = "t40 dir";
char *name1 = "t40 file";
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test40: enumerate deleted folder\n");

	dir   = FPCreateDir(Conn,vol, DIRDID_ROOT , name);

	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		FPCloseVol(Conn,vol);
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPDelete(Conn, vol,  dir, name1);

	FPDelete(Conn, vol,  DIRDID_ROOT, name);

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol,  dir, "", 0, 
			(1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS))) 
	{
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		FPCloseVol(Conn,vol);
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test41()
{

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test41: enumerate folder not there\n");
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  DIRDID_ROOT , "test/foo/bar", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (ntohl(AFPERR_PARAM) != FPEnumerate(Conn, vol,  0 , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test42()
{
char *name = "t42 dir";
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test42: dir deleted by someone else, access with ID from another dir\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_NOOBJ) != FPEnumerate(Conn, vol,  dir, "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, "new", 0, 
			(1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS) | (1<<DIRPBIT_OFFCNT))) 
	{
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble/.Parent");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp, "/.AppleDouble");
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	/* our curdir is in the deleted folder so no error! */
	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol,  dir, "bar", 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  dir, "")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}

/* ------------------------- */
void test43()
{
char *name = "t43 dir";
char *name1= "foo";
int  dir = 0,dir1 = 0,dir2 = 0;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test43: rename, move and rename folder\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir1  = FPCreateDir(Conn,vol, DIRDID_ROOT , name1);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	dir2  = FPCreateDir(Conn,vol, dir1 , name1);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (FPRename(Conn, vol, DIRDID_ROOT, name, "test1")) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (FPMoveAndRename(Conn, vol, dir1, dir, name1, name1)) {
		fprintf(stderr,"\tFAILED\n");
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
		fprintf(stderr,"\tFAILED\n");
		return;
	}
fin:
	if (dir2 && FPDelete(Conn, vol,  dir2, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (dir1 && FPDelete(Conn, vol,  dir1, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (dir && FPDelete(Conn, vol,  dir, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test44()
{
char *name = "Contents";
int  dir;
int did;
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test44: access .. folder\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "/", 0,(1<< DIRPBIT_DID))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	if (dir != did) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetFileDirParams(Conn, vol,  dir, "//", 0,(1<< DIRPBIT_DID))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	if (DIRDID_ROOT != did) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir, "")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}
/* ------------------------- */
void test45()
{
char *name = "bar/Contents";
int  dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test45: Folder Creation\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (dsi->header.dsi_code != ntohl(AFPERR_ACCESS)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , "new");
	if (dsi->header.dsi_code != htonl(AFPERR_EXIST)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test46()
{
u_int16_t bitmap = 0;
int fork, fork1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test46: ressource fork fork\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (FPWrite(Conn, fork1, 8000, 2048, Data, 0 /*0x80 */)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		goto fin1;
	}
	FPFlushFork(Conn, fork1);

	if (FPRead(Conn, fork, 0, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork1, 0, 100, Data, 0x80 )) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}
	FPCloseFork(Conn,fork1);
	FPCloseFork(Conn,fork);
	/* ----------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "toto",OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (ntohl(AFPERR_ACCESS) != FPSetForkParam(Conn, fork, (1<<FILPBIT_RFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}

	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_RFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 10, 10000, Data)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}
	
	if (FPWrite(Conn, fork1, 100, 20, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPRead(Conn, fork, 110, 10, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
fin1:
	FPCloseFork(Conn,fork1);
fin:
	if (fork) FPCloseFork(Conn,fork);

	if (FPDelete(Conn, vol,  DIRDID_ROOT , "toto")) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test47()
{
u_int16_t bitmap = 0;
int fork, fork1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test47: open read only file read only then read write\n");
    fprintf(stderr,"test47: in a read only folder\n");


	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "new/toto.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "new/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		FPCloseFork(Conn,fork);
		return;
	}		

	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "new/toto.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "new/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		FPCloseFork(Conn,fork);
		return;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "new/toto.txt", OPENACC_RD);

	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}
	FPCloseFork(Conn,fork1);

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}

	FPCloseFork(Conn,fork);
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test47: in a read/write folder\n");


	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	FPCloseFork(Conn,fork);

	strcpy(temp, Path);strcat(temp,"/test folder/.AppleDouble/toto.txt");
	if (unlink(temp)) {
		fprintf(stderr,"\tRessource fork not there\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	FPCloseFork(Conn,fork);

	strcpy(temp, Path);strcat(temp,"/test folder/.AppleDouble/toto.txt");
	if (unlink(temp)) {
		fprintf(stderr,"\tRessource fork not there\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);
    /* bad, but we are able to open read-write the ressource for of a read-only file (data fork)
     * difficult to fix.
     */
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (FPWrite(Conn, fork1, 0, 10, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		FPCloseFork(Conn,fork1);
		return;
	}

	if (FPRead(Conn, fork, 0, 10, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		FPCloseFork(Conn,fork1);
		return;
	}

	FPCloseFork(Conn,fork);
	if (FPWrite(Conn, fork1, 0, 20, Data, 0x80 )) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		return;
	}
	
	if (ntohl(AFPERR_PARAM) != FPRead(Conn, fork, 0, 30, Data)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		return;
	}

	FPCloseFork(Conn,fork1);
}

/* ------------------------- */
void test48()
{
u_int16_t bitmap = 0;
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test48: open fork a folder\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "new", OPENACC_RD);

	if (fork) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}
    if (ntohl(AFPERR_BADTYPE) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test49()
{
u_int16_t bitmap = 0;
int fork, fork1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test49: open read-write file without ressource fork\n");
    fprintf(stderr,"test49: in a read-write folder\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto-rw.txt", OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	strcpy(temp, Path);strcat(temp,"/test folder/.AppleDouble/toto-rw.txt");
	unlink(temp);
	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto-rw.txt",OPENACC_WR | OPENACC_RD);

	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		

	FPCloseFork(Conn,fork);
	FPCloseFork(Conn,fork1);

	if (!unlink(temp)) {
		fprintf(stderr,"\tRessource fork there!\n");
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto-rw.txt",OPENACC_WR | OPENACC_RD);

	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		
	FPCloseFork(Conn,fork1);

	if (!unlink(temp)) {
		fprintf(stderr,"\tRessource fork there!\n");
	}

}

/* -------------------------- */
void test50()
{
u_int16_t bitmap;
int fork;
int fork2;
char *name = "toto";
char *name1 = "tata";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test50:  deny mode & move file\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	bitmap = (1 << FILPBIT_DFLEN);
	if (FPGetForkParam(Conn, fork, bitmap)) {
		fprintf(stderr,"\tFAILED to get fork data len\n");
	}

	bitmap = (1 << FILPBIT_RFLEN);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork, bitmap)) {
		fprintf(stderr,"\tFAILED got the fork ressource len!\n");
	}
	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		FPCloseFork(Conn,fork);
		FPDelete(Conn, vol,  DIRDID_ROOT , name);
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA, 0 ,DIRDID_ROOT, name1, OPENACC_WR);

    if (ntohl(AFPERR_DENYCONF) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (fork2) FPCloseFork(Conn,fork2);

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS, 0 ,DIRDID_ROOT, name1, OPENACC_WR);
	if (!fork2) {
		fprintf(stderr,"\tFAILED opening ressource fork\n");
		return;
	}
	bitmap = (1 << FILPBIT_DFLEN)| (1<< FILPBIT_MDATE);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork2, bitmap)) {
		fprintf(stderr,"\tFAILED got fork data len!\n");
	}

	bitmap = (1 << FILPBIT_RFLEN)| (1<< FILPBIT_MDATE);
	if (FPGetForkParam(Conn, fork2, bitmap)) {
		fprintf(stderr,"\tFAILED to get the fork ressource len!\n");
	}

	FPCloseFork(Conn,fork);

	bitmap = (1 << FILPBIT_DFLEN)| (1<< FILPBIT_MDATE);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork2, bitmap)) {
		fprintf(stderr,"\tFAILED got fork data len!\n");
	}

	bitmap = (1 << FILPBIT_RFLEN)| (1<< FILPBIT_MDATE);
	if (FPGetForkParam(Conn, fork2, bitmap)) {
		fprintf(stderr,"\tFAILED to get the fork ressource len!\n");
	}

	if (fork2) FPCloseFork(Conn,fork2);

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* -------------------------- */
void test51()
{
u_int16_t bitmap;
int fork;
char *name = "new";
char *name1 = "tata";
char *name2 = "toto.txt";
char *name3 = "folder";
int  dir;
int  did;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test51:  Create file with errors\n");

	bitmap = (1<< DIRPBIT_DID);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	if (ntohl(AFPERR_BADTYPE) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (ntohl(AFPERR_ACCESS) != FPCreateFile(Conn, vol,  0, did , name1)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_EXIST) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name2)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name2,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_EXIST) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name2)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}
	if (ntohl(AFPERR_BUSY) != FPCreateFile(Conn, vol,  1, DIRDID_ROOT , name2)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);
		return;
	}
	FPCloseFork(Conn,fork);

	if ( ntohl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  1, did , "folder/essai")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	/* ----------- */
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name3))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  1, dir , name2)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name3);
		return;
	}

	if (FPCreateFile(Conn, vol,  1, dir , name2)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir , name2)) { 
		fprintf(stderr,"\tNOT TESTED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name3)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
}

/* -------------------------- */
void test52()
{
    fprintf(stderr,"===================\n");
	fprintf(stderr, "test52: test .file without AppleDouble\n");

	if (!FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "new/.invisible", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
	         0
		)) 
	{
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* -------------------------- */
void test53()
{
int fork;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test53: get comment\n");

	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , "bar")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetComment(Conn, vol,  DIRDID_ROOT , "test folder")) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, "toto.txt",OPENACC_RD );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);
}

/* -------------------------- */
void test54()
{
int fork;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test54: remove comment\n");

	if (ntohl(AFPERR_ACCESS) != FPRemoveComment(Conn, vol,  DIRDID_ROOT , "bar")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPRemoveComment(Conn, vol,  DIRDID_ROOT , "new")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , "test folder")) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, "toto.txt",OPENACC_RD );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);

	if (ntohl(AFPERR_NOITEM) != FPRemoveComment(Conn, vol,  DIRDID_ROOT , "bogus folder")) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test55()
{
int fork;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test55: add comment\n");

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , "bar","essai")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , "new","essai")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPAddComment(Conn, vol,  DIRDID_ROOT , "toto.txt", "Comment for toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPAddComment(Conn, vol,  DIRDID_ROOT , "test folder", "Comment for test folder")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , "test folder")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, "toto.txt",OPENACC_RD );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPAddComment(Conn, vol,  DIRDID_ROOT , "toto.txt", "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , "toto.txt")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	FPCloseFork(Conn,fork);

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , "bogus folder","essai")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* -------------------------- */
void test56()
{
u_int16_t bitmap;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test56: Volume parameters\n");
    bitmap = (1 << VOLPBIT_ATTR  )|
	    (1 << VOLPBIT_SIG   )|
    	(1 << VOLPBIT_CDATE )|
	    (1 << VOLPBIT_MDATE )|
    	(1 << VOLPBIT_BDATE )|
	    (1 << VOLPBIT_VID   )|
    	(1 << VOLPBIT_BFREE )|
	    (1 << VOLPBIT_BTOTAL)|
    	(1 << VOLPBIT_NAME)|
	    (1 << VOLPBIT_XBFREE)|
    	(1 << VOLPBIT_XBTOTAL)|
	    (1 << VOLPBIT_BSIZE);
	    
 	if (FPGetVolParam(Conn, vol, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
} 	

/* -------------------------- */
void test57()
{
int dir;
    fprintf(stderr,"===================\n");
	fprintf(stderr, "test57: OpenDir call\n");

	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , "bar");
    if (dir || ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , "new");
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , "toto.txt");
    if (ntohl(AFPERR_BADTYPE) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}

/* -------------------------- */
void test58()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test58: folder 1 (DIRDID_ROOT_PARENT\n");

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT_PARENT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_OFFCNT))
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_OFFCNT))
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}

/* -------------------------- */
void test59()
{
int fork;
u_int16_t bitmap = 0;
char *name = "test59 FPRead,FPWrite 2GB lim";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test59: 2 GBytes for offset limit FPRead, FPWrite\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

    /* > 2 Gb */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	if (ntohl(AFPERR_PARAM) != FPRead(Conn, fork, ((off_t)1 << 31) +20, 3000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_PARAM) != FPWrite(Conn, fork, ((off_t)1 << 31) +20, 3000, Data, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_DFULL) != FPWrite(Conn, fork, 0x7fffffff, 30, Data,0)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
static char afp_cmd_with_fork[] = {
AFP_BYTELOCK,
AFP_CLOSEFORK,
AFP_GETFORKPARAM,
AFP_SETFORKPARAM,
AFP_READ,
AFP_FLUSHFORK,
AFP_WRITE,
};

void test60()
{
int i;
int ofs;
int fork;
u_int16_t param;
u_int16_t bitmap = 0;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test60: illegal fork\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , "toto")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, "toto");
		return;
	}

	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, "toto");

	param  = fork;

	for (i = 0 ;i < sizeof(afp_cmd_with_fork);i++) {
		memset(dsi->commands, 0, DSI_CMDSIZ);
		dsi->header.dsi_flags = DSIFL_REQUEST;     
		dsi->header.dsi_command = DSIFUNC_CMD;
		dsi->header.dsi_requestID = htons(dsi_clientID(dsi));

		ofs = 0;
		dsi->commands[ofs++] = afp_cmd_with_fork[i];
		dsi->commands[ofs++] = 0;

		memcpy(dsi->commands +ofs, &param, sizeof(param));
		ofs += sizeof(param);
		
		dsi->datalen = ofs;
		dsi->header.dsi_len = htonl(dsi->datalen);
		dsi->header.dsi_code = 0; // htonl(err);
 
   		my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
		my_dsi_receive(dsi);
		
    	if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
			fprintf(stderr,"\tFAILED command %i\n", afp_cmd_with_fork[i]);
    	}
    }
}

/* -------------------------- */
void test61()
{
int fork;
u_int16_t bitmap = 0;
char *name = "test61 FPRead, FPWrite error";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test61: FPRead, FPWrite errors\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (ntohl(AFPERR_ACCESS) != FPRead(Conn, fork, 0, 30, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork, 0, 0, Data, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_RD );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (ntohl(AFPERR_ACCESS) != FPWrite(Conn, fork, 0, 30, Data,0)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRead(Conn, fork, 0, 0, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR|OPENACC_RD );
	if (FPWrite(Conn, fork, 0, 300, Data, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 400, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork, 0, 9000, Data, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 10000, Data) ) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
void test62()
{
int fork;
int fork1;

u_int16_t bitmap = 0;
char *name = "test62 SetForkParams Errors";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test62: SetForkParams errors\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	if (ntohl(AFPERR_PARAM) != FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), (1<< 31))) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_PARAM) != FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), -2)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_BITMAP) != FPSetForkParam(Conn, fork, 1<<FILPBIT_ATTR, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPRead(Conn, fork, 0, 100, Data)) {fprintf(stderr,"\tFAILED\n");}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {fprintf(stderr,"\tFAILED\n");}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 1, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock(Conn, fork, 0, 0 , 0 /* set */, 100)) {fprintf(stderr,"\tFAILED\n");}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 50)) {
			fprintf(stderr,"\tFAILED\n");
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1)
		fprintf(stderr,"\tFAILED\n");
	else {
		if (!FPByteLock(Conn, fork1, 0, 0 , 60 , 100)) {
			fprintf(stderr,"\tFAILED\n");
			FPByteLock(Conn, fork1, 0, 1 , 60 , 100);
		}
		if (htonl(AFPERR_LOCK) != FPSetForkParam(Conn, fork1, (1<<FILPBIT_DFLEN), 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork1);
	}
	if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 200)) {
			fprintf(stderr,"\tFAILED\n");
	}
	if (FPByteLock(Conn, fork, 0, 0 , 0 /* set */, 100)) {fprintf(stderr,"\tFAILED\n");}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 50)) {
			fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) {fprintf(stderr,"\tFAILED\n");}

	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
static void test_bytelock(char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0, 0 , -1, 75)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NORANGE) != FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 75)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if ( htonl(AFPERR_RANGEOVR) != FPByteLock(Conn, fork, 0, 0 , 80 /* set */, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1)
		fprintf(stderr,"\tFAILED\n");
	else {
		if (htonl(AFPERR_LOCK) != FPByteLock(Conn, fork1, 0, 0 /* set */ , 20, 60)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPSetForkParam(Conn, fork, len , 50)) {
				fprintf(stderr,"\tFAILED\n");
		}

		if (FPSetForkParam(Conn, fork1, len , 60)) {
				fprintf(stderr,"\tFAILED\n");
		}
		if (htonl(AFPERR_LOCK) != FPRead(Conn, fork1, 0, 40, Data)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (htonl(AFPERR_LOCK) != FPWrite(Conn, fork1, 10, 40, Data, 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork1);
	}

	/* fin */
	if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NORANGE) != FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, 50)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork, len , 200)) {fprintf(stderr,"\tFAILED\n");}

	if (FPByteLock(Conn, fork, 1 /* end */, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 200, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock(Conn, fork, 0 /* end */, 0 /* set */, 0, -1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, -1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0 /* start */, 0 /* set */, 0, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_PARAM) != FPByteLock(Conn, fork, 0 /* start */, 0 /* set */, 0, -2)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseFork(Conn,fork);
	if (htonl (AFPERR_PARAM ) != FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name);

}
/* ----------- */
void test63()
{
char *name = "test63 FPByteLock DF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test63: FPByteLock Data Fork\n");
	test_bytelock(name, OPENFORK_DATA);
	return;
}

/* ----------- */
void test64()
{
char *name = "test64 FPByteLock RF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test64: FPByteLock Ressource Fork\n");
	test_bytelock(name, OPENFORK_RSCS);
	return;
}

/* -------------------------- */
static void test_bytelock3(char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
u_int16_t vol2;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseVol(Conn2,vol2);
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);FPCloseVol(Conn2,vol2);
		return;
	}

	if (FPSetForkParam(Conn, fork, len , 50)) {fprintf(stderr,"\tFAILED\n");}
	if (FPByteLock(Conn, fork, 0, 0 , 0 , 100)) {fprintf(stderr,"\tFAILED\n");}

	fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (htonl(AFPERR_LOCK) != FPRead(Conn2, fork1, 0, 40, Data)) {fprintf(stderr,"\tFAILED\n");}
		if (htonl(AFPERR_LOCK) != FPWrite(Conn2, fork1, 10, 40, Data, 0)) {fprintf(stderr,"\tFAILED\n");}
		if (htonl(AFPERR_LOCK) != FPWrite(Conn2, fork1, 55, 40, Data, 0)) {fprintf(stderr,"\tFAILED\n");}
		if (FPSetForkParam(Conn2, fork1, len , 60)) {
				fprintf(stderr,"\tFAILED\n");
		}
		if (FPWrite(Conn, fork, 55, 40, Data, 0)) {fprintf(stderr,"\tFAILED\n");}
		if (htonl(AFPERR_LOCK) != FPSetForkParam(Conn2, fork1, len , 60)) {fprintf(stderr,"\tFAILED\n");}
		if (htonl(AFPERR_LOCK) != FPByteLock(Conn2, fork1, 0, 0 /* set */ , 20, 60)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPSetForkParam(Conn, fork, len , 200)) {fprintf(stderr,"\tFAILED\n");}
		if (FPSetForkParam(Conn2, fork1, len ,120)) {fprintf(stderr,"\tFAILED\n");}
		
	}
	FPCloseFork(Conn,fork);
	if (fork1) FPCloseFork(Conn2,fork1);

	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPCloseVol(Conn2,vol2);
}

/* --------------- */
void test65()
{
char *name = "t65 DF FPByteLock 2 users";

    if (!Conn2) {
    	return;
    }
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test65: FPByteLock 2users DATA FORK\n");
    test_bytelock3(name, OPENFORK_DATA);

	name = "t65 RF FPByteLock 2 users";
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test65: FPByteLock 2users Ressource FORK\n");
    test_bytelock3(name, OPENFORK_RSCS);
}

/* -------------------------- */
static void test_bytelock_ext(char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (FPByteLock_ext(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock_ext(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_PARAM) != FPByteLock_ext(Conn, fork, 0, 0 , -1, 75)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NORANGE) != FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 0, 75)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if ( htonl(AFPERR_RANGEOVR) != FPByteLock_ext(Conn, fork, 0, 0 , 80 /* set */, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1)
		fprintf(stderr,"\tFAILED\n");
	else {
		if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn, fork1, 0, 0 /* set */ , 20, 60)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPSetForkParam(Conn, fork, len , 50)) {
				fprintf(stderr,"\tFAILED\n");
		}

		if (FPSetForkParam(Conn, fork1, len , 60)) {
				fprintf(stderr,"\tFAILED\n");
		}
		if (htonl(AFPERR_LOCK) != FPRead(Conn, fork1, 0, 40, Data)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (htonl(AFPERR_LOCK) != FPWrite(Conn, fork1, 10, 40, Data, 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork1);
	}

	/* fin */
	if (FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NORANGE) != FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 0, 50)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork, len , 200)) {fprintf(stderr,"\tFAILED\n");}

	if (FPByteLock_ext(Conn, fork, 1 /* end */, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 200, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPByteLock_ext(Conn, fork, 0 /* end */, 0 /* set */, 0, -1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPByteLock_ext(Conn, fork, 0, 1 /* clear */ , 0, -1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_PARAM) != FPByteLock_ext(Conn, fork, 0 /* start */, 0 /* set */, 0, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_PARAM) != FPByteLock_ext(Conn, fork, 0 /* start */, 0 /* set */, 0, -2)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseFork(Conn,fork);
	if (htonl (AFPERR_PARAM ) != FPByteLock_ext(Conn, fork, 0, 0 /* set */, 0, 100)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name);

}
/* ----------- */
void test66()
{
char *name = "t66 FPByteLock_ext DF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test66: FPByteLock Data Fork\n");
	test_bytelock_ext(name, OPENFORK_DATA);
	return;
}

/* ----------- */
void test67()
{
char *name = "t67 FPByteLock_ext RF";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test67: FPByteLock Ressource Fork\n");
	test_bytelock_ext(name, OPENFORK_RSCS);
	return;
}

/* ----------- */
void test68()
{
int ofs;
u_int16_t bitmap;
int len;
int did = DIRDID_ROOT;
char *name = "new/test.txt\0";

	if (Conn->afp_version >= 30)
		return;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test68: cname with a trailing 0??\n");

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"GetFileDirParams Vol %d \n\n", vol);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_GETFLDRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(1 << FILPBIT_LNAME);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(DIRPBIT_LNAME);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 2;		/* long name */
	len = strlen(name) +1;
	dsi->commands[ofs++] = len;
	u2mac(&dsi->commands[ofs], name, len);
	ofs += len;

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; // htonl(err);
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_stream_receive(dsi, dsi->data, DSI_DATASIZ, &dsi->datalen);

	dump_header(dsi);
	if (dsi->header.dsi_code != 0) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test69()
{
int  dir;
char *name = "t69 rename weird name Newé dir";
char *name2 = "test1";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test69: rename a folder with Unix name != Mac name\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPRename(Conn, vol, DIRDID_ROOT, name, "test1")) {fprintf(stderr,"\tFAILED\n");}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_EXIST) != FPRename(Conn, vol, DIRDID_ROOT, name, name2)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) {fprintf(stderr,"\tFAILED\n");}
	if (FPRename(Conn, vol, DIRDID_ROOT, name, name2)) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) {fprintf(stderr,"\tFAILED\n");}

    /* other sens */
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPRename(Conn, vol, DIRDID_ROOT, name2, name)) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) {fprintf(stderr,"\tFAILED\n");}
}

/* ----------- */
void test70()
{
int ofs;
u_int16_t bitmap;
int len;
int did = DIRDID_ROOT;
char *name = "t70 bogus cname";

	if (Conn->afp_version >= 30)
		return;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test70: bogus cname (unknow type)\n");

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"GetFileDirParams Vol %d \n\n", vol);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_GETFLDRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(1 << FILPBIT_LNAME);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(DIRPBIT_LNAME);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 4;		/* ERROR !! long name */
	len = strlen(name);
	dsi->commands[ofs++] = len;
	u2mac(&dsi->commands[ofs], name, len);
	ofs += len;

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; // htonl(err);
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_stream_receive(dsi, dsi->data, DSI_DATASIZ, &dsi->datalen);

	dump_header(dsi);
	if (dsi->header.dsi_code != htonl(AFPERR_PARAM)) {
		fprintf(stderr,"\tFAILED\n");
	}

}

/* ------------------------- */
void test71()
{
int fork;
int dir;
int dir1;
u_int16_t bitmap = 0;
char *name  = "t71 Copy file";
char *name1 = "t71 new file name";
char *name2 = "t71 dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test71: Copy file\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { fprintf(stderr,"\tFAILED\n");}

	/* sdid bad */
	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	/* cname unchdirable */
	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, "bar", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* second time once bar is in the cache */
	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, "bar", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {fprintf(stderr,"\tFAILED\n");}

	/* source is a dir */
	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name2, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (fork) {
		if (ntohl(AFPERR_DENYCONF) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork);
	}	
	/* dvol bad */
	if (ntohl(AFPERR_PARAM) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol +1, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ddid bad */
	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT , vol, dir,  name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ok */
	if (FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPDelete(Conn, vol,  DIRDID_ROOT, name1);
fin:	
	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test72()
{
int dir;
char *name  = "t72 check rename input";
char *name1 = "t72 new folder";
char *name2 = "t72 dir";
int  ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test72: check rename input\n");

	if (ntohl(AFPERR_NORENAME) != FPRename(Conn, vol, DIRDID_ROOT, "", "volume")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	ret = FPRename(Conn, vol, DIRDID_ROOT, "Network Trash Folder", "volume");
	if (ntohl(AFPERR_OLOCK) != ret) {
		fprintf(stderr,"\tFAILED\n");
		if (!ret) {
			FPRename(Conn, vol, DIRDID_ROOT, "volum", "Network Trash Folder");
		}
		return;
	}
	
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	/* FIXME should FAIL! */
	if (!FPRename(Conn, vol, DIRDID_ROOT, name2, name2)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRename(Conn, vol, DIRDID_ROOT, name, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRename(Conn, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
}

/* ------------------------- */
void test73()
{
int fork;
int dir;
int dir1;
int dir2;
u_int16_t bitmap = 0;
char *name  = "t73 Move and rename";
char *name1 = "T73 Move and rename";
char *name2 = "t73 dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test73: Move and rename\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { fprintf(stderr,"\tFAILED\n");}

	/* cname unchdirable */
	if (ntohl(AFPERR_ACCESS) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, "bar/essay", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	/* bad ddid */
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {fprintf(stderr,"\tFAILED\n");}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir1, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPMoveAndRename(Conn, vol, dir1, DIRDID_ROOT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* dirty but well */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name1)){fprintf(stderr,"\tFAILED\n");}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (ntohl(AFPERR_EXIST) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)){
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork);
	}	
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPDelete(Conn, vol,  DIRDID_ROOT, name1);

	if (!(dir2 = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (!FPCreateDir(Conn,vol, dir2 , name1)) {fprintf(stderr,"\tFAILED\n");}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir1,  name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  dir2, name1)) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1, name)) {fprintf(stderr,"\tFAILED\n");}

	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
	FPFlush(Conn, vol);
}

/* -------------------------- */
void test74()
{
int fork;
u_int16_t bitmap = 0;
u_int16_t vol2;
char *name = "t74 Delete File 2 users";
int type = OPENFORK_DATA;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    if (!Conn2) {
    	return;
    }
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test74: Delete File 2 users\n");

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseVol(Conn2,vol2);
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);FPCloseVol(Conn2,vol2);
		return;
	}

	if (FPSetForkParam(Conn, fork, len , 50)) {fprintf(stderr,"\tFAILED\n");}

	if (htonl(AFPERR_BUSY)!= FPDelete(Conn2, vol2,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);

	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPCloseVol(Conn2,vol2);

}

/* -------------------------- */
void test75()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test75: Get User Info\n");

	if (htonl(AFPERR_PARAM) != FPGetUserInfo(Conn, 0, 0, 1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_BITMAP) != FPGetUserInfo(Conn, 1, 0, 0xff)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetUserInfo(Conn, 1, 0, 3)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetUserInfo(Conn, 1, 0, 1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetUserInfo(Conn, 1, 0, 2)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* FIXME */
	if (FPMapName(Conn, 3, User?User:"didier")) {
		fprintf(stderr,"\tFAILED\n");
	} 
}

/* -------------------------- */
void test76()
{
int  dir;
char *name = "t76 Resolve ID file";
char *name1 = "t76 Resolve ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test76: Resolve ID\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPCreateID(Conn,vol, dir, name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (htonl(AFPERR_BITMAP) != FPResolveID(Conn, vol, filedir.did, 0xffff)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
		
	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}

}

/* -------------------------- */
void test77()
{
int fork;
int dir;
char *name = "t77 Move open fork other dir";
char *name1 = "t77 dir";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "t77: Move an open fork in a different folder\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name,
		OPENACC_RD| OPENACC_WR|  OPENACC_DWR );

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);
	if (FPDelete(Conn, vol,  dir , name)) {fprintf(stderr,"\tFAILED\n");}
	if (!FPDelete(Conn, vol, DIRDID_ROOT , name)) {fprintf(stderr,"\tFAILED\n");}
	FPDelete(Conn, vol,  DIRDID_ROOT , name1);
}

/* ---------------------------- */
static void test_bytelock2(char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	/* fin */
	if (FPByteLock(Conn, fork, 0 /* end */, 0 /* set */, 0, -1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (Conn->afp_version >= 30) {
			fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
			if (!fork1)
				fprintf(stderr,"\tFAILED\n");
			else {
				if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn, fork1, 0, 0, 20, 60)) {
					fprintf(stderr,"\tFAILED\n");
				}
				if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn, fork1, 0, 0, ((off_t)1<<32)+2, 60)) {
					fprintf(stderr,"\tFAILED\n");
					FPByteLock_ext(Conn, fork1, 0, 1, ((off_t)1<<32)+2, 60);
				}
				FPCloseFork(Conn,fork1);
			}
    		if (Conn2) {
				u_int16_t vol2;

				dsi2 = &Conn2->dsi;
				vol2  = FPOpenVol(Conn2, Vol);

				fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
				if (!fork1) {
					fprintf(stderr,"\tFAILED\n");
				}
				else {
					if (htonl(AFPERR_LOCK) != FPByteLock_ext(Conn2, fork1, 0, 0, ((off_t)1<<32)+2, 60)) {
						fprintf(stderr,"\tFAILED\n");
						FPByteLock_ext(Conn2, fork1, 0, 1, ((off_t)1<<32)+2, 60);
					}
					if (htonl(AFPERR_LOCK)  != FPWrite_ext(Conn2, fork1, ((off_t)1<<32)+2, 40, Data, 0)) {
						fprintf(stderr,"\tFAILED\n");
					}
					FPCloseFork(Conn2,fork1);
				}
				FPCloseVol(Conn2,vol2);
			}
		}
		if (FPByteLock(Conn, fork, 0, 1 /* clear */ , 0, -1)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
void test78()
{
char *name = "t78 FPByteLock RF size -1";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test78: test Byte Lock size -1 with no large file support\n");
	test_bytelock2(name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test78: test Byte Lock size -1 with no large file support, DATA fork\n");
	name = "t78 FPByteLock DF size -1";
	test_bytelock2(name, OPENFORK_DATA);
}

/* ----------- */
void test79()
{
int fork;
int fork1;
u_int16_t bitmap = 0;
int type = OPENFORK_DATA;
char *name = "t79 FPByteLock Read";
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test79: test Byte Lock follow by read\n");
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (FPSetForkParam(Conn, fork, len , 60)) {fprintf(stderr,"\tFAILED\n");}

	if (FPByteLock(Conn, fork, 0, 0, 0, 100)) {fprintf(stderr,"\tFAILED\n");}

	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1)
		fprintf(stderr,"\tFAILED\n");
	else {
		if (htonl(AFPERR_LOCK) != FPRead(Conn, fork1, 0, 40, Data)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork1);
	}

	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
static void test_bytelock5(char *name, int type)
{
int fork;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	if (FPSetForkParam(Conn, fork, len , 50)) {fprintf(stderr,"\tFAILED\n");}

	if (FPByteLock(Conn, fork, 0, 0 , 0 , 100)) {fprintf(stderr,"\tFAILED\n");}
	if (FPRead(Conn, fork, 0, 40, Data)) {fprintf(stderr,"\tFAILED\n");}
	if (FPWrite(Conn, fork, 10, 120, Data, 0)) {fprintf(stderr,"\tFAILED\n");}

	if (FPByteLock(Conn, fork, 0, 1 , 0 , 100)) {fprintf(stderr,"\tFAILED\n");}
	FPCloseFork(Conn,fork);
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
void test80()
{
char *name = "t80 RF FPByteLock Read write";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test80: Ressource Fork test Byte Lock and read write same user(file)\n");
	test_bytelock5(name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test80: Data Fork test Byte Lock and read write same user(file)\n");
	name = "t80 DF FPByteLock Read write";
	test_bytelock5(name, OPENFORK_DATA);
}

/* ---------------------------- */
static void test_denymode(char *name, int type)
{
int fork;
int fork1;
int fork2;
int fork3;
u_int16_t bitmap = 0;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	fork2 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork2) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}
	fork3 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD | OPENACC_DWR);
	if (!fork3) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (fork1 || dsi->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
		fprintf(stderr,"\tFAILED\n");
		if (fork1) FPCloseFork(Conn,fork1);
	}
	if (fork3) FPCloseFork(Conn,fork3);
	
    if (Conn2) {
		u_int16_t vol2;

		dsi2 = &Conn2->dsi;
		vol2  = FPOpenVol(Conn2, Vol);

		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
		if (fork1 || dsi2->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
			fprintf(stderr,"\tFAILED\n");
			if (fork1) FPCloseFork(Conn2,fork1);
		}
		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_RD);
		if (!fork1) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			FPCloseFork(Conn2,fork1);
		}
		fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
		if (fork1 || dsi2->header.dsi_code != ntohl(AFPERR_DENYCONF)) {
			fprintf(stderr,"\tFAILED\n");
			if (fork1) FPCloseFork(Conn2,fork1);
		}
		FPCloseVol(Conn2,vol2);
	}
	FPCloseFork(Conn,fork);
	if (fork2) FPCloseFork(Conn,fork2);
	
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------------- */
void test81()
{
char *name = "t81 Denymode RF 2users";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test81: Deny mode 2 users RF\n");
		test_denymode(name, OPENFORK_RSCS);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test81: Deny mode 2 users DF\n");
	name = "t81 Denymode DF 2users";
	test_denymode(name, OPENFORK_DATA);
}

/* ------------------------- */
void test82()
{
int  dir;
char *name = "t82 test dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;

#if 0
/* (1<<DIRPBIT_ATTR) |*/ (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			/* (1 << DIRPBIT_GID) |*/(1 << DIRPBIT_ACCESS);
#endif


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t82: test dir\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}


	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test83()
{
char *name = "t83 test file setfilparam";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);



    fprintf(stderr,"===================\n");
    fprintf(stderr,"t83: test set file setfilparam\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test84()
{
int  dir;
char *name = "t84 no delete dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t84: test dir no delete attribute\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
		if (ntohl(AFPERR_OLOCK) != FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
			fprintf(stderr,"\tFAILED\n");
			return;
		}
		filedir.attr = ATTRBIT_NODELETE;
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- 
   dirA
     child --> dirB
                next --> dirB
                prev --> dirB

     child --> dirB
                next --> dirA1
                prev --> dirA1
                           next --> dirB
                           prev --> dirB
                	       
     child --> dirA2
                next --> dirA1                	       
   dirC
     child --> symX
                 next --> dirA1
                 prev --> dirA1


   dirA
     child --> dirB
                next --> dirB
                prev --> dirA1
                
     			
*/
void test85()
{
struct sigaction action;    
struct itimerval    it;     

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t85: test bogus symlink\n");

   	it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 15;
    it.it_value.tv_usec = 0;

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/dirC");
	/* need to ckeck we have
     * dirC/
          folder
          symlink to dirA/dirB
		  folder2
       an readdir return folder before symlink befor folder2!
     */
    action.sa_handler = alarm_handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGHUP);
    sigaddset(&action.sa_mask, SIGTERM);
    action.sa_flags = SA_RESTART | SA_ONESHOT;
    if ((sigaction(SIGALRM, &action, NULL) < 0) ||
            (setitimer(ITIMER_REAL, &it, NULL) < 0)) {
		fprintf(stderr,"\tFAILED\n");
		return;
    }

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirA", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirC", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirA", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirC", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


   	it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
   	setitimer(ITIMER_REAL, &it, NULL);

}

/* ---------------------- */
void test86() {
int  dir;
char *name = "t86 new dir";
char *name1 = "t86 admin create file";
u_int16_t vol2;
int fork;
u_int16_t bitmap = 0;
u_int16_t dbitmap = (1 << DIRPBIT_ACCESS);
struct afp_filedir_parms filedir;
int  ofs =  3 * sizeof( u_int16_t );


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t86: test file/dir created by admin\n");

	if (!Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
		
	}

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (!(dir = FPCreateDir(Conn2,vol2, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , name, 0, dbitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, dbitmap);
        filedir.access[2] = 3; /* group read only */
 		if (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , name, dbitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}


	if (FPCreateFile(Conn2, vol2,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , dir, name1, OPENACC_WR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
    else {
		FPCloseFork(Conn,fork);
    }

	if (FPDelete(Conn2, vol2,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn2, vol2,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn2,vol2);

}

/* ---------------------- */
void test87() {
int  dir;
char *name = "t87 dir without .Appledoube";
char *name1 = "t87 file without .Appledoube";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t87: add comment file/dir without .Appledouble\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);strcat(temp,"/.AppleDouble/.Parent");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPAddComment(Conn, vol,  DIRDID_ROOT , name, "Comment for test folder")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	strcpy(temp, Path);strcat(temp,"/");strcat(temp, name);
	strcat(temp,"/.AppleDouble/");strcat(temp,name1);
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPAddComment(Conn, vol,  dir , name1, "Comment for test folder")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test88()
{
int  dir;
char *name = "t88 error setdirparams";
char *name1 = "t88 error setdirparams file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t88: test error setdirparam\n");
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bar", 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
 		if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, DIRDID_ROOT , "new", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, DIRDID_ROOT , "bar", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_BADTYPE) != FPSetDirParms(Conn, vol, dir , name1, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPDelete(Conn, vol,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test89()
{
int  dir;
char *name1 = "t89 error setfilparams dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);



    fprintf(stderr,"===================\n");
    fprintf(stderr,"t89: test set file setfilparam\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "test/2.txt", bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (ntohl(AFPERR_BADTYPE) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_ACCESS) != FPSetFileParams(Conn, vol, DIRDID_ROOT , "test/2.txt", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_ACCESS) != FPSetFileParams(Conn, vol, DIRDID_ROOT , "bar/2.txt", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "bogus folder/test.pdf", bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (ntohl(AFPERR_ACCESS) != FPSetFileParams(Conn, vol, DIRDID_ROOT , "bogus folder/test.pdf", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	bitmap = (1<<FILPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "bogus folder/test.pdf", bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , "bogus folder/test.pdf", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}

}

/* ------------------------- 
 * FIXME we should get ride of this
*/
void test90()
{

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test90: delete a dir without access\n");

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bar", 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_ACCESS) != FPDelete(Conn, vol,  DIRDID_ROOT , "bar")) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- 
*/
void test91()
{
int  dir;
int  dir1;
char *name = "t91 test ID file";
char *name1 = "t91 test ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test91: Resolve ID\n");

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}

	ret = FPDeleteID(Conn, vol,  dir1);
	if (htonl(AFPERR_NOOP) == ret) { 
		fprintf(stderr,"\tFPDeleteID not supported\n");
		return;
	}
	if (htonl(AFPERR_NOID) != ret) { fprintf(stderr,"\tFAILED\n");}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (htonl(AFPERR_NOID) != FPResolveID(Conn, vol, dir1, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_BADTYPE) != FPResolveID(Conn, vol, dir, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateID(Conn,vol, dir, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPDeleteID(Conn, vol, filedir.did)) {
			fprintf(stderr,"\tFAILED\n");
		}
		
	}	
	if ( FPCreateID(Conn,vol, dir, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_EXISTID) != FPCreateID(Conn,vol, dir, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------------------- */
void test92()
{
struct sigaction action;    
struct itimerval    it;     

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t92: test bogus symlink 2\n");

   	it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 15;
    it.it_value.tv_usec = 0;

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	strcpy(temp, Path);strcat(temp,"/dirC");
	/* need to ckeck we have
     * dirC/
          folder
          symlink to dirA/dirB
		  folder2
       an readdir return folder before symlink befor folder2!
     */
    action.sa_handler = alarm_handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGHUP);
    sigaddset(&action.sa_mask, SIGTERM);
    action.sa_flags = SA_RESTART | SA_ONESHOT;
    if ((sigaction(SIGALRM, &action, NULL) < 0) ||
            (setitimer(ITIMER_REAL, &it, NULL) < 0)) {
		fprintf(stderr,"\tFAILED\n");
		return;
    }

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirD", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirC", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "dirD", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }


   	it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
   	setitimer(ITIMER_REAL, &it, NULL);

}


/* -------------------------------------- */
void test93()
{
char *name = "t93 bad enumerate file";
char *name1 = "t93 bad enumerate dir";
int dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t93: enumerate error\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_BADTYPE) != FPEnumerate(Conn, vol,  DIRDID_ROOT , name, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }

	if (htonl(AFPERR_BITMAP) != FPEnumerate(Conn, vol,  DIRDID_ROOT , name1, 0,0)) {
		fprintf(stderr,"\tFAILED\n");
    }

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

}

/* ------------------------- */
void test94()
{
int  dir;
int  dir1;
char *name = "t94 invisible dir";
char *name1 = "t94 sub dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
#if 0
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)| (1<<DIRPBIT_UID) |
	    			(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS);
#endif
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t94: test invisible bit\n");

	if (htonl(AFPERR_ACCESS) != FPDelete(Conn, vol,  DIRDID_ROOT , "")) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir1 = FPCreateDir(Conn,vol, dir , name1))) {fprintf(stderr,"\tFAILED\n");}

	if (htonl( AFPERR_DIRNEMPT) != FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
	sleep(4);
	
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date dir %x\n", filedir.mdate);
	sleep(5);
			
	filedir.attr = ATTRBIT_INVISIBLE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date dir %x\n", filedir.mdate);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
end:
	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test95()
{
int dir;
char *name  = "t95 exchange file";
char *name1 = "t95 new file name";
char *name2 = "Contents";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test95: exchange files\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { fprintf(stderr,"\tFAILED\n");}

	/* sdid bad */
	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, dir, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* name bad */
	if (ntohl(AFPERR_NOID) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* a dir */
	if (ntohl(AFPERR_BADTYPE) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* cname unchdirable */
	if (ntohl(AFPERR_ACCESS) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, "bar/2.txt", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* FIXME second time once bar is in the cache */
	if (ntohl(AFPERR_ACCESS) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, "bar/2.txt", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
#if 0
	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {fprintf(stderr,"\tFAILED\n");}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (fork) {
		if (ntohl(AFPERR_DENYCONF) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork);
	}	
#endif
	/* sdid bad */

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT,dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* name bad */
	if (ntohl(AFPERR_NOID) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* same object */
	if (ntohl(AFPERR_SAMEOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* a dir */
	if (ntohl(AFPERR_BADTYPE) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)){fprintf(stderr,"\tFAILED\n");}

	/* ok */
	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPDelete(Conn, vol,  DIRDID_ROOT, name1);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test96()
{
char *name = "t96 invisible file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t96: test file's invisible bit\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
	sleep(4);
	
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name,bitmap, 0 )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date file %x\n", filedir.mdate);
	sleep(5);
			
	filedir.attr = ATTRBIT_INVISIBLE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0 )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date file %x\n", filedir.mdate);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
end:
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test97()
{
int fork;
int dir;
int dir1;
u_int16_t bitmap = 0;
char *name  = "t97 mswindows é";
char *name1 = "t97 new file name";
char *name2 = "Contents";
char *newv = strdup(Vol);
int  l = strlen(newv);
int  vol1;

	newv[1] = newv[1] +1;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test97: options mswindows\n");
	vol1  = FPOpenVol(Conn, newv);
	if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (!dsi->header.dsi_code)
			FPCloseVol(Conn,vol1);
		return;
	}
	newv[1] = newv[1] -1;
    newv[l -1] += 2;		/* was test5 open test7 */
	vol1  = FPOpenVol(Conn, newv);
	if (dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPEnumerate(Conn, vol1,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }
	if (FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPEnumerate(Conn, vol1,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }

	if (FPDelete(Conn, vol1,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	goto end;
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { fprintf(stderr,"\tFAILED\n");}

	/* sdid bad */
	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	/* cname unchdirable */
	if (ntohl(AFPERR_ACCESS) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, "bar", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* second time once bar is in the cache */
	if (ntohl(AFPERR_ACCESS) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, "bar", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {fprintf(stderr,"\tFAILED\n");}

	/* source is a dir */
	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name2, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (fork) {
		if (ntohl(AFPERR_DENYCONF) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork);
	}	
	/* dvol bad */
	if (ntohl(AFPERR_PARAM) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol +1, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ddid bad */
	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT , vol, dir,  name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ok */
	if (FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPDelete(Conn, vol,  DIRDID_ROOT, name1);
	FPDelete(Conn, vol,  DIRDID_ROOT, name2);
	FPFlush(Conn, vol);
end:
	FPCloseVol(Conn,vol1);
	free(newv);
}

/* ------------------------- */
void test98()
{
int  dir;
char *name = "t98 error setfildirparams";
char *name1 = "t98 error setfildirparams file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t98: test error setdirparam\n");
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bar", 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_ACCESS) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT , "new", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_ACCESS) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT , "bar", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (FPSetFilDirParam(Conn, vol, dir , name1, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT, name1, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (FPSetFilDirParam(Conn, vol, DIRDID_ROOT, name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPDelete(Conn, vol,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ---------------------- 
afp_openfork
afp_createfile
afp_setfilparams
afp_copyfile
afp_createid
afp_exchangefiles
dirlookup
afp_setdirparams
afp_createdir
afp_opendir
afp_getdiracl
afp_setdiracl
afp_openvol
afp_getfildirparams
afp_setfildirparams
afp_delete
afp_moveandrename
afp_enumerate
*/

void cname_test(char *name)
{
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	               (1 << DIRPBIT_ACCESS);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (filedir.pdid != 2) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.pdid, 2 );
	}
	if (strcmp(filedir.lname, name)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name );
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
    }
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (filedir.pdid != 2) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.pdid, 2 );
	}
	if (strcmp(filedir.lname, name)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name );
	}

	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , name)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED)\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);

	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , name)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED)\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
    }

	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , name)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED)\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}
}

void test99()
{

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t99: test new cname\n");
    
    cname_test("bar");
    cname_test("essai permission");
}

/* --------------------- */
void test100()
{
int dir;
char *name = "t100 no obj error";
char *name1 = "t100 no obj error/none";
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR);
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t100: no obj cname error \n");

	if (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol,  DIRDID_ROOT , name1,"essai")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol,  DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol,  DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	filedir.isdir = 1;
	filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 	if (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if ((dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}
	else if (ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
	    
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , name1);
    if (ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  DIRDID_ROOT , name1, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* FIXME afp_errno in file.c */
	ret = FPCreateID(Conn,vol, DIRDID_ROOT, name1);
	if (ret != ntohl(AFPERR_NOOBJ) && ret != ntohl(AFPERR_NOOP) ) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* FIXME ? */
	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED AFPERR_NOOBJ expected\n");
	}

 	if (ntohl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

 	if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

}

/* --------------------- */
void test101()
{
int dir;
char *name = "t101 no obj error";
char *name1 = "bar/none";
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR);
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t101: access error cname \n");

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , name1,"essai")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_ACCESS) != FPGetComment(Conn, vol,  DIRDID_ROOT , name1)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPRemoveComment(Conn, vol,  DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	filedir.isdir = 1;
	filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 	if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if ((dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}
	else if (ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
	    
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , name1);
    if (ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPEnumerate(Conn, vol,  DIRDID_ROOT , name1, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	ret = FPCreateID(Conn,vol, DIRDID_ROOT, name1);
	if (ret != ntohl(AFPERR_ACCESS) && ret != ntohl(AFPERR_NOOP)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

 	if (ntohl(AFPERR_ACCESS) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (ntohl(AFPERR_ACCESS) != FPRename(Conn, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED AFPERR_ACCESS expected\n");
	}

}

/* --------------------- */
void test102()
{
int dir;
char *name = "t102 access error";
char *name1 = "bar";
int ret;
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t102: access error but not cname \n");

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , name1,"essai")) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED)\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , name1)) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED)\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	if (ntohl(AFPERR_ACCESS) != (ret =FPRemoveComment(Conn, vol,  DIRDID_ROOT , name1))) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED)\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	 	if (!ret) {
			FPAddComment(Conn, vol,  DIRDID_ROOT , name1,"essai");
		}
	}

	filedir.isdir = 1;
	filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 	if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if ((dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}
	else if (ntohl(AFPERR_EXIST) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
	    
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , name1);
    if (dir || ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != FPEnumerate(Conn, vol,  DIRDID_ROOT , name1, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED AFPERR_NODIR expected\n");
	}

	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_BADTYPE) != FPCreateID(Conn,vol, DIRDID_ROOT, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOID) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFIXME FAILED AFPERR_NOID expected\n");
	}

 	if (ntohl(AFPERR_BADTYPE) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (FPRename(Conn, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		FPRename(Conn, vol, DIRDID_ROOT, name, name1);
	}

	if (ntohl(AFPERR_ACCESS) != FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { 
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1);	
	}
}

/* --------------------- */
void test103()
{
int dir;
char *name = "t103 did access error";
char *name1 = "bar";
int ret;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<< DIRPBIT_DID);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t103: did access error \n");

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name1, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	dir = filedir.did;

	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  dir , "","essai")) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  dir , "")) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED)\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	if (ntohl(AFPERR_ACCESS) != (ret =FPRemoveComment(Conn, vol,  dir , ""))) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED)\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	 	if (!ret) {
			FPAddComment(Conn, vol,  dir , "","essai");
		}
	}

	filedir.isdir = 1;
	filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
	bitmap = (1<< DIRPBIT_ATTR);
 	if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (ntohl(AFPERR_ACCESS) != FPEnumerate(Conn, vol,  dir , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED AFPERR_NODIR expected\n");
	}

	if (ntohl(AFPERR_BADTYPE) != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, "", name)) {
	    if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_BADTYPE\n");
	    }
	    else {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	if (ntohl(AFPERR_BADTYPE) != FPCreateID(Conn,vol, dir, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOID) != FPExchangeFile(Conn, vol, dir, DIRDID_ROOT, "", name)) {
		fprintf(stderr,"\tFIXME FAILED AFPERR_NOID expected\n");
	}
	ret = FPSetFileParams(Conn, vol, dir , "", bitmap, &filedir);
 	if (ntohl(AFPERR_BADTYPE) != ret) {
 	    if (ret != htonl(AFPERR_PARAM)) {
			fprintf(stderr,"\tFAILED\n");
 	    }
 	    else {
		   fprintf(stderr,"\tWARNING FAILED check bitmap before if it's a dir\n");
		}
 	}
	if (FPRename(Conn, vol, dir, "", name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPRename(Conn, vol, DIRDID_ROOT, name, name1);
	}
	if (ntohl(AFPERR_ACCESS) != FPDelete(Conn, vol,  dir , "")) { 
		if (Quirk) 
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_ACCESS\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}

	if (FPMoveAndRename(Conn, vol, dir, DIRDID_ROOT, "", name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1);	
	}
}

/* --------------------- */
void test104()
{
char *name1 = "t104 dir1";
char *name2 = "t104 dir2";
char *name3 = "t104 dir3";
char *name4 = "t104 dir4";
char *name5 = "t104 file";

int  dir1, dir2, dir3, dir4;

int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t104: cname with trailing 0 \n");

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir2 = FPCreateDir(Conn,vol, dir1 , name2))) {fprintf(stderr,"\tFAILED\n");}


	if (FPCreateFile(Conn, vol,  0, dir2 , name5)) { fprintf(stderr,"\tFAILED\n"); }

	if (!(dir3 = FPCreateDir(Conn,vol, dir2, name3))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir4 = FPCreateDir(Conn,vol, dir3, name4))) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol, dir3, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (filedir.did != dir3) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir3 );
	}
	if (strcmp(filedir.lname, name3)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name3);
	}

    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4///", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.did != dir2) {
			fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir2 );
		}
		if (strcmp(filedir.lname, name2)) {
			fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name2);
		}
		if (filedir.offcnt != 2) {
			fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		}
	}

	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4/", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.did != dir4) {
			fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir4 );
		}
		if (strcmp(filedir.lname, name4)) {
			fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name4);
		}
	}

	if (FPDelete(Conn, vol,  dir3 , name4)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , name3)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , name5)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1 , name2)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1 , ""))    { fprintf(stderr,"\tFAILED\n");}
}

/* --------------------- */
void test105()
{
int dir;
int err;
char *name = "t105 bad did";
char *name1 = "t105 no obj error/none";
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t105: bad DID in call \n");

    dir = 0;
    err = ntohl(AFPERR_PARAM);
	if (err != FPAddComment(Conn, vol, dir, name1,"essai")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (err != FPGetComment(Conn, vol, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (err != FPRemoveComment(Conn, vol, dir  , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	filedir.isdir = 1;
	filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 	if (err != FPSetDirParms(Conn, vol, dir , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if ((dir = FPCreateDir(Conn,vol, dir , name1))) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  dir , name1);
	}
	else if (err != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
	    
	dir = FPOpenDir(Conn,vol, dir , name1);
    if (err  != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (err  != FPEnumerate(Conn, vol,  dir , name1, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (err != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* FIXME afp_errno in file.c */
	if (err != FPCreateID(Conn,vol, dir, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* FIXME ? */
	if (ntohl(AFPERR_PARAM) != FPExchangeFile(Conn, vol, dir, DIRDID_ROOT, name1, name)) {
		if (Quirk) {
			fprintf(stderr,"\tFAILED (IGNORED) netatalk AFPERR_PARAM\n");
		}
		else {
			fprintf(stderr,"\tFAILED\n");
		}
	}

 	if (err  != FPSetFileParams(Conn, vol, dir , name1, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (err != FPRename(Conn, vol, dir, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (err != FPDelete(Conn, vol,  dir , name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}

	if (err != FPMoveAndRename(Conn, vol, dir, DIRDID_ROOT, name1, name)) {
		fprintf(stderr,"\tFAILED\n");
	}

}

/* --------------------- */
void test106()
{
char *name1 = "t104 dir1";
char *name2 = "t104 dir2";
char *name3 = "t104 dir3";
char *name4 = "t104 dir4";
char *name5 = "t104 file";
char *name6 = "t104 dir2_1";

int  dir1, dir2, dir3, dir4;

int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t106: cname with trailing 0 and chdir\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir2 = FPCreateDir(Conn,vol, dir1 , name2))) {fprintf(stderr,"\tFAILED\n");}


	if (FPCreateFile(Conn, vol,  0, dir2 , name5)) { fprintf(stderr,"\tFAILED\n"); }

	if (!(dir3 = FPCreateDir(Conn,vol, dir2, name3))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir4 = FPCreateDir(Conn,vol, dir3, name4))) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol, dir3, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (filedir.did != dir3) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir3 );
	}
	if (strcmp(filedir.lname, name3)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name3);
	}

    sleep(1);
	strcpy(temp, Path);strcat(temp,"/t104 dir1/t104 dir2/t104 dir2_1");
	if (mkdir(temp, 0777)) {
	    fprintf(stderr,"\tFAILED mkdir %s %s\n", temp, strerror(errno));
	}
    
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4///t104 dir2_1//", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.did != dir2) {
			fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir2 );
		}
		if (strcmp(filedir.lname, name2)) {
			fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name2);
		}
		if (filedir.offcnt != 3) {
			fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		}
	}

	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4/", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.did != dir4) {
			fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir4 );
		}
		if (strcmp(filedir.lname, name4)) {
			fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name4);
		}
	}

	if (FPDelete(Conn, vol,  dir3 , name4)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , name3)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , name5)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , name6)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  dir1 , name2)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1 , ""))    { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test107()
{
int  dir;
char *name = "t107 test dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
u_int16_t vol2;


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t107: test dir\n");

    if (!Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
    	return;
    }
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	if (!(dir = FPCreateDir(Conn2,vol2, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = geteuid();
 		if (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , "bar", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = geteuid();
 		if (ntohl(AFPERR_ACCESS) != FPSetDirParms(Conn2, vol2, DIRDID_ROOT , "bar", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	if (FPGetFileDirParams(Conn2, vol2,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.uid = geteuid();
 		if (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
        filedir.access[0] = 0; 
        filedir.access[1] = 7; 
        filedir.access[2] = 7; 
        filedir.access[3] = 7; 
 		if (FPSetDirParms(Conn2, vol2, DIRDID_ROOT , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
        
	}

	if (FPDelete(Conn2, vol2,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn2,vol2);

}

/* ---------------------- */
int get_fid(int dir, char *name)
{
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

	filedir.did = 0;
	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (!filedir.did) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	return filedir.did;
}

/* ---------------------- */
int get_did(int dir, char *name)
{
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<DIRPBIT_DID );
struct afp_filedir_parms filedir;

	filedir.did = 0;
	if (FPGetFileDirParams(Conn, vol,  dir , name, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (!filedir.did) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	return filedir.did;
}

/* ------------------------- */
void write_fork(int dir, char *name, char *txt)
{
int fork;
u_int16_t bitmap = 0;

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPWrite(Conn, fork, 0, strlen(txt), txt, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}	
	FPCloseFork(Conn,fork);
}

/* ------------------------- */
void read_fork(int dir, char *name,int len)
{
int fork;
u_int16_t bitmap = 0;

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	memset(Data, 0, len +1);
	if (FPRead(Conn, fork, 0, len, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseFork(Conn,fork);
}

/* ------------------------- */
void test108()
{
int dir;
char *name  = "t108 exchange file";
char *name1 = "t108 new file name";
char *ndir  = "t108 dir";
int fid_name;
int fid_name1;
int temp;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test108: exchange files\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}
	if (FPCreateFile(Conn, vol,  0, dir, name1)){fprintf(stderr,"\tFAILED\n");}
	fid_name  = get_fid( DIRDID_ROOT , name);
	fid_name1 = get_fid( dir , name1);
	write_fork( DIRDID_ROOT , name, "blue");
	write_fork( dir , name1, "red");
	/* ok */
	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if ((temp = get_fid(DIRDID_ROOT , name)) != fid_name) {
		fprintf(stderr,"\tFAILED %x should be %x\n", temp, fid_name);
	}

	if ((temp = get_fid(dir , name1)) != fid_name1) {
		fprintf(stderr,"\tFAILED %x should be %x\n", temp, fid_name1);
	}
	read_fork( DIRDID_ROOT , name, 3);
	if (strcmp(Data,"red")) {
		fprintf(stderr,"\tFAILED should be red\n");
		
	}
	read_fork( dir , name1, 4);
	if (strcmp(Data,"blue")) {
		fprintf(stderr,"\tFAILED should be blue\n");
	}
	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	FPDelete(Conn, vol,  DIRDID_ROOT, ndir);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test109()
{
int dir;
char *name  = "t109 exchange file cross dev";
char *name1 = "t109 new file name";
char *ndir  = "folder_symlink/dir";
int fid_name;
int fid_name1;
int ret;
struct stat st;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test109: exchange files cross dev\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/folder_symlink");
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}
	if (FPCreateFile(Conn, vol,  0, dir, name1)){fprintf(stderr,"\tFAILED\n");}
	fid_name  = get_fid( DIRDID_ROOT , name);
	fid_name1 = get_fid( dir , name1);
	write_fork( DIRDID_ROOT , name, "blue");
	write_fork( dir , name1, "red");
	
#if 0
	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {fprintf(stderr,"\tFAILED\n");}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (fork) {
		if (ntohl(AFPERR_DENYCONF) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
			fprintf(stderr,"\tFAILED\n");
		}
		FPCloseFork(Conn,fork);
	}	
#endif

	/* ok */
	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	read_fork( DIRDID_ROOT , name, 3);
	if (strcmp(Data,"red")) {
		fprintf(stderr,"\tFAILED should be red\n");
		
	}
	read_fork( dir , name1, 4);
	if (strcmp(Data,"blue")) {
		fprintf(stderr,"\tFAILED should be blue\n");
	}
	if ((ret = get_fid(DIRDID_ROOT , name)) != fid_name) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name);
	}

	if ((ret = get_fid(dir , name1)) != fid_name1) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name1);
	}
		
	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	FPDelete(Conn, vol,  DIRDID_ROOT, ndir);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test110()
{
int fork;
int fork1;
int dir;
u_int16_t bitmap = 0;
char *name  = "t110 exchange file cross dev";
char *name1 = "t110 new file name";
char *ndir  = "folder_symlink/dir";
int fid_name;
int fid_name1;
struct stat st;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test110: error exchange files cross dev\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/folder_symlink");
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}
	if (FPCreateFile(Conn, vol,  0, dir, name1)){fprintf(stderr,"\tFAILED\n");}
	fid_name  = get_fid( DIRDID_ROOT , name);
	fid_name1 = get_fid( dir , name1);
	write_fork( DIRDID_ROOT , name, "blue");
	write_fork( dir , name1, "red");
	
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}	

	if (ntohl(AFPERR_MISC) != FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, name1, OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
	}	

	if (ntohl(AFPERR_MISC) != FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) FPCloseFork(Conn,fork);

	if (ntohl(AFPERR_MISC) != FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (fork1) FPCloseFork(Conn,fork1);
		
	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, ndir)) { fprintf(stderr,"\tFAILED\n");}
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test111()
{
int fork;
int fork1;
int dir;
u_int16_t bitmap = 0;
char *name  = "t111 exchange open files";
char *name1 = "t111 new file name";
char *ndir  = "t111 dir";
int fid_name;
int fid_name1;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test111: exchange open files\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}
	if (FPCreateFile(Conn, vol,  0, dir, name1)){fprintf(stderr,"\tFAILED\n");}
	fid_name  = get_fid( DIRDID_ROOT , name);
	fid_name1 = get_fid( dir , name1);
	write_fork( DIRDID_ROOT , name, "blue");
	write_fork( dir , name1, "red");
	
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}	

	/* ok */
	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	read_fork( DIRDID_ROOT , name, 3);
	if (strcmp(Data,"red")) {
		fprintf(stderr,"\tFAILED should be red\n");
		
	}
	read_fork( dir , name1, 4);
	if (strcmp(Data,"blue")) {
		fprintf(stderr,"\tFAILED should be blue\n");
	}

	if (FPWrite(Conn, fork, 0, 3, "new", 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}	

	read_fork( dir , name1, 3);
	if (strcmp(Data,"new")) {
		fprintf(stderr,"\tFAILED should be new\n");
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
	}	

	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork1) FPCloseFork(Conn,fork1);

	if (FPExchangeFile(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (fork) FPCloseFork(Conn,fork);
	if ((ret = get_fid(DIRDID_ROOT , name)) != fid_name) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name);
	}

	if ((ret = get_fid(dir , name1)) != fid_name1) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name1);
	}
		
	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, ndir)) { fprintf(stderr,"\tFAILED\n");}
	FPFlush(Conn, vol);
}
/* ------------------------- */
void test112()
{
int dir;
char *name  = "t112 move, rename across dev";
char *name1 = "t112 new file name";
char *ndir  = "folder_symlink/dir";
int fid_name;
int ret;
struct stat st;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test112: move and rename file across dev\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/folder_symlink");
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}

	fid_name  = get_fid( DIRDID_ROOT , name);

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if ((ret = get_fid(dir , name1)) != fid_name) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name);
	}

	if (FPDelete(Conn, vol,  dir , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (!FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, ndir)) { fprintf(stderr,"\tFAILED\n");}
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test113()
{
int dir;
int dir1;
char *name  = "t113 move, dir across dev";
char *name1 = "t113 new dir name";
char *odir  = "t113 dir";
char *ndir  = "folder_symlink/dir";
int fid_name;
int ret; 
struct stat st;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test113: move and rename file across dev\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/folder_symlink");
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , odir))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}

	if (FPCreateFile(Conn, vol,  0, dir , name)){fprintf(stderr,"\tFAILED\n");}

	fid_name  = get_fid( dir , name);

	if (FPMoveAndRename(Conn, vol, dir, dir1, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if ((ret = get_fid(dir , name)) != fid_name) {
		fprintf(stderr,"\tFAILED %x should be %x\n", ret, fid_name);
	}

	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}

	if (FPDelete(Conn, vol,  dir, "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1, "")) { fprintf(stderr,"\tFAILED\n");}
	if (!FPDelete(Conn, vol,  DIRDID_ROOT, odir)) { fprintf(stderr,"\tFAILED\n");}
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test114()
{
u_int16_t bitmap = 0;
char *name  = "bogus folder/t114 file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test114: Various errors\n");

	if (ntohl(AFPERR_ACCESS) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
	}
	if (!FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
		fprintf(stderr,"\tFAILED create returned an error but the file is there (DF only)\n");
	}
	
	if (Conn->afp_version < 30) {
		if (ntohl(AFPERR_NOOP) != FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0xffff)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (ntohl(AFPERR_NOOP) != FPEnumerate_ext2(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0xffff)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (ntohl(AFPERR_BITMAP) != FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0xffff)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_BITMAP) != FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
		                    0xffff,
		        (1<< DIRPBIT_ATTR) |
				(1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "new-rw", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != 0) {
			fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		}
	}
	if (ntohl(AFPERR_DIRNEMPT) != FPDelete(Conn, vol,  DIRDID_ROOT, "new-rw")) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test115()
{
u_int16_t dt;

	dt = FPOpenDT(Conn,vol);
	if (FPGetIcon(Conn,  dt, "SITx", "TEXT", 4, 64 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPGetIcon(Conn,  dt, "SITx", "APPL", 1, 256 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetIconInfo(Conn,  dt, "SITx", 1 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NOITEM) != FPGetIconInfo(Conn,  dt, "SITx", 256 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetIconInfo(Conn,  dt, "UNIX", 1 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NOITEM) != FPGetIconInfo(Conn,  dt, "UNIX", 2 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPGetIcon(Conn,  dt, "UNIX", "TEXT", 1, 256 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	FPCloseDT(Conn,dt);

}

/* ------------------------- */
void test116()
{
char *name = "t116 no write file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE);
int fork;


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t116: test file's no write bit\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name,bitmap, 0 )) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
			
	filedir.attr = ATTRBIT_NOWRITE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);

	if (fork || dsi->header.dsi_code != ntohl(AFPERR_OLOCK)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		FPCloseFork(Conn,fork);
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		FPCloseFork(Conn,fork);
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0xffff ,DIRDID_ROOT, name, OPENACC_RD);

	if (fork || dsi->header.dsi_code != ntohl( AFPERR_BITMAP)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		FPCloseFork(Conn,fork);
	}
end:
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
static void test_bytelock_ex(u_int16_t vol2, char *name, int type)
{
int fork;
int fork1;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);
struct flock lock;
int fd;
int ret;
  
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	if (FPSetForkParam(Conn, fork, len , 50)) {fprintf(stderr,"\tFAILED\n");}
	if (FPByteLock(Conn, fork, 0, 0 , 0 , 100)) {fprintf(stderr,"\tFAILED\n");}
	if (FPRead(Conn, fork, 0, 40, Data)) {fprintf(stderr,"\tFAILED\n");}
	if (FPWrite(Conn, fork, 10, 40, Data, 0)) {fprintf(stderr,"\tFAILED\n");}
	fork1 = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork);		
	}

	strcpy(temp, Path);
	strcat(temp,(type == OPENFORK_RSCS)?"/.AppleDouble/":"/");
	strcat(temp, name);

	fd = open(temp, O_RDWR, 0);	
	if (fd >= 0) {
	  	lock.l_start = 60;
    	lock.l_type = F_WRLCK;
	    lock.l_whence = SEEK_SET;
    	lock.l_len = 300;
         
    	if ((ret = fcntl(fd, F_SETLK, &lock)) >= 0 || (errno != EACCES && errno != EAGAIN)) {
    	    if (!ret >= 0) 
    	    	errno = 0;
    		perror("fcntl ");
			fprintf(stderr,"\tFAILED \n");
    	}
    	fcntl(fd, F_UNLCK, &lock);
    	close(fd);
	}
	else {
    	perror("open ");
		fprintf(stderr,"\tFAILED \n");
	}
	fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn2,fork1);		
	}

	FPCloseFork(Conn,fork);
	fork1 = FPOpenFork(Conn2, vol2, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		FPCloseFork(Conn2,fork1);		
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
}

/* -------------------- */
void test117()
{
char *name = "t117 exclusive open DF";
u_int16_t vol2;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t117: test open excl mode\n");
	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	test_bytelock_ex(vol2, name, OPENFORK_DATA);

	name = "t117 exclusive open RF";	
	test_bytelock_ex(vol2, name, OPENFORK_RSCS);

	FPCloseVol(Conn2,vol2);
}

/* ------------------------- */
void test118()
{
char *name = "t118 no delete file";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t118: test file  no delete attribute\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.attr = ATTRBIT_NODELETE | ATTRBIT_SETCLR ;
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
		if (ntohl(AFPERR_OLOCK) != FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
			fprintf(stderr,"\tFAILED\n");
			return;
		}
		filedir.attr = ATTRBIT_NODELETE;
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------- */
void test119()
{
int dir;
char *ndir = "t119 bogus dir";
char *name = "t119 delete read only file";
u_int16_t vol2;
struct flock lock;
int fd;
int err;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t119: delete read only/local fcntl locked file\n");
	if (!Path || !Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	sprintf(temp, "%s/%s/%s", Path, ndir, name);
	fd = open(temp, O_RDWR, 0);	
	if (fd >= 0) {
	  	lock.l_start = 0;
    	lock.l_type = F_WRLCK;
	    lock.l_whence = SEEK_SET;
    	lock.l_len = 0;
         
    	if (fcntl(fd, F_SETLK, &lock) < 0) {
    		perror("fcntl ");
			fprintf(stderr,"\tFAILED \n");
    	}
	}
	else {
    	perror("open ");
		fprintf(stderr,"\tFAILED \n");
	}
	
	if (htonl(AFPERR_BUSY) != (err = FPDelete(Conn, vol,  dir , name))) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (fd >= 0) {
    	fcntl(fd, F_UNLCK, &lock);
    	close(fd);
    }

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);

	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , ndir)) { fprintf(stderr,"\tFAILED\n");}
	FPCloseVol(Conn2,vol2);
}

/* ------------------------- */
void test120()
{
char *name = "t120 test file setfilparam";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);



    fprintf(stderr,"===================\n");
    fprintf(stderr,"t120: test set file setfilparam (create .AppleDouble)\n");

	if (!Path ) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);

		sprintf(temp, "%s/.AppleDouble/%s", Path, name);

		if (unlink(temp)) { 
			fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		}		
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test121()
{
int dir;
char *name = "t121 test dir setdirparam";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_CDATE) | 
					(1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE);



    fprintf(stderr,"===================\n");
    fprintf(stderr,"t121: test set dir setfilparam (create .AppleDouble)\n");

	if (!Path ) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

		sprintf(temp, "%s/%s/.AppleDouble/.Parent", Path, name);

		if (unlink(temp)) { fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));}		
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 		if (htonl(AFPERR_BITMAP) != FPSetDirParms(Conn, vol, DIRDID_ROOT , name, 0xffff, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}

	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test122()
{
char *name = "t122 setfilparam open fork";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int fork;
int fork1;
int ret;
int type = OPENFORK_RSCS;

u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);



    fprintf(stderr,"===================\n");
    fprintf(stderr,"t122: test setfilparam open fork\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, type , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);

 		if ((ret = FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir))) {
			if (Quirk && ret == htonl(AFPERR_PARAM)) {
				fprintf(stderr,"\tFAILED (IGNORED) OS version dependent\n"); 
			}
			else {
				fprintf(stderr,"\tFAILED\n");
			}
 		}
 		if (htonl(AFPERR_BITMAP) != FPSetFileParams(Conn, vol, DIRDID_ROOT , name, 0xffff, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	fork1 = FPOpenFork(Conn, vol, type , 0 ,DIRDID_ROOT, name, OPENACC_RD);
	if (fork1) {
		FPCloseFork(Conn,fork1);
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseFork(Conn,fork);
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------------ */
void test123()
{
int  dir;
int  dir1,dir2, dir3,dir4,dir5;
char *name = "t123 dir1";
char *name1 = "t123 dir1_1";
char *name2 = "t123 dir1_2";
char *name3 = "t123 dir1_3";
char *dest  = "t123 dest";
char *dest1  = "t123 dest_1";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test123: Move And Rename dir with sibling\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir1 = FPCreateDir(Conn,vol, dir , name1))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir2 = FPCreateDir(Conn,vol, dir , name2))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir3 = FPCreateDir(Conn,vol, dir , name3))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir4 = FPCreateDir(Conn,vol, DIRDID_ROOT, dest))) {fprintf(stderr,"\tFAILED\n");}
	if (!(dir5 = FPCreateDir(Conn,vol, dir4 , dest1))) {fprintf(stderr,"\tFAILED\n");}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_CANTMOVE) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "new")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPMoveAndRename(Conn, vol, dir2, dir4, "", "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir5 , "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir3 , "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir2 , "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir1 , "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir4 , "")) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}

}

/* ------------------------------ */
void test124()
{
int  dir;
char *name = "t124 dir1";

int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test124: dangling symlink\n");

	if (!Path ) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {fprintf(stderr,"\tFAILED\n");}

	sprintf(temp, "%s/%s/none", Path, name);
	sprintf(temp1,"%s/%s/link", Path, name);
	
	if (symlink(temp, temp1) < 0) {
		perror("symlink ");
		fprintf(stderr,"\tFAILED\n");
	}
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != 1) {
			fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		}
	}
	if (htonl(AFPERR_NOOBJ) != FPEnumerate(Conn, vol,  dir , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != 0) {
			fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		}
	}

	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test125()
{
char *name  = "t125 Un nom long 0123456789 0123456789 0123456789 0123456789.txt";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test125: mangled name\n");

		fprintf(stderr,"\tFIXME FAILED\n");
		return;
		
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
		 ,
		 0
		)) {
		fprintf(stderr,"\tFAILED\n");
	}

    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name , bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, filedir.lname , bitmap,0)) {
			fprintf(stderr,"\tFIXME FAILED\n");
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test126()
{
char *name  = "t126 Un rep long 0123456789 0123456789 0123456789 0123456789";
int  dir;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test126: mangled name\n");

	if (!Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn, vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
		 ,
		 0
		)) {
		fprintf(stderr,"\tFAILED\n");
	}

    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name , bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, filedir.lname , bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	FPDelete(Conn, vol,  DIRDID_ROOT, name);
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test127()
{
char *name  = "t127 smb afp dir1";
char *name1 = "t127 dir1_1";
int  dir1,dir;
u_int16_t bitmap = (1<<FILPBIT_FNUM );

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test127: dir removed with cnid not updated\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn, vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir1 = FPCreateDir(Conn, vol, dir , name1))) {
		fprintf(stderr,"\tFAILED\n");
	}

	sprintf(temp, "%s/%s/%s/.AppleDouble/.Parent", Path, name, name1);
	if (unlink(temp) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	sprintf(temp, "%s/%s/%s/.AppleDouble", Path, name, name1);
	if (rmdir(temp) <0) {
		fprintf(stderr,"\tFAILED rmdir %s %s\n", temp, strerror(errno));
	}

	sprintf(temp, "%s/%s/%s", Path, name, name1);
	if (rmdir(temp) <0) {
		fprintf(stderr,"\tFAILED rmdir %s %s\n",temp, strerror(errno));
	}

	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	if (FPDelete(Conn,vol, dir,"")) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn,vol, DIRDID_ROOT , name);
	}
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);

	if (htonl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, dir1, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------- */
static void delete_dir(char *name)
{
	sprintf(temp, "%s/%s/.AppleDouble/.Parent", Path, name);
	if (unlink(temp) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	sprintf(temp, "%s/%s/.AppleDouble", Path, name);
	if (rmdir(temp) <0) {
		fprintf(stderr,"\tFAILED rmdir %s %s\n", temp, strerror(errno));
	}

	sprintf(temp, "%s/%s", Path, name);
	if (rmdir(temp) <0) {
		fprintf(stderr,"\tFAILED rmdir %s %s\n", temp, strerror(errno));
	}
}

/* ------------------------- */
void test128()
{
char *name  = "t128 smb afp dir1";
char *name1 = "t128 dir1_1";
int  dir1,dir;
u_int16_t bitmap = (1<<FILPBIT_FNUM );

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test128: dir removed with cnid not updated\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn, vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir1 = FPCreateDir(Conn, vol, dir , name1))) {
		fprintf(stderr,"\tFAILED\n");
	}
	sprintf(temp1, "%s/%s", name, name1);
	delete_dir(temp1);

	if (FPDelete(Conn,vol, dir,"")) {fprintf(stderr,"\tFAILED\n");}

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);

	if (htonl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, dir1, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	if (!FPDelete(Conn,vol, dir,"")) {fprintf(stderr,"\tFAILED\n");}
}

/* -------------------------- */
void test129()
{
int  dir;
char *name = "t129 Resolve ID file";
char *name1 = "t129 Resolve ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test129: Resolve ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);

	if (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test130()
{
int  dir;
char *name = "t130 Delete ID file";
char *name1 = "t130 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test130: Delete ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_BADTYPE ) != FPDeleteID(Conn, vol, dir)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_PARAM ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test131()
{
int  dir;
char *name = "t131 Delete ID file";
char *name1 = "t131 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test131: Resolve ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);
	if (ntohl(AFPERR_NOOBJ ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_NOID ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test132()
{
int  dir;
char *name = "t132 file";
char *name1 = "t132 dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test132: GetFilDirParams \n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (htonl(AFPERR_BITMAP) != FPGetFileDirParams(Conn, vol,  dir , name, 0xffff,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_BITMAP) != FPGetFileDirParams(Conn, vol,  dir , "",0, 0xffff)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test133()
{
int fork;
int dir;
u_int16_t bitmap = 0;
char *name  = "t133 mswindows dir";
char *name1 = "t133 new name.txt";
char *newv = strdup(Vol);
int  l = strlen(newv);
int  vol1;
char *buf = "essai\nsuite\n";
int  len  = strlen(buf);
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test133: options mswindows crlf\n");

    newv[l -1] += 2;		/* was test5 open test7 */
	vol1  = FPOpenVol(Conn, newv);
	if (dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol1 , DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPRename(Conn, vol1, dir, "", "")) {fprintf(stderr,"\tFAILED\n");}
	}

	if (ntohl(AFPERR_PARAM) != FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , " PRN")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_PARAM) != FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , "P*RN")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_EXIST) != FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , "icon")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , "icone")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRename(Conn, vol1, DIRDID_ROOT, "icone", "icone")) {fprintf(stderr,"\tFAILED\n");}

	if (ntohl(AFPERR_PARAM) != FPRename(Conn, vol1, DIRDID_ROOT, "icone", " PRN")) {fprintf(stderr,"\tFAILED\n");}

	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol1, DIRDID_ROOT, DIRDID_ROOT, "icone", " PRN")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol1,  DIRDID_ROOT , "icone")) { fprintf(stderr,"\tFAILED\n");}

	/* -------------------- */
	if (FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol1, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1, OPENACC_WR | OPENACC_RD);
	if (!fork) {
	}
	else {
		if (FPWrite(Conn, fork, 0, len, buf, 0 )) {fprintf(stderr,"\tFAILED\n");}
		if (FPRead(Conn, fork, 0, len, Data)) {fprintf(stderr,"\tFAILED\n");}

		FPCloseFork(Conn, fork);
	}
		
	if (FPDelete(Conn, vol1,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}

	if (dir) {
		if (FPDelete(Conn, vol1,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
	}

end:
	FPCloseVol(Conn,vol1);
	free(newv);
}

/* ------------------------- */
void test134()
{
int dir;
u_int16_t bitmap = 0;
char *name  = "t134 Copy file";
char *name1 = "t134 new file name";
char *name2 = "bogus folder";
struct afp_filedir_parms filedir;
int  ofs =  3 * sizeof( u_int16_t );
int err;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test134: Copy file errors (right access)\n");

	bitmap = (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |(1 << DIRPBIT_ACCESS);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name2, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir = filedir.did;
	
	if (htonl(AFPERR_ACCESS) != (err = FPCopyFile(Conn, vol, DIRDID_ROOT, vol , dir, name, name1))) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) !=  FPDelete(Conn, vol,  dir, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_ACCESS) != (err = FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name1))) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  dir, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCopyFile(Conn, vol, dir, vol , DIRDID_ROOT, "test.pdf", name1) ) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPDelete(Conn, vol,  DIRDID_ROOT, name1) ) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test135()
{
int fork;
int dir;
u_int16_t bitmap = 0;
char *name  = "t135 move, rename across dev";
char *name1 = "t135 new file name";
char *ndir  = "folder_symlink/dir";
struct stat st;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test135: move and rename file across dev\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	strcpy(temp, Path);strcat(temp,"/folder_symlink");
	if (stat(temp, &st)) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){fprintf(stderr,"\tFAILED\n");}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {fprintf(stderr,"\tFAILED\n");}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_OLOCK) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (fork && FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir, "")) { 
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  dir, name);
		FPDelete(Conn, vol,  dir, "");
	}
}

/* ------------------------- */
void test136()
{
int dir;
char *name  = "t136 move, rename ";
char *name1 = "t136 dir/new file name";
char *name2 = "new file name";
char *ndir  = "t136 dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test136: move and rename\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	sprintf(temp, "%s/%s", Path, ndir);
	if (mkdir(temp, 0777)) {
	    fprintf(stderr,"\tFAILED mkdir %s %s\n", temp, strerror(errno));
	}

	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	dir = get_did(DIRDID_ROOT, ndir);

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) { fprintf(stderr,"\tFAILED\n");}
	if (!FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir, "")) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test137()
{
int fork;
int dir;
u_int16_t bitmap = 0;
char *name  = "t137 move, rename ";
char *name1 = "t137 dir/new file name";
char *name2 = "new file name";
char *ndir  = "t137 dir";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test137: move and rename\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	sprintf(temp, "%s/%s", Path, ndir);
	if (mkdir(temp, 0777)) {
		fprintf(stderr,"\tFAILED mkdir %s %s\n", temp, strerror(errno));
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	dir = get_did(DIRDID_ROOT, ndir);

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (fork && FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) { fprintf(stderr,"\tFAILED\n");}
	if (!FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir, "")) { fprintf(stderr,"\tFAILED\n");}
}

/* -------------------------- */
void test138()
{
int  dir;
char *name = "t138 file";
char *name1 = "t138 dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);


    fprintf(stderr,"===================\n");
    fprintf(stderr,"test138: Move And Rename \n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPMoveAndRename(Conn, vol, dir, DIRDID_ROOT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetFileDirParams(Conn, vol,  dir , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.access[1] = 3; 
        filedir.access[2] = 3; 
        filedir.access[3] = 3; 
 		if (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	if (ntohl(AFPERR_ACCESS) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
    filedir.access[1] = 7; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	if (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
}

/* -------------------------- */
void test139()
{
int  dir;
char *name = "t139 file";
char *name1 = "t139 dir";


    fprintf(stderr,"===================\n");
    fprintf(stderr,"test139: Move And Rename \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {fprintf(stderr,"\tFAILED\n");}
	
	sprintf(temp, "%s/.AppleDouble/%s", Path, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test140()
{
int dir;
u_int16_t bitmap = 0;
char *name  = "t140 Exchange file";
char *name1 = "test.pdf";
char *name2 = "bogus folder";
struct afp_filedir_parms filedir;
int  ofs =  3 * sizeof( u_int16_t );

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test140: Exchange file errors (right access)\n");

	bitmap = (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |(1 << DIRPBIT_ACCESS);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name2, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	dir = filedir.did;
	if (ntohl(AFPERR_ACCESS) != FPExchangeFile(Conn, vol, DIRDID_ROOT,dir,  name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test141()
{
int fork;
u_int16_t bitmap = 0;
char *name  = "t140 setforkmode file";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test141: Setforkmode error\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_BITMAP) != FPSetForkParam(Conn, fork, (1<<FILPBIT_RFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (Conn->afp_version < 30) {
		if (ntohl(AFPERR_BITMAP) != FPSetForkParam(Conn, fork, (1<<FILPBIT_EXTDFLEN), 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	if (FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_BITMAP) != FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (Conn->afp_version < 30) {
		if (ntohl(AFPERR_BITMAP) != FPSetForkParam(Conn, fork, (1<<FILPBIT_EXTRFLEN), 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	if (FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test142()
{
int fork;
u_int16_t bitmap = 0;
char *name  = "dropbox/toto.txt";
u_int16_t vol2;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test142: -wx folder\n");

	if (!Conn2) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (fork || dsi->header.dsi_code != ntohl(AFPERR_LOCK)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (fork || dsi->header.dsi_code != ntohl(AFPERR_LOCK)) {
		fprintf(stderr,"\tFIXME? FAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	/* ------------------ */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name , OPENACC_WR );
	if (!fork ) {
		fprintf(stderr,"\tFIXME? FAILED\n");
	}
	else if (FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name , OPENACC_WR );
	if (!fork ) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ------------------ */	
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	fork = FPOpenFork(Conn2, vol2, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (fork || dsi2->header.dsi_code != ntohl(AFPERR_ACCESS)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn2, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	fork = FPOpenFork(Conn2, vol2, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (fork || dsi2->header.dsi_code != ntohl(AFPERR_ACCESS)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn2, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	/* ------------------ */
	fork = FPOpenFork(Conn2, vol2, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPCloseFork(Conn2, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn2, vol2, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_WR );
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (FPCloseFork(Conn2, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* ------------------ */
	fork = FPOpenFork(Conn2, vol2, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD );
	if (fork || dsi2->header.dsi_code != ntohl(AFPERR_ACCESS)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn2, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	fork = FPOpenFork(Conn2, vol2, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_RD );
	if (fork || dsi2->header.dsi_code != ntohl(AFPERR_ACCESS)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork) {
		if (FPCloseFork(Conn2, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}


	FPCloseVol(Conn2,vol2);
	
}

/* --------------------------------- */
static int is_there(int did, char *name)
{
	return FPGetFileDirParams(Conn, vol,  did, name, 
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
	         ,
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
		);
}

/* ------------------------- */
void test143()
{
int id = getpid();
char *ndir;
int dir;
int i,maxi = 0;
int fork;
char *data;
int nowrite;
int numread = 2; /*469;*/

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test143: LanTest\n");
	sprintf(temp,"LanTest.tmp.32.-%d", id);
	
	ndir = strdup(temp);
	data = calloc(1, 65536);
	if (ntohl(AFPERR_NOOBJ) != is_there(DIRDID_ROOT, ndir)) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, "", 0
	         , (1<< DIRPBIT_DID) )) {
		fprintf(stderr,"\tFAILED\n");
		goto fin;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_NOOBJ) != is_there(dir, "File.big")) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "", 0
	         , (1<< DIRPBIT_DID) )) {
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}
	if (FPCreateFile(Conn, vol,  0, dir , "File.big")){
		fprintf(stderr,"\tFAILED\n");
		goto fin1;
	}
	/* --------------- */
	for (i=1; i <= 100; i++) {
		sprintf(temp, "File.small%d", i);
		if (ntohl(AFPERR_NOOBJ) != is_there(dir, temp)) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
				fprintf(stderr,"\tFAILED\n");
			}
		    if (FPCreateFile(Conn, vol,  0, dir , temp)){
				fprintf(stderr,"\tFAILED\n");
				break;
			}
			if (is_there(dir, temp)) {
				fprintf(stderr,"\tFAILED\n");
			}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	    	     	(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
		    	     (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	            , 0)) {
				fprintf(stderr,"\tFAILED\n");
			}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	    	     	(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
	    	     	 (1<< DIRPBIT_ATTR)|(1<<DIRPBIT_BDATE)|
		    	     (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	            , 0)) {
				fprintf(stderr,"\tFAILED\n");
			}
			fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			            (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
			if (!fork) {
				fprintf(stderr,"\tFAILED\n");
			}
			else {
				if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))
				) {
					fprintf(stderr,"\tFAILED\n");
				}
				if (FPWrite(Conn, fork, 0, 20480, data, 0 )) {
					fprintf(stderr,"\tFAILED\n");
				}
				if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
			}
		}
		maxi = i;
	}
	if (FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.small%d", i);
		if (is_there(dir, temp)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
			fprintf(stderr,"\tFAILED\n");
		}
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342, dir, temp, OPENACC_RD);
		if (!fork) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fprintf(stderr,"\tFAILED\n");}
			if (FPRead(Conn, fork, 0, 512, data)) {fprintf(stderr,"\tFAILED\n");}
			if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		}
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_DWR);
		if (!fork) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fprintf(stderr,"\tFAILED\n");}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
				fprintf(stderr,"\tFAILED\n");
			}
			if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		}
	}
	/* ---------------- */
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.small%d", i);
		if (is_there(dir, temp)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0, (1<< FILPBIT_FNUM) )) {
			fprintf(stderr,"\tFAILED\n");
		}

		if (FPDelete(Conn, vol,  dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	}
	    
 	if (FPGetVolParam(Conn, vol, (1 << VOLPBIT_MDATE )|(1 << VOLPBIT_XBFREE))) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* --------------- */
	strcpy(temp, "File.big");
	if (is_there(dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {fprintf(stderr,"\tFAILED\n");}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
		fprintf(stderr,"\tFAILED\n");
	}
	nowrite = 0;
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			            (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))) {
			fprintf(stderr,"\tFAILED\n");
		}
		for (i=0; i <= numread ; i++) {
			if (FPWrite(Conn, fork, i*65536, 65536, data, 0 )) {
				fprintf(stderr,"\tFAILED\n");
				nowrite = 1;
			}
		}
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
	}

	if (is_there(dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d, 0)) {fprintf(stderr,"\tFAILED\n");}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fprintf(stderr,"\tFAILED\n");}
		if (FPRead(Conn, fork, 0, 512, data)) {fprintf(stderr,"\tFAILED\n");}
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
	}
	if (!nowrite) {	
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_DWR);
		if (!fork) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fprintf(stderr,"\tFAILED\n");}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
				fprintf(stderr,"\tFAILED\n");
			}
			for (i=0; i <= numread ; i++) {
				if (FPRead(Conn, fork, i*65536, 65536, data)) {
					fprintf(stderr,"\tFAILED\n");
				}
			}
			if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		}
	}
	/* --------------- */
	strcpy(temp, "File.lock");
	
	if (ntohl(AFPERR_NOOBJ) != is_there(dir, temp)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, dir , temp)){fprintf(stderr,"\tFAILED\n");}

	if (is_there(dir, temp)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	 		(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
		    (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	        , 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			(1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))
				) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 
				(1<< DIRPBIT_ATTR)|(1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|
				(1<<FILPBIT_FNUM)|
		 		(1<<FILPBIT_FINFO)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
		        , 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPWrite(Conn, fork, 0, 40000, data, 0 )) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
	}
	/* -------------- */
	if (is_there(dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f)) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fprintf(stderr,"\tFAILED\n");}
		if (FPRead(Conn, fork, 0, 512, data)) {fprintf(stderr,"\tFAILED\n");}
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		/* ----------------- */
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_WR);

		if (!fork) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fprintf(stderr,"\tFAILED\n");}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d, 0)) {
				fprintf(stderr,"\tFAILED\n");
			}
#if 0
			for (j = 0; j <= 10; j++) {
				for (i = 0;i <= 390; i += 10) {
					if (FPByteLock(Conn, fork, 0, 0 , i , 10)) {fprintf(stderr,"\tFAILED\n");}
				}	
				for (i = 390;i >= 0; i -= 10) {
					if (FPByteLock(Conn, fork, 0, 1 , i , 10)) {fprintf(stderr,"\tFAILED\n");}
				}
			}	
#endif			
			if (is_there(dir, temp)) {fprintf(stderr,"\tFAILED\n");}
			if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
			if (FPDelete(Conn, vol,  dir, "File.lock")) {fprintf(stderr,"\tFAILED\n");}
		}
	}		

	/* --------------- */
	for (i=1; i <= 320; i++) {
		sprintf(temp, "File.0k%d", i);
		if (ntohl(AFPERR_NOOBJ) != is_there(dir, temp)) {
			fprintf(stderr,"\tFAILED\n");
		}
		else {
			if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
				fprintf(stderr,"\tFAILED\n");
			}
		    if (FPCreateFile(Conn, vol,  0, dir , temp)){
				fprintf(stderr,"\tFAILED\n");
				break;
			}
		}
		maxi = i;
	}
	if (FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---------------- */
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.0k%d", i);
		if (is_there(dir, temp)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0, (1<< FILPBIT_FNUM) )) {
			fprintf(stderr,"\tFAILED\n");
		}

		if (FPDelete(Conn, vol,  dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	}
	    
	
fin1:
	if (FPDelete(Conn, vol,  dir, "File.big")) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir, "")) {fprintf(stderr,"\tFAILED\n");}
fin:
	free(ndir);
	free(data);
}

/* ------------------------- */
void test144()
{
int forkd[128];
int fork;
int i;
char *ndir = "test144";
int dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test144: lot of open fork\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* --------------- */
	for (i=0; i < 128; i++) {
		sprintf(temp, "File.small%d", i);
		if (FPCreateFile(Conn, vol,  0, dir , temp)){
			fprintf(stderr,"\tFAILED\n");
		}
		fork = forkd[i] = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			            (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork) {fprintf(stderr,"\tFAILED\n");}
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))
			) {
				fprintf(stderr,"\tFAILED\n");
		}
		if (FPWrite(Conn, fork, 0, 20480, Data, 0 )) {
				fprintf(stderr,"\tFAILED\n");
		}
	}
	for (i=0; i < 128; i++) {
		fork = forkd[i];
		sprintf(temp, "File.small%d", i);
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		if (FPDelete(Conn, vol,  dir, temp)) {fprintf(stderr,"\tFAILED\n");}
	}

	if (FPDelete(Conn, vol,  dir, "")) {fprintf(stderr,"\tFAILED\n");}
}

/* -------------------------- */
void test145()
{
u_int16_t bitmap = 0;
int fork;
char *name = "toto.txt";

    fprintf(stderr,"===================\n");
	fprintf(stderr, "test145: open deny mode/ fork attrib\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, 0);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}	
	if (FPCloseFork(Conn,fork)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test146()
{
int fork,fork1;
int  dir;
char *name = "t146 file";
char *name1 = "t146 dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
u_int16_t vol2;


    fprintf(stderr,"===================\n");
	fprintf(stderr, "test146: delete read only open file\n");

	if (!Conn2|| !Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.access[1] = 3; /* everyone */
        filedir.access[2] = 3; /* group */
        filedir.access[3] = 7; /* owner */
 		if (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	/* ------------------ */	
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (ntohl(AFPERR_ACCESS) != FPDelete(Conn2, vol2,  dir , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_BUSY) != FPDelete(Conn2, vol2,  dir , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}

    filedir.access[1] = 3; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	if (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
	if (ntohl(AFPERR_BUSY) != FPDelete(Conn2, vol2,  dir , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	sprintf(temp,"%s/%s/.AppleDouble/%s", Path, name1, name);
	if (chmod(temp, 0644) <0) {
		fprintf(stderr,"\tFAILED chmod %s %s\n", temp, strerror(errno));
	}
	if (ntohl(AFPERR_BUSY) != FPDelete(Conn2, vol2,  dir , name)) { 
		fprintf(stderr,"\tFIXME FAILED open but deleted\n");
		if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
		if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
		if (!fork) {fprintf(stderr,"\tFAILED\n");}
	}

	if (FPCloseFork(Conn,fork)) {fprintf(stderr,"\tFAILED\n");}
	fork1 = FPOpenFork(Conn2, vol2, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_BUSY) != FPDelete(Conn, vol,  dir , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCloseFork(Conn2,fork1)) {fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  dir , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , "")) { fprintf(stderr,"\tFAILED\n");}
	FPCloseVol(Conn2,vol2);
/*
int FPEnumerateFull(DSI *Conn, u_int16_t vol, u_int16_t sindex, u_int16_t reqcnt, u_int16_t size, 
    int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);
*/
}

/* ------------------------- */
void test147()
{
char *name = "t147 new file name\252 a";

char *newv = strdup(Vol);
int  l = strlen(newv);
int  vol1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test147: options mswindows\n");
    newv[l -1] += 2;		/* was test5 open test7 */
	vol1  = FPOpenVol(Conn, newv);
	if (dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPEnumerate(Conn, vol1,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }
	if (FPCreateFile(Conn, vol1,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
#if 0
	if (FPEnumerate(Conn, vol1,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
    }

	if (FPDelete(Conn, vol1,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	goto end;
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name2))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name2)) { fprintf(stderr,"\tFAILED\n");}

	/* sdid bad */
	if (ntohl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, dir, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
#endif

	FPDelete(Conn, vol1,  DIRDID_ROOT, name);
	FPCloseVol(Conn,vol1);
	free(newv);
}

/* --------------------- */

void test148()
{
int fork;
int fork1;
u_int16_t bitmap = 0;
char *name = "t148 test";
u_int32_t rep;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test148: AFP 3.0 FPWriteExt\n");

	if (FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	                        
	if (FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 0, (1 << DIRPBIT_PDINFO ))) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

    /* > 2 Gb */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, "toto");
		return;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn,fork1);
		FPDelete(Conn, vol,  DIRDID_ROOT, "toto");
		return;
	}

	memset(w_buf, 'b', BUF_S);
	if (FPWrite_ext(Conn, fork, 0, 1714, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		memcpy(&rep, dsi->commands, sizeof(rep));
		if (rep) {
			fprintf(stderr,"\tFAILED size %d\n", rep);
		}	
		memcpy(&rep, dsi->commands +sizeof(rep), sizeof(rep));
		if (ntohl(rep) != 1714) {
			fprintf(stderr,"\tFAILED size %d\n", ntohl(rep));
		}	
	}

	if (FPWrite_ext(Conn, fork1, 0, 1714, w_buf, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		memcpy(&rep, dsi->commands, sizeof(rep));
		if (rep) {
			fprintf(stderr,"\tFAILED size %d\n", rep);
		}	
		memcpy(&rep, dsi->commands +sizeof(rep), sizeof(rep));
		if (ntohl(rep) != 1714) {
			fprintf(stderr,"\tFAILED size %d\n", ntohl(rep));
		}	
	}
	
	if (ntohl(AFPERR_EOF) != FPRead_ext(Conn, fork, 0, 2000, Data) ||
		dsi->header.dsi_len != htonl(1714)
		) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);
	                        
	FPCloseFork(Conn,fork);
	FPCloseFork(Conn,fork1);

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		fprintf(stderr,"\tFAILED\n");
	}	
}

/* ------------------------- */
void test149()
{
u_int16_t bitmap = 0;
char *name  = "bogus folder/t149 file";
int fork, fork1, fork2, fork3;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test149: Error when no write access to .AppleDouble\n");

	if (ntohl(AFPERR_ACCESS) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD);
	if (fork) {
		fprintf(stderr,"\tFIXME FAILED create failed but it's there\n");
		fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_RD);
		if (!fork1) {
			fprintf(stderr,"\tFAILED\n");
		}
		fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
		if (!fork2) {
			fprintf(stderr,"\tFAILED\n");
		}

		fork3 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
		if (fork3) {
			fprintf(stderr,"\tFAILED\n");
		}
		else if (dsi->header.dsi_code != ntohl(AFPERR_LOCK)) {
			fprintf(stderr,"\tFAILED\n");
		}

		if (fork && FPCloseFork(Conn, fork)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (fork1 && FPCloseFork(Conn, fork1)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (fork2 && FPCloseFork(Conn, fork2)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (fork3 && FPCloseFork(Conn, fork3)) {
			fprintf(stderr,"\tFAILED\n");
		}

		if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
			fprintf(stderr,"\tFAILED\n");
		}
	}
}

/* ------------------------- */
void test150()
{
u_int16_t bitmap = 0;
char *name  = "bogus folder/test.pdf";
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test150: Error when no write access to .AppleDouble\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {
		fprintf(stderr,"\tFAILED\n");
	}

	bitmap = 1 << FILPBIT_DFLEN;
	if (FPGetForkParam(Conn, fork, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork, 0, 2000, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPRead(Conn, fork, 0, 2000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		if (FPRead(Conn, fork, 0, 2000, Data)) {
			fprintf(stderr,"\tFAILED\n");
		}
		if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test151()
{
u_int16_t bitmap = 0;
char *name1  = "t151 file";
int fork;
int i;
int maxf;
int ok = 0;
static int forkt[2049];

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test151: Error when no write access to .AppleDouble\n");
    fprintf(stderr,"test151: too many open files\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)){ 
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	maxf = 0;
	for (i = 0; i < 2048; i++) {
		forkt[i] = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1 ,OPENACC_RD|OPENACC_DWR);
		if (!forkt[i]) {
			maxf = i;
			ok = 1;
			if (dsi->header.dsi_code != ntohl(AFPERR_NFILE)) {
				fprintf(stderr,"\tFAILED\n");
			}
			break;
		}
	}
	if (!ok) {
		fprintf(stderr,"\tFAILED NOT TESTED\n");
		maxf = 2048;
	}
	for (i = 0; i < maxf; i++) {
		if (forkt[i] && FPCloseFork(Conn, forkt[i])) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	if (!ok) {
		goto end;
	}
	for (i = 0; i < maxf; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1 ,OPENACC_RD|OPENACC_WR);
		if (!fork) {
			fprintf(stderr,"\tFAILED after %d OpenFork\n", i);
			break;
		}
		if (fork && FPCloseFork(Conn, fork)) {
			fprintf(stderr,"\tFAILED\n");
			break;
		}
	}
end:
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test152()
{
u_int16_t bitmap = 0;
char *name  = "bogus folder/test.pdf";
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test152: Error when no write access to .AppleDouble\n");

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
	if (fork) {
		fprintf(stderr,"\tFAILED\n");
		FPCloseFork(Conn, fork);
		return;
	}
	
	if (dsi->header.dsi_code != ntohl(AFPERR_LOCK)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test153()
{
char *name  = "t153.txt";
u_int16_t bitmap = 0;
int fork;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test153: open data fork without ressource fork\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	sprintf(temp, "%s/.AppleDouble/%s", Path, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s\n", temp);
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);

	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}		
	if (FPWrite(Conn, fork, 0, 2000, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRead(Conn, fork, 0, 2000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test154()
{
char *name  = "t154.txt";
u_int16_t bitmap = 0;
int fork = 0, fork1 = 0;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test154: no adouble create/open/write file\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	sprintf(temp, "%s/.AppleDouble/%s", Path, name);
	if (!unlink(temp)) {
	    if (Noadouble) {
			fprintf(stderr,"\tFAILED unlink %s was there\n", temp);
			sprintf(temp, "%s/.AppleDouble", Path);
			if (rmdir(temp)) {
				fprintf(stderr,"\tFAILED rmdir %s  %s\n", temp, strerror(errno));
			}
		}
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, 
		OPENACC_WR | OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, 
		OPENACC_WR | OPENACC_RD| OPENACC_DWR| OPENACC_DRD);

	if (!fork1) {
		fprintf(stderr,"\tFAILED\n");
		goto end;
	}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 19696)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPFlushFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_RFLEN), 2336)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPFlushFork(Conn, fork1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork, 0, 19696, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPWrite(Conn, fork1, 0, 2336, Data, 0 )) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetForkParam(Conn, fork1, 0x442)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPRead(Conn, fork, 0, 2000, Data)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 19696)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}
	fork = 0;
	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_RFLEN), 2336)) {
		fprintf(stderr,"\tFAILED\n");
	}

end:
	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (fork1 && FPCloseFork(Conn, fork1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (Noadouble) {
		sprintf(temp, "%s/.AppleDouble", Path);
		if (rmdir(temp)) {
			fprintf(stderr,"\tFAILED rmdir %s  %s\n", temp, strerror(errno));
		}
	}

}
/* ------------------------- */
void test155()
{
char *name  = "t155.doc";
char *name1 = "t155 new.oc";
u_int16_t bitmap = 0;
int fork = 0, fork1;
int fork2, fork3;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test155: Word save NOT USED\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_RD);
	FPCloseFork(Conn, fork);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_RD);
	FPCloseFork(Conn, fork);
	/* 1024 */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_RD|OPENACC_WR);
	/* 1280 */
	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_RD|OPENACC_WR);
		
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)){ 
		fprintf(stderr,"\tFAILED\n");
	}

	/* 2048 */
	fork2 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name1, OPENACC_RD|OPENACC_WR);

	/* 2304 */
	fork3 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_RD);
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name1)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test156()
{
u_int16_t bitmap = 0;
char *name  = "bogus folder/test.pdf";
int fork;
int i;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test156: Open fork with no write access to .AppleDouble\n");
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);

	if (!fork) {
		fprintf(stderr,"\tFAILED after %d OpenFork\n", i);
		return;
	}
	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD|OPENACC_WR);
	if (!fork) {
		fprintf(stderr,"\tFAILED after %d OpenFork\n", i);
		return;
	}
	if (fork && FPCloseFork(Conn, fork)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test157()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test157: bad .AppleDouble ressource fork\n");

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}

}

/* ------------------------- */
void test158()
{
char *name  = "t158 old file name";
char *name1 = "t158 new file name";

	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test158: copyFile dest exist\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* sdid bad */
	if (ntohl(AFPERR_EXIST) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}

}

/* ------------------------- */
void test159()
{
char *name  = "test.txt";
u_int16_t bitmap;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;

	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test159: AppleDouble V1 to V2\n");
    bitmap = (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap, 0)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
}

/* ------------------------- */
void test160()
{
char *name  = "OpenFolderListDF\r";
char ndir[4];
char nfile[6];

u_int16_t bitmap;
int fork;
int dir;
int  ofs =  3 * sizeof( u_int16_t );
int offcnt;
struct afp_filedir_parms filedir;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test160: utf8 name with \\r and Mac Code\n");
    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);

    fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name,0x33);
    if (fork || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn, fork);
	}
	if (Conn->afp_version >= 30) {
	    return;
	}
	/* ------------------ */
	
    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		offcnt = filedir.offcnt;
	}

	ndir[0] = 'a';
	ndir[1] = 0xaa;
	ndir[2] = 'e';
	ndir[3] = 0;
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != offcnt +1) {
			fprintf(stderr,"\tFAILED is %d want %d\n", filedir.offcnt, offcnt +1);
		}
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != offcnt +1) {
			fprintf(stderr,"\tFAILED is %d want %d\n", filedir.offcnt, offcnt +1);
		}
	}
	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	    if (strcmp(filedir.lname, ndir)) {
		    fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, ndir );
	    }
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , ndir)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	/* ----------------- */
	strcpy(nfile, "e.rtf");
	nfile[0] = 0x8e;         /* é.rtf */
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, nfile)) {
		    fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, nfile);
	    }
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , nfile)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test161()
{
char *nfile = ".Essai";
char *ndir  = ".Dir";

u_int16_t bitmap;
int dir;
int  ofs =  3 * sizeof( u_int16_t );
int offcnt;
struct afp_filedir_parms filedir;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test161: leading . in name UTF8 and unix\n");

    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		offcnt = filedir.offcnt;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != offcnt +1) {
			fprintf(stderr,"\tFAILED is %d want %d\n", filedir.offcnt, offcnt +1);
		}
	}
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (filedir.offcnt != offcnt +1) {
			fprintf(stderr,"\tFAILED is %d want %d\n", filedir.offcnt, offcnt +1);
		}
	}
	if (FPGetFileDirParams(Conn, vol, dir, "", 0, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	    if (strcmp(filedir.lname, ndir)) {
		    fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, ndir );
	    }
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , ndir)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	/* ----------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, nfile)) {
		    fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, nfile);
	    }
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , nfile)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}	

/* ------------------------- */
void test162()
{
char ndir[4];

int dir;
	
	if (Conn->afp_version < 30) {
	    return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test162: illegal UTF8 name\n");

    ndir[0] = 'e';
    ndir[1] = 0xc3;
    ndir[2] = 0;
	if ((dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		fprintf(stderr,"\tFAILED\n");
		if (FPDelete(Conn, vol,  DIRDID_ROOT , ndir)) { 
			fprintf(stderr,"\tFAILED\n");
		}
	}
	else if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {	
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test163()
{
char *name  = "Un nom long 0123456789 0123456789 0123456789 0123456789.txt";
char *name1 = "Un nom long 0123456789 0123456789 0123456789 0123456790.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test163: mangled names\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
}
/* ------------------------- */
void test164()
{
u_int16_t bitmap = 0;
char *name   = "Un nom long 0123456789 0123456789 0123456789 0123456789.txt";
char *name1  = "Un nom long 0123456789 ~000.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test164: mangled names\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);

    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME);

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
		if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
		if (FPDelete(Conn, vol,  DIRDID_ROOT , filedir.lname)) {
			fprintf(stderr,"\tFIXME FAILED lname\n");
			if (FPDelete(Conn, vol, DIRDID_ROOT, name1)) {
				fprintf(stderr,"\tFAILED mangle\n");
				FPDelete(Conn, vol,  DIRDID_ROOT , name);
			}	
		}
	}
}

/* ------------------------- */
void test165()
{
char *name  = "t165.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test165: no adouble getcomment\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	sprintf(temp, "%s/.AppleDouble/%s", Path, name);
	if (!unlink(temp)) {
	    if (Noadouble) {
			fprintf(stderr,"\tFAILED unlink %s was there\n", temp);
			sprintf(temp, "%s/.AppleDouble", Path);
			if (rmdir(temp)) {
				fprintf(stderr,"\tFAILED rmdir %s  %s\n", temp, strerror(errno));
			}
		}
	}
	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , name,"essai")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
	if (Noadouble) {
		sprintf(temp, "%s/.AppleDouble", Path);
		if (!rmdir(temp)) {
			fprintf(stderr,"\tFAILED rmdir %s  no error\n", temp);
		}
	}
}

/* ------------------------- */
void test166()
{
char nfile[8];
u_int16_t bitmap;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;

	if (Conn->afp_version < 30) {
	    return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test166: utf8 precompose decompose\n");
    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);
	strcpy(nfile, "ee.rtf");
	nfile[0] = 0xc3;         /* é.rtf */
	nfile[1] = 0xa9;
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME)|(1 << FILPBIT_PDINFO );

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, "\216.rtf")) {
		    fprintf(stderr,"\tFAILED %s should be \\216.rtf\n",filedir.lname);
	    }
	}
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);

	strcpy(nfile, "eee.rtf");
	nfile[1] = 0xcc;         /* é.rtf */
	nfile[2] = 0x81;
	if (FPDelete(Conn, vol,  DIRDID_ROOT , nfile)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test167()
{
char nfile[8];
u_int16_t bitmap;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;

	if (Conn->afp_version < 30) {
	    return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test167: utf8 precompose decompose\n");
    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);
	strcpy(nfile, "laa");
	nfile[1] = 0xc3;         /* là */
	nfile[2] = 0xa0;
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME)|(1 << FILPBIT_PDINFO );

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
			fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, "l\210")) {
		    fprintf(stderr,"\tFAILED %s should be l\\210\n",filedir.lname);
	    }
	}
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);

	strcpy(nfile, "laaa");
	nfile[2] = 0xcc;         /* là */
	nfile[3] = 0x80;
	if (FPDelete(Conn, vol,  DIRDID_ROOT , nfile)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test168()
{
char *name = "t168 setfilparam noadouble";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t168: test set file setfilparam noadouble\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	sprintf(temp, "%s/.AppleDouble/%s", Path, name);
	if (!unlink(temp)) {
	    if (Noadouble) {
			fprintf(stderr,"\tFAILED unlink %s was there\n", temp);
			sprintf(temp, "%s/.AppleDouble", Path);
			if (rmdir(temp)) {
				fprintf(stderr,"\tFAILED rmdir %s  %s\n", temp, strerror(errno));
			}
		}
	}

	filedir.isdir = 0;
    bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}

	}
	bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		if (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}

	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}
}

/* ------------------------- */
void test169()
{
u_int16_t dt;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t169: test appl\n");

	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	FPCloseDT(Conn,dt);
}

/* -------------------------- */
void test170()
{
u_int16_t bitmap = 0;
char *name = "test170.txt";
char *name1 = "newtest170.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int fork;
int dir;
u_int16_t dt;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test170: cname error did=1 name=\"\"\n");

    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT_PARENT, "",OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	if (htonl(AFPERR_PARAM) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT_PARENT , "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		if (htonl(AFPERR_PARAM) != FPSetFileParams(Conn, vol, DIRDID_ROOT_PARENT , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (htonl(AFPERR_PARAM) != FPCopyFile(Conn, vol, DIRDID_ROOT_PARENT, vol, DIRDID_ROOT, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT_PARENT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	/* -------------------- */
	ret = FPCreateID(Conn,vol, DIRDID_ROOT_PARENT, "");
	if (htonl(AFPERR_NOOBJ) != ret && htonl(AFPERR_PARAM) != ret ) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT_PARENT,dir, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, DIRDID_ROOT_PARENT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	if (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, DIRDID_ROOT_PARENT , "", bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT_PARENT , "");
	if (dir || ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, DIRDID_ROOT_PARENT , "");
    if (dir || ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT_PARENT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, DIRDID_ROOT_PARENT , "", bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, DIRDID_ROOT_PARENT, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  DIRDID_ROOT_PARENT , "")) { 
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT_PARENT, DIRDID_ROOT, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT_PARENT, name, "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_NOOBJ) != FPEnumerate(Conn, vol,  DIRDID_ROOT_PARENT , "", 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- desktop.c ---- */
	if (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol,  DIRDID_ROOT_PARENT , "", "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol,  DIRDID_ROOT_PARENT , "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol,  DIRDID_ROOT_PARENT , "")) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	/* ---- appl.c ---- */
#if 0
    AddApl
	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	RmAppl
	FPCloseDT(Conn,dt);
#endif

}

/* -------------------------- */
void test171()
{
u_int16_t bitmap = 0;
char *tname = "test171";
char *name = "test171.txt";
char *name1 = "newtest171.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int tdir = DIRDID_ROOT_PARENT;
int fork;
int dir;
u_int16_t dt;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test171: cname error did=1 name=bad name\n");

    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , tdir, tname,OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	if (htonl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  0, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		if (htonl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, tdir , vol, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	/* -------------------- */
	ret = FPCreateID(Conn,vol, tdir, tname);
	if (htonl(AFPERR_NOOBJ) != ret && htonl(AFPERR_PARAM) != ret ) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, tdir,dir, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	if (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, tdir, tname, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, tdir, tname);
    if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, tdir, tname, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, tdir, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  tdir, tname)) { 
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, tdir, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  tdir, tname, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- desktop.c ---- */
	if (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol, tdir, tname, "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	/* ---- appl.c ---- */
#if 0
    AddApl
	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	RmAppl
	FPCloseDT(Conn,dt);
#endif
}

/* -------------------------- */
void test172()
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
u_int16_t dt;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test172: did error did=<deleted> name=test172 name\n");

	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, tname);
	if (!tdir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPDelete(Conn, vol,  tdir , "")) { 
		fprintf(stderr,"\tFAILED\n");
		return;
	}

    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , tdir, tname,OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	if (htonl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  0, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		if (htonl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, tdir , vol, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	/* -------------------- */
	ret = FPCreateID(Conn,vol, tdir, tname);
	if (htonl(AFPERR_NOOBJ) != ret && htonl(AFPERR_PARAM) != ret ) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	ret = FPExchangeFile(Conn, vol, tdir,dir, tname, name1);
	if (ntohl(AFPERR_NOOBJ) != ret) {
		if (Quirk && ret == htonl(AFPERR_PARAM)) 
			fprintf(stderr,"\tFAILED (IGNORED) not always the same error code!\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	if (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, tdir, tname, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, tdir, tname);
    if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, tdir, tname, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, tdir, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  tdir, tname)) { 
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, tdir, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  tdir, tname, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- desktop.c ---- */
	if (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol, tdir, tname, "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	/* ---- appl.c ---- */
#if 0
    AddApl
	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	RmAppl
	FPCloseDT(Conn,dt);
#endif
}

/* -------------------------- */
void test173()
{
u_int16_t bitmap = 0;
char *tname = "test173";
char *name = "test173.txt";
char *name1 = "newtest173.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int tdir = 0;
int fork;
int dir;
u_int16_t dt;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test173: did error did=0 name=test173 name\n");


    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , tdir, tname,OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	if (htonl(AFPERR_PARAM) != FPCreateFile(Conn, vol,  0, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		if (htonl(AFPERR_PARAM) != FPSetFileParams(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (htonl(AFPERR_PARAM) != FPCopyFile(Conn, vol, tdir , vol, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	if (htonl(AFPERR_PARAM) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	/* -------------------- */
	ret = FPCreateID(Conn,vol, tdir, tname);
	if (htonl(AFPERR_PARAM) != ret && htonl(AFPERR_PARAM) != ret ) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, tdir,dir, tname, name1)) {
		fprintf(stderr,"\tFIXME FAILED, NOOBJ expected ?\n");
	}

	if (ntohl(AFPERR_PARAM) != FPExchangeFile(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	if (ntohl(AFPERR_PARAM) != FPSetDirParms(Conn, vol, tdir, tname, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, tdir, tname);
    if (dir || ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_PARAM) != FPGetFileDirParams(Conn, vol, tdir, tname, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_PARAM) != FPSetFilDirParam(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 	}
 	/* ---------------- */
	if (ntohl(AFPERR_PARAM) != FPRename(Conn, vol, tdir, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_PARAM) != FPDelete(Conn, vol,  tdir, tname)) { 
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, tdir, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_PARAM) != FPEnumerate(Conn, vol,  tdir, tname, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- desktop.c ---- */
	if (ntohl(AFPERR_PARAM) != FPAddComment(Conn, vol, tdir, tname, "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_PARAM) != FPGetComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_PARAM) != FPRemoveComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	/* ---- appl.c ---- */
#if 0
    AddApl
	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	RmAppl
	FPCloseDT(Conn,dt);
#endif
}

/* -------------------------- */
void test174()
{
u_int16_t bitmap = 0;
char *tname = "test174";
char *name = "test174.txt";
char *name1 = "newtest174.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
int tdir;
int fork;
int dir;
u_int16_t dt;
u_int16_t vol2;
int ret;

	if (!Conn2) 
		return;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test174: did error two users from parent folder did=<deleted> name=test174 name\n");

	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, tname);
	if (!tdir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPGetFileDirParams(Conn, vol,  tdir , "", 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT))) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT))) {
		fprintf(stderr,"\tFAILED\n");
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (FPDelete(Conn2, vol2,  tdir , "")) { 
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  tdir , "");
		FPCloseVol(Conn2,vol2);
		return;
	}
	FPCloseVol(Conn2,vol2);

    /* ---- fork.c ---- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , tdir, tname,OPENACC_WR | OPENACC_RD);
	if (fork || htonl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (fork) FPCloseFork(Conn,fork);
	}

    /* ---- file.c ---- */
	if (htonl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  0, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	bitmap = (1<<FILPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		filedir.isdir = 0;
 		if (htonl(AFPERR_NOOBJ) != FPSetFileParams(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}
	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, tdir , vol, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT , name1);
	}

	if (htonl(AFPERR_NOOBJ) != FPCopyFile(Conn, vol, DIRDID_ROOT, vol, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	/* -------------------- */
	ret = FPCreateID(Conn,vol, tdir, tname);
	if (htonl(AFPERR_NOOBJ) != ret && htonl(AFPERR_PARAM) != ret ) {
		fprintf(stderr,"\tFAILED\n");
	}

	/* -------------------- */
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	ret = FPExchangeFile(Conn, vol, tdir,dir, tname, name1);
	if (ntohl(AFPERR_NOOBJ) != ret) {
		if (Quirk && ret == htonl(AFPERR_PARAM)) 
			fprintf(stderr,"\tFAILED (IGNORED) not always the same error code!\n");
		else 
			fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPExchangeFile(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
	
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) { fprintf(stderr,"\tFAILED\n");}
	
	/* ---- directory.c ---- */
	filedir.isdir = 1;
 	if (ntohl(AFPERR_NOOBJ) != FPSetDirParms(Conn, vol, tdir, tname, bitmap, &filedir)) {
		fprintf(stderr,"\tFAILED\n");
 	}
 	/* ---------------- */
	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	dir = FPOpenDir(Conn,vol, tdir, tname);
    if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---- filedir.c ---- */

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, tdir, tname, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---------------- */
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (ntohl(AFPERR_NOOBJ) != FPSetFilDirParam(Conn, vol, tdir, tname, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
 	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, tdir, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol,  tdir, tname)) { 
		fprintf(stderr,"\tFAILED\n");
	}
 	/* ---------------- */
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, tdir, DIRDID_ROOT, tname, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, tdir, name, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}

	/* ---- enumerate.c ---- */
	if (ntohl(AFPERR_NODIR) != FPEnumerate(Conn, vol,  tdir, tname, 
	     (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	     (1<<FILPBIT_CDATE) | (1<< FILPBIT_PDID)
	      ,
		 (1<< DIRPBIT_ATTR) |
		 (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- desktop.c ---- */
	if (ntohl(AFPERR_NOOBJ) != FPAddComment(Conn, vol, tdir, tname, "Comment")) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (ntohl(AFPERR_NOOBJ) != FPRemoveComment(Conn, vol, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	/* ---- appl.c ---- */
#if 0
    AddApl
	dt = FPOpenDT(Conn,vol);
	if (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 )) {
		fprintf(stderr,"\tFAILED\n");
	}
	RmAppl
	FPCloseDT(Conn,dt);
#endif
}

/* -------------------------- */
static int create_deleted_folder(char *name)
{
u_int16_t vol2;
int tdir;

	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, name);
	if (!tdir) {
		fprintf(stderr,"\tFAILED\n");
		return 0;
	}

	if (FPGetFileDirParams(Conn, vol,  tdir , "", 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT))) {
		fprintf(stderr,"\tFAILED\n");
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (FPDelete(Conn2, vol2,  tdir , "")) { 
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  tdir , "");
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	FPCloseVol(Conn2,vol2);

	return(tdir);
}

/* ----------- */
void test175()
{
char *tname = "test175";
int tdir,dir;

	if (!Conn2) 
		return;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test175: did error two users in  folder did=<deleted> name=test175\n");

    /* ---- file.c ---- */
	if (!(tdir = create_deleted_folder(tname))) {
		return;
	}

	if (htonl(AFPERR_NOOBJ) != FPCreateFile(Conn, vol,  0, tdir, tname)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ---- directory.c ---- */
	if (!(tdir = create_deleted_folder(tname))) {
		return;
	}

	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (dir) { FPDelete(Conn, vol, dir , "");}
	}
	/* ----------- */
	tname = ".test175";
	if (!(tdir = create_deleted_folder(tname))) {
		return;
	}

	dir  = FPCreateDir(Conn,vol, tdir, tname);
	if (dir || ntohl(AFPERR_NOOBJ) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (dir) { FPDelete(Conn, vol, dir , "");}
	}

	/* ----------- */
	tname = ".test175b";
	if (!(tdir = create_deleted_folder(tname))) {
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT, tname);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
	} 
	else {
	    FPDelete(Conn, vol, dir , "");
	} 
}

/* ------------------------- */
void test176()
{
int  dir;
char *name = "t176 setdirparam noadouble";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t176: test set dir setdirparam noadouble\n");

	if (!(dir = FPCreateDir(Conn, vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	sprintf(temp, "%s/%s/.AppleDouble/.Parent", Path, name);
	if (!unlink(temp)) {
	    if (Noadouble) {
			fprintf(stderr,"\tFAILED unlink %s was there\n", temp);
			sprintf(temp, "%s/%s/.AppleDouble", Path, name);
			if (rmdir(temp)) {
				fprintf(stderr,"\tFAILED rmdir %s  %s\n", temp, strerror(errno));
			}
		}
	}

	filedir.isdir = 1;
    bitmap = (1<<DIRPBIT_MDATE);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}

	}
	bitmap = (1<< DIRPBIT_ATTR) |(1<<DIRPBIT_FINFO)| (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}

	}

	if (htonl(AFPERR_BITMAP) !=FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0, 0xffff)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test177()
{
int  dir;
char *name = ".t177 test nohex,usedots";
char *name1 = "t177.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t177: tests nohex,usedots options\n");

	dir = FPCreateDir(Conn, vol, DIRDID_ROOT , ".Apple");
	if (dir || ntohl(AFPERR_EXIST) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED\n");
		if (dir) { FPDelete(Conn, vol, dir , "");}
	}

	dir = FPCreateDir(Conn, vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	bitmap = (1<< DIRPBIT_ATTR) |(1<<DIRPBIT_FINFO)| (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE);
	if (FPGetFileDirParams(Conn, vol,  dir , "", 0, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		if (!(filedir.attr & ATTRBIT_INVISIBLE)) {
			fprintf(stderr,"\tFAILED visible\n");
		}
	}
	if (FPCreateFile(Conn, vol,  0, dir , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , name, 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPDelete(Conn, vol, dir , name1)) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol, dir , "")) {fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test178()
{
char *name = ".t178 file.txt";
char *name1= ".t178!file.txt";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t178: tests nohex,usedots options\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	bitmap = (1<< FILPBIT_ATTR) |(1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap, 0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (!(filedir.attr & ATTRBIT_INVISIBLE)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}
	FPDelete(Conn, vol, DIRDID_ROOT , name);

	if (htonl(AFPERR_PARAM) != FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol, DIRDID_ROOT , name1);
	}
}

/* ------------------------- */
void test179()
{
int  dir;
char *name = "t179 test dir noadouble";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;


    fprintf(stderr,"===================\n");
    fprintf(stderr,"t179: test dir noadouble\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
 		if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
			fprintf(stderr,"\tFAILED\n");
 		}
	}

	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test180()
{
int  dir;
char *name = "t180 test Map name";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"t180: test Map Name\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		fprintf(stderr,"\tFAILED\n");
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (FPMapID(Conn, 1, 0)) { /* user to Mac roman */
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPMapID(Conn, 1, filedir.uid)) { /* user to Mac roman */
		fprintf(stderr,"\tFAILED\n");
	}
	if (htonl(AFPERR_NOITEM) != FPMapID(Conn, 1, -filedir.uid)) { /* user to Mac roman */
		fprintf(stderr,"\tFAILED\n");
	}

	if (htonl(AFPERR_NOITEM) != FPMapID(Conn, 2, -filedir.gid)) { /* group to Mac roman */
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPMapID(Conn, 2, filedir.gid)) { /* group to Mac roman */
		fprintf(stderr,"\tFAILED\n");
	}
	
	ret = FPMapID(Conn, 3, filedir.uid); /* user to UTF8 */
	if (Conn->afp_version >= 30 && ret) {
		fprintf(stderr,"\tFAILED\n");
	}
	ret = FPMapID(Conn, 4, filedir.gid); /* group to UTF8 */
	if (Conn->afp_version >= 30 && ret) {
		fprintf(stderr,"\tFAILED\n");
	}

	if ((htonl(AFPERR_PARAM) != FPMapID(Conn, 5, filedir.gid))) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* --------------------- */
	if (htonl(AFPERR_PARAM) != FPMapName(Conn, 5, "toto")) {
		fprintf(stderr,"\tFAILED\n");
	} 

	if (FPMapName(Conn, 3, "")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	if (htonl(AFPERR_NOITEM) != FPMapName(Conn, 3, "toto")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	if (htonl(AFPERR_NOITEM) != FPMapName(Conn, 4, "toto")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	/* group */

	if (FPMapName(Conn, 2, "staff")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	if (FPMapName(Conn, 4, "staff")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	if (FPDelete(Conn, vol,  DIRDID_ROOT , name)) { 
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test181()
{
char *name  = "t181 folder";
char *name1 = "t181 donne\314\201es"; /* decomposed données */
char *name2 = "t181 foo";
int  dir;
int  dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test181: test search by ID UTF8\n");

	dir   = FPCreateDir(Conn, vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	dir1  = FPCreateDir(Conn,vol, dir , name1);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		return;
	}

	FPCreateFile(Conn, vol,  0, dir1 , name2);

	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	if (FPEnumerate(Conn, vol,  dir1 , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		/* warm the cache */
		FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);
		FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);
	}

	if (FPDelete(Conn, vol,  dir1 , name2)) {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  dir , name1))  {fprintf(stderr,"\tFAILED\n");}
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {fprintf(stderr,"\tFAILED\n");}
}

/* ------------------------- */
void test182()
{
char *name = "t182 Contents";
char *name1 = "t182 foo";
int  dir,dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test182: dir deleted by someone else, access with ID (dirlookup)\n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	dir  = FPCreateDir(Conn,vol, dir , name);
	if (!dir) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCreateFile(Conn, vol,  0, dir , name1);

	if (FPEnumerate(Conn, vol,  dir, "", 
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

	FPDelete(Conn, vol,  dir , name1);
	sprintf(temp,"%s/%s/%s/.AppleDouble/.Parent", Path, name, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp,"%s/%s/%s/.AppleDouble", Path, name, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED rmdir\n");
	}
	sprintf(temp,"%s/%s/%s", Path, name, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
	}
	/* ------------ */
	sprintf(temp,"%s/%s/.AppleDouble/.Parent", Path, name);
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp,"%s/%s/.AppleDouble", Path, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED rmdir\n");
	}
	sprintf(temp,"%s/%s", Path, name);
	if (rmdir(temp)) {
		fprintf(stderr,"\tFAILED\n");
	}

	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol,  dir, "", 
	         0
	         ,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	dir1  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir1) {
		fprintf(stderr,"\tFAILED\n");
	}
	if (FPGetFileDirParams(Conn, vol,  dir1, "", 0,
		     (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		fprintf(stderr,"\tFAILED\n");
	}

    /* dir and dir1 should be != but if inode reused they are the same */
	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) { fprintf(stderr,"\tFAILED\n"); }
}

/* -------------------------- */
static int create_double_deleted_folder(char *name)
{
u_int16_t vol2;
int tdir;
int tdir1;

	tdir  = FPCreateDir(Conn,vol, DIRDID_ROOT, name);
	if (!tdir) {
		fprintf(stderr,"\tFAILED\n");
		return 0;
	}

	tdir1  = FPCreateDir(Conn,vol,tdir, name);
	if (!tdir1) {
		fprintf(stderr,"\tFAILED\n");
		return 0;
	}

	if (FPGetFileDirParams(Conn, vol,  tdir1 , "", 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT))) {
		fprintf(stderr,"\tFAILED\n");
	}

	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (FPDelete(Conn2, vol2,  tdir1 , "")) { 
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol, tdir1 , "");
		tdir1 = 0;
	}
	if (FPDelete(Conn2, vol2,  tdir , "")) { 
		fprintf(stderr,"\tFAILED\n");
		FPDelete(Conn, vol, tdir , "");
		tdir1 = 0;
	}	
	FPCloseVol(Conn2,vol2);

	return(tdir1);
}

/* ----------- */
void test183()
{
char *tname = "test183";
char *name1 = "test183.new";
int tdir;

	if (!Conn2) 
		return;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test183: did error two users in  folder did=<deleted> name=test183\n");

	/* ---- directory.c ---- */
	if (!(tdir = create_double_deleted_folder(tname))) {
		return;
	}

	if (ntohl(AFPERR_NOOBJ) != FPDelete(Conn, vol, tdir , "")) {
		fprintf(stderr,"\tFAILED\n");
	}

 	/* ---- filedir.c ------------ */
	if (!(tdir = create_double_deleted_folder(tname))) {
		return;
	}
	if (ntohl(AFPERR_NOOBJ) != FPRename(Conn, vol, tdir, "", name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* ------------------------- */
void test184()
{
char *name  = "t184.txt";
char *name1 = "t184new.txt";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test184: rename\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (FPRename(Conn, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPDelete(Conn, vol, DIRDID_ROOT , name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPFlush(Conn, vol);
}

/* ------------------------- 
t181 donne\314\201es
 */
void test185()
{
char *name = "t185.txt";
char *name1 = "t185 donne\314"; /* decomposed données */

	if (Conn->afp_version < 30)
		return;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test185: rename\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
	
	if (ntohl(AFPERR_PARAM) != FPRename(Conn, vol, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		fprintf(stderr,"\tFAILED\n");
	}
	
	if (FPDelete(Conn, vol, DIRDID_ROOT , name)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPFlush(Conn, vol);
}

/* ------------------------- */
void test997()
{
u_int32_t len;
u_int32_t pid;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test997: send SIGHUP\n");

	if (FPGetSessionToken(Conn,0 ,0 ,0 ,NULL)) {
		fprintf(stderr,"\tFAILED\n");
	}
	memcpy(&len, dsi->data, sizeof(len));
	len = ntohl(len);
	if (len != 4) {
		fprintf(stderr,"\tFAILED\n");
	}
	else if (getuid() == 0) {
		memcpy(&pid, dsi->data +4, sizeof(pid));
		sleep(3);
		if (kill(pid, 1) < 0) {
			fprintf(stderr,"\tFAILED kill(%d,1) %s\n",pid, strerror(errno) );
		}
	}
}

/* ------------------------- */
void test998()
{
u_int16_t vol2;

	if (!Conn2) {
		return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test998: bad packet disconnect\n");
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (FPBadPacket(Conn2, 4, "staff")) {
		fprintf(stderr,"\tFAILED\n");
	} 
	FPCloseVol(Conn2,vol2);
}

/* ------------------------- */
void test999()
{

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test999: timeout disconnect \n");
    sleep(60*3);
}

/* ======================= */
void run_one()
{
	dsi = &Conn->dsi;
	if (Conn->afp_version < 10)
		return;
	vol  = FPOpenVol(Conn, Vol);

    test997();	
	FPCloseVol(Conn,vol);
}

/* ------------------ */
void run_common()
{
	vol  = FPOpenVol(Conn, Vol);
	if (!vol) 
		return;
	test5();
	test6();
	test13();test14();test15();test16();test17();
	test18();test19();test20();test21();
	
	test26();test27();

	FPCloseVol(Conn,vol);
	test28();

	vol  = FPOpenVol(Conn, Vol);
	test40();

	test63();test64();
	test70();
	test75();test76();
	test78();
	test79();test80();
	test81();test82();
	test88();
	test99();
	test100();test101();test102();
	test103();test104();test105();
	test108();
	test111();
	test116();
	test118();
	test122();test123();
	test132();
	test138();
	test145();
	test151();
	test157();test158();
	test161();
	test170();test171();test172();test173();test174();
	test175();
	test182();test183();
}

/* ------------------ */
void run_all()
{	
u_int16_t dt;

	dsi = &Conn->dsi;

	test1();
	vol  = FPOpenVol(Conn, Vol);
	if (!vol) 
		return;
	dt = FPOpenDT(Conn,vol);
	#if 0
		test3();
	#endif	
	test5();test6();
	
	test11();test12();test13();test14();test15();test16();
	test17();test18();test19();test20();test21();
	
	test24();
	test26();test27();
	FPCloseDT(Conn,dt);
	FPCloseVol(Conn,vol);

	test28();

	vol  = FPOpenVol(Conn, Vol);
	test29();test30();test31();test32();test34();test35();
	test36();test37();test38();test39();test40();
	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	test41();test42();test43();test44();
	FPCloseVol(Conn,vol);

	vol  = FPOpenVol(Conn, Vol);
	test26();test45();test46();test47();test48();test49();
	test50();test51();test52();
	test1();
	test53();test54();test55();test56();
	test57();test58();
	test60();test61();test62();test63();
	test64();
	test68();test69();test70();
	test71();test72();test73();
	test75();

	test77();test78();test79();test80();
	test81();test82();test83();test84();
	test85();
	test87();test88();test89();test90();
	test91();test92();test93();test94();
	test95();test96();test97();test98();
	test99();test100();test101();
	test106();
	test109();test110();
	test112();test113();test114();test115();
	test118();
	test120();test121();
	test124();test125();	
	test127();test128();test129();
	test130();test131();
	test133();test134();test135();
	test136();test137();
	test139();test140();test141();
	test143();
	test144();
	test149();test150();
	test152();test153();test154();
	test156();
	test159();
	test163();test164();
	test180();
	test182();
	FPCloseVol(Conn,vol);

	if (Conn->afp_version < 30)
		return;
	vol  = FPOpenVol(Conn, Vol);
	test22();test23();
	test59();
	test66();test67();
	test148();
	FPCloseVol(Conn,vol);

	if (Conn->afp_version < 31)
		return;
	vol  = FPOpenVol(Conn, Vol);
	test25();
	test997();
	FPCloseVol(Conn,vol);
}
/* ------------------ */
void run_loop()
{
	dsi = &Conn->dsi;
	vol  = FPOpenVol(Conn, Vol);
	if (!vol) 
		return;
	while (1) {
		test143();
	}
	
}

/* ------------------ */
void run_all_double()
{
	dsi = &Conn->dsi;

	vol  = FPOpenVol(Conn, Vol);
	test65();
	test74();
	test78();
	test81();
	test107();
	test117();
	test119();
	test142();
	test146();
	FPCloseVol(Conn,vol);
}

/* ------------------ */
void run_noadouble()
{
	dsi = &Conn->dsi;
	vol  = FPOpenVol(Conn, Vol);
	test82();
	test108();
	test123();
	test154();
	test165();
	test168();test169();
	test176();
	test177();
	test178();
	test184();
	FPCloseVol(Conn,vol);
}

/* ------------------ */
void run_root()
{
	dsi = &Conn->dsi;
	vol  = FPOpenVol(Conn, Vol);
    /* need admin right*/
	test86();
	test998();
	test999();
	FPCloseVol(Conn,vol);
}

/* ------------------ */
void run_utf8()
{
	dsi = &Conn->dsi;
	vol  = FPOpenVol(Conn, Vol);
	test160();test161();test162();
	test166();test167();
	test181();

	test185();
	FPCloseVol(Conn,vol);
}
