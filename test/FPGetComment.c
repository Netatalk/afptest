/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test53()
{
int fork;
char *name  = "t53 dir no access";
char *name1 = "t53 file.txt";
char *name3  = "t53 --rwx-- dir";
int pdir;
int dir;
int ret;
u_int16_t vol = VolID;
u_int16_t vol2;
DSI *dsi2;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetComment:test53: get comment\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		goto test_exit;
	}		

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, name))) {
		goto test_exit;
	}
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		goto fin;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, DIRDID_ROOT , name3))) {
		nottested();
		goto fin;
	}

	if (ntohl(AFPERR_NOITEM) != FPGetComment(Conn, vol,  DIRDID_ROOT , name)) {
		failed();
	}
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1))
	ret = FPGetComment(Conn, vol,  DIRDID_ROOT , name1);
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, 0)) {
		failed();
	}

	ret = FPGetComment(Conn, vol,  DIRDID_ROOT , name3);
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, 0)) {
		failed();
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name1, OPENACC_RD );
	if (!fork) {
		failed();
		goto fin;
	}
	ret = FPGetComment(Conn, vol,  DIRDID_ROOT , name1);
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, 0)) {
		failed();
	}

	FAIL (FPCloseFork(Conn,fork))

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name3)) 
	FAIL (FPCloseVol(Conn2,vol2))
fin:
	delete_folder(vol, DIRDID_ROOT, name);
test_exit:
	exit_test("test53");
}

/* ----------- */
void FPGetComment_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetComment page 176\n");
	test53();
}

