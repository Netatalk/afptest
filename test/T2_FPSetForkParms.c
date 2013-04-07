#include "specs.h"
#include "ea.h"

/* -------------------------- */
STATIC void test9()
{
    int fork;
    int fork1 = 0;
    u_int16_t bitmap = 0;
    char *name = "ta\314\210st9";
    int ret;
    u_int16_t vol = VolID;
    DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPSetForkParms:test9: name encoding\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto test_exit;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS, bitmap ,DIRDID_ROOT, name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_RFLEN), 10)) {
		failed();
		goto fin;
	}

	if (FPSetForkParam(Conn, fork, (1<<FILPBIT_RFLEN), 0)) {
		failed();
		goto fin;
	}

fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test9");
}

/* ----------- */
void FPSetForkParms_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPSetForkParms\n");
	test9();
}
