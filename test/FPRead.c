/* ----------------------------------------------
*/
#include "specs.h"


/* ------------------------- */
STATIC void test5()
{
u_int16_t bitmap = 0;
int fork, fork1;
char *name = "t5 file.txt";
u_int16_t vol = VolID;
int size;
DSI *dsi;

	dsi = &Conn->dsi;
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test5: read/write data fork\n");
	size = min(10000, dsi->server_quantum);
	if (size < 2000) {
		fprintf(stderr,"\t server quantum (%d) too small\n", size);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, size, Data)) {
		failed();
		goto fin1;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		failed();
		goto fin1;
	}

	if (FPWrite(Conn, fork1, size -2000, 2048, Data, 0 /*0x80 */)) {
		failed();
		goto fin2;
	}
	FAIL (FPFlushFork(Conn, fork1))

	if (FPRead(Conn, fork, 0, size, Data)) {
		failed();
		goto fin2;
	}

	if (FPWrite(Conn, fork1, 0, 100, Data, 0x80 )) {
		failed();
		goto fin2;
	}
	FAIL (FPFlush(Conn, vol))
	FAIL (FPCloseFork(Conn,fork1))
	FAIL (FPCloseFork(Conn,fork))
	/* ----------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		failed();
		goto fin2;
	}

	if (ntohl(AFPERR_ACCESS) != FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), 0)) {
		failed();
		goto fin2;
	}

	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_DFLEN), 0)) {
		failed();
		goto fin2;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 10, size, Data)) {
		failed();
		goto fin2;
	}
	
	if (FPWrite(Conn, fork1, 100, 20, Data, 0 )) {
		failed();
		goto fin2;
	}

	if (FPRead(Conn, fork, 110, 10, Data)) {
		failed();
		goto fin2;
	}

fin2:
	FPCloseFork(Conn,fork1);

fin1:
	FAIL (FPCloseFork(Conn,fork)) 

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ------------------------- */
STATIC void test46()
{
u_int16_t bitmap = 0;
int fork, fork1;
char *name = "t46 file.txt";
u_int16_t vol = VolID;
int size;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPread:test46: read/write resource fork\n");
	size = min(10000, dsi->server_quantum);
	if (size < 2000) {
		fprintf(stderr,"\t server quantum (%d) too small\n", size);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, size, Data)) {
		failed();
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		failed();
		goto fin;
	}

	if (FPWrite(Conn, fork1, size -2000, 2048, Data, 0 /*0x80 */)) {
		failed();
		FPCloseFork(Conn,fork1);
		goto fin1;
	}
	FPFlushFork(Conn, fork1);

	if (FPRead(Conn, fork, 0, size, Data)) {
		failed();
	}

	if (FPWrite(Conn, fork1, 0, 100, Data, 0x80 )) {
		failed();
		goto fin1;
	}
	FPCloseFork(Conn,fork1);
	FPCloseFork(Conn,fork);
	/* ----------------- */
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name,OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork1) {
		failed();
		goto fin;
	}

	if (ntohl(AFPERR_ACCESS) != FPSetForkParam(Conn, fork, (1<<FILPBIT_RFLEN), 0)) {
		failed();
		goto fin1;
	}

	if (FPSetForkParam(Conn, fork1, (1<<FILPBIT_RFLEN), 0)) {
		failed();
		goto fin1;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 10, size, Data)) {
		failed();
		goto fin1;
	}
	
	if (FPWrite(Conn, fork1, 100, 20, Data, 0 )) {
		failed();
	}
	else if (FPRead(Conn, fork, 110, 10, Data)) {
		failed();
	}
fin1:
	FPCloseFork(Conn,fork1);
fin:
	if (fork) FPCloseFork(Conn,fork);

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
}

/* -------------------------- */
STATIC void test59()
{
int fork;
u_int16_t bitmap = 0;
char *name = "test59 FPRead,FPWrite 2GB lim";
u_int16_t vol = VolID;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test59: 2 GBytes for offset limit FPRead, FPWrite\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

    /* > 2 Gb */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name ,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}

	FAIL (ntohl(AFPERR_PARAM) != FPRead(Conn, fork, ((off_t)1 << 31) +20, 3000, Data)) 
	FAIL (ntohl(AFPERR_PARAM) != FPWrite(Conn, fork, ((off_t)1 << 31) +20, 3000, Data, 0)) 
	ret = FPWrite(Conn, fork, 0x7fffffff, 30, Data,0);
	if (not_valid(ret, /* MAC */AFPERR_MISC, AFPERR_DFULL)) {
		failed();
	}

	FAIL (FPCloseFork(Conn,fork))
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* -------------------------- */
STATIC void test61()
{
int fork;
u_int16_t bitmap = 0;
char *name = "test61 FPRead, FPWrite error";
u_int16_t vol = VolID;
int size;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRread:test61: FPRead, FPWrite errors\n");
	size = min(10000, dsi->server_quantum);
	if (size < 2000) {
		fprintf(stderr,"\t server quantum (%d) too small\n", size);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR );
	if (!fork) {
		failed();
		goto fin;
		return;
	}
	FAIL (ntohl(AFPERR_ACCESS) != FPRead(Conn, fork, 0, 30, Data)) 
	FAIL (FPWrite(Conn, fork, 0, 0, Data, 0))
	FAIL (FPCloseFork(Conn,fork))

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_RD );
	if (!fork) {
		failed();
		goto fin;
	}
	FAIL (ntohl(AFPERR_ACCESS) != FPWrite(Conn, fork, 0, 30, Data,0)) 

	FAIL (FPRead(Conn, fork, 0, 0, Data))
	FAIL (FPCloseFork(Conn,fork))

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR|OPENACC_RD );
	if (!fork) {
		failed();
		goto fin;
	}
	FAIL (FPWrite(Conn, fork, 0, 300, Data, 0)) 
	FAIL (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 400, Data)) 
	FAIL (FPWrite(Conn, fork, 0, size -1000, Data, 0)) 
	FAIL (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, size, Data) )

	FPCloseFork(Conn,fork);
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}


/* ----------- */
void FPRead_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead page 238\n");
	test5();
	test46();
	test59();
	test61();
}

