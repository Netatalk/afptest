/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test201()
{
u_int16_t  dir;
u_int16_t vol = VolID;
int ret;
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPCloseDT:test201: FPCloseDT call\n");

	if (0xffff == (dir = FPOpenDT(Conn,vol))) {
		nottested();
		return;
	}
	ret = FPCloseDT(Conn, dir +1);
	if (not_valid(ret, AFPERR_PARAM, 0)) {
		failed();
	}
	FAIL (FPCloseDT(Conn, dir))
}

/* ----------- */
void FPCloseDT_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCloseDT page 128\n");
	test201();
}

