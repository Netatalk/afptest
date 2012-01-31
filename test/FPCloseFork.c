/* ----------------------------------------------
*/
#include "specs.h"

/* ----------- */
STATIC void test186()
{
int fork;
u_int16_t bitmap = 0;
u_int16_t vol = VolID;
int type = OPENFORK_DATA;
char *name = "t186 FPCloseFork";

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPCloseFork:test186: FPCloseFork\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto test_exit;
	}

	fork = FPOpenFork(Conn, vol, type , bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		nottested();
		FPDelete(Conn, vol,  DIRDID_ROOT, name);
		goto test_exit;
	}
	FAIL (FPCloseFork(Conn,fork))
	/* double close */
	FAIL (htonl(AFPERR_PARAM) != FPCloseFork(Conn, fork))
	
	FAIL (htonl(AFPERR_PARAM) != FPCloseFork(Conn, 0))

	if (FPDelete(Conn, vol,  DIRDID_ROOT, name)) {
		nottested();
	}
test_exit:
	exit_test("test186");
}

/* -------------------------- */
STATIC void test187()
{
char *name ="t187 illegal fork";
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPCloseFork:test187: illegal fork\n");

	illegal_fork(dsi, AFP_CLOSEFORK, name);
	exit_test("test187");
}

/* ----------- */
void FPCloseFork_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPCloseFork page 129\n");
	test186();
	test187();
}

