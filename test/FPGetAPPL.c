/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test169()
{
u_int16_t vol = VolID;
u_int16_t dt;
char *file = "t169 file";
int dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetAPPL:t169: test appl\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)) {
		nottested();
		return;
	}
	dt = FPOpenDT(Conn,vol);
	FAIL (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 ))

	FAIL (FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, file))

	FAIL (htonl(AFPERR_BITMAP) != FPGetAppl(Conn,  dt, "ttxt", 1, 0xffff ))


	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , file))
	FAIL (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 ))

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , file)) ) {
		failed();
		return;
	}

	FAIL (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 ))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , file))
	FAIL ( FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttxt", file))
	FAIL (FPCloseDT(Conn,dt))
}

/* ----------- */
void FPGetAPPL_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetAPPL page 172\n");
	test169();
}

