/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test215()
{
u_int16_t vol = VolID;
u_int16_t dt;
char *file = "t215 file";
char *file1 = "t215 file1";
unsigned int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRemoveAPPL:t215: remove appl\n");

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)) {
		nottested();
		return;
	}
	dt = FPOpenDT(Conn,vol);
	FAIL (htonl(AFPERR_NOITEM) != FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 ))

	FAIL (FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, file))

	ret = FPRemoveAPPL(Conn , dt, DIRDID_ROOT_PARENT, "ttxt", file1);
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, AFPERR_NOOBJ)) {
		failed();
	}

	ret = FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttut", file1);
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, AFPERR_NOOBJ)) {
		failed();
	}

	ret = FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttxt", "");
	if (not_valid(ret, /* MAC */AFPERR_NOITEM, AFPERR_BADTYPE)) {
		failed();
	}

	FAIL (htonl(AFPERR_PARAM) != FPRemoveAPPL(Conn , dt, 0, "ttxt", file))

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , file))
	FAIL ( FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttxt", file))
	FAIL (FPCloseDT(Conn,dt))
}

/* ----------- */
void FPRemoveAPPL_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRemoveAPPL page 245\n");
	test215();
}

