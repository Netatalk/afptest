/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test199()
{
int dir;
char *name = "t199 dir";
u_int16_t vol = VolID;
int ret;
DSI *dsi;

		
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPCloseDir:test199: FPCloseDir call\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	dsi = &Conn->dsi;
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}

	dir = FPOpenDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		goto fin;
	}
	FAIL (FPCloseDir(Conn, vol, DIRDID_ROOT_PARENT))
	FAIL (FPCloseDir(Conn, vol, DIRDID_ROOT))
	ret = FPCloseDir(Conn, vol +1, dir);
	if (not_valid(ret, /* MAC */AFPERR_PARAM, 0)) {
		failed();
	}
	
	FAIL (FPCloseDir(Conn, vol, dir))

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ----------- */
void FPCloseDir_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCloseDir page 127\n");
	test199();
}

