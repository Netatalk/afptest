/* ----------------------------------------------
*/
#include "specs.h"


/* ------------------------- */
STATIC void test216()
{
u_int16_t bitmap = 0;
int fork;
char *name = "t216 file.txt";
u_int16_t vol = VolID;
int size;
DSI *dsi;

	dsi = &Conn->dsi;
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPWrite:test216: read/write data fork\n");
	size = min(0x20000, dsi->server_quantum);
	if (size < 0x20000) {
		fprintf(stderr,"\t server quantum (%d) too small\n", size);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		
	memset(Data, 0xff, 0x20000);
	
	if (htonl(AFPERR_DFULL) != FPWrite(Conn, fork, 0x7fffffffL -20, 15000, Data, 0 )) {
		failed();
		goto fin;
	}

	if (htonl(AFPERR_DFULL) != FPWrite(Conn, fork, 0x7fffffffL -12000, 15000, Data, 0 )) {
		failed();
		goto fin;
	}

	if (htonl(AFPERR_DFULL) != FPWrite(Conn, fork, 0x7ffe0000L, size, Data, 0 )) {
		failed();
		goto fin;
	}

fin:

	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ----------- */
void FPWrite_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPWrite page 270\n");
	test216();
}

