/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- 
*/

/* ------------- */
static void check_forklen(DSI *dsi, int type, u_int32_t  len)
{
u_int32_t flen;
	flen = get_forklen(dsi, type);
	if (flen != len) {
		fprintf(stderr,"\tFAILED got %d but %d expected\n", flen, len);
		failed_nomsg();
	}
}

/* ------------- */
static void test_21(u_int16_t vol, char *name, int type)
{

int fork = 0;
int fork2 = 0;
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);
DSI *dsi;

	dsi = &Conn->dsi;

	fork = FPOpenFork(Conn, vol, type, bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD); 
	if (!fork) {
		failed();
		goto fin;
	}

	fork2= FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork2) {
		failed();
		goto fin;
	}

	if (FPWrite(Conn, fork, 0, 100, Data, 0)) {
		failed();
		goto fin;
	}

	bitmap = len;
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}
	check_forklen(dsi, type, 100);
	
	if (FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
		goto fin;
	}
	check_forklen(dsi, type, 100);
	
	FAIL (FPFlushFork(Conn, fork))

	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}
	check_forklen(dsi, type, 100);

	if (FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
		goto fin;
	}
	check_forklen(dsi, type, 100);
fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (fork2 && FPCloseFork(Conn,fork2))

}

/* -------- */
STATIC void test21()
{
u_int16_t vol = VolID;
char *name = "t21 file";

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetForkParms:test21: setting/reading fork len\n");

    
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

    test_21(VolID, name, OPENFORK_DATA);
    test_21(VolID, name, OPENFORK_RSCS);

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* -------------------------- 
FIXME set ressource for size and check
*/
STATIC void test50()
{
u_int16_t bitmap;
int fork = 0;
int fork2 = 0;
char *name = "t50 file";
char *name1 = "t50 new name";
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetForkParms:test50: deny mode & move file\n");

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
	bitmap = (1 << FILPBIT_DFLEN);
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}

	bitmap = (1 << FILPBIT_RFLEN);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}
	if (FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1)) {
		failed();
		goto fin;
	}

	fork2 = FPOpenFork(Conn, vol, OPENFORK_RSCS, 0 ,DIRDID_ROOT, name1, OPENACC_RD| OPENACC_WR);
	if (!fork2) {
		failed();
		goto fin;
	}
	bitmap = (1 << FILPBIT_DFLEN)| (1<< FILPBIT_MDATE);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
	}

	bitmap = (1 << FILPBIT_RFLEN)| (1<< FILPBIT_MDATE);
	if (FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
	}

	FAIL (FPCloseFork(Conn,fork))
	fork = 0;

	bitmap = (1 << FILPBIT_DFLEN)| (1<< FILPBIT_MDATE);
	if (ntohl(AFPERR_BITMAP) != FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
	}

	bitmap = (1 << FILPBIT_RFLEN)| (1<< FILPBIT_MDATE);
	if (FPGetForkParam(Conn, fork2, bitmap)) {
		failed();
	}

fin:
	FAIL (fork2 && FPCloseFork(Conn,fork2))
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) 
}

/* -------------------------- */
STATIC void test188()
{
char *name ="t188 illegal fork";
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetForkParms:test188: illegal fork\n");

	illegal_fork(dsi, AFP_GETFORKPARAM, name);
}

/* -------------------------- */
STATIC void test192()
{
u_int16_t bitmap;
int fork = 0;
char *name = "t192 file";
int ret;
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetForkParms:test192: open write only\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name,OPENACC_WR );

	if (!fork) {
		nottested();
		goto fin;
	}
	bitmap = (1 << FILPBIT_DFLEN);
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}

	bitmap = (1 << FILPBIT_RFLEN)| (1<< FILPBIT_MDATE);
	ret = FPGetForkParam(Conn, fork, bitmap);

	if (not_valid(ret, /* MAC */AFPERR_ACCESS, AFPERR_BITMAP)) {
		failed();
	}
	FAIL (FPCloseFork(Conn,fork))
	fork = 0;

fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ----------- */
void FPGetForkParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetForkParms page 184\n");
    test21();
    test50();
	test188();
	test192();
}

