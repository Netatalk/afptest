/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test203()
{
u_int16_t vol = VolID;
u_int16_t bitmap = (1<<FILPBIT_MDATE);
struct afp_filedir_parms filedir;
int fork = 0;
char *name = "t203 file";
u_int32_t mdate;
DSI *dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPFlushFork:test203: flush fork call\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto test_exit;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , bitmap ,DIRDID_ROOT, name,
		OPENACC_RD | OPENACC_WR);

    if (!fork) {
		failed();
		goto fin;
    }        
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->commands +2 * sizeof(u_int16_t), bitmap, 0);
	mdate = filedir.mdate;

	sleep(2);
	FAIL (FPFlushFork(Conn, fork))
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}
	
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +sizeof(u_int16_t), bitmap, 0);
	/* is that always true? ie over nfs */
	if (mdate != filedir.mdate) {
		fprintf(stderr,"\tFAILED dates differ\n");
		failed_nomsg();
	}

	mdate = filedir.mdate;
	FAIL (FPWrite(Conn, fork, 0, 10, Data, 0 ))
	sleep(2);
	FAIL (FPFlushFork(Conn, fork))
	if (FPGetForkParam(Conn, fork, bitmap)) {
		failed();
		goto fin;
	}
	
	filedir.isdir = 0;
	afp_filedir_unpack(&filedir, dsi->data +sizeof(u_int16_t), bitmap, 0);
	/* is that always true? ie over nfs */
	if (mdate == filedir.mdate) {
		fprintf(stderr,"\tFAILED dates equal\n");
		failed_nomsg();
	}

	FAIL (htonl(AFPERR_PARAM) != FPFlushFork(Conn, fork +1))

fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
test_exit:
	exit_test("test203");
}

/* ----------- */
void FPFlushFork_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPFlushFork page 171\n");
	test203();
}

