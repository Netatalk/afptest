/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test201()
{
u_int16_t  dir;
u_int16_t vol = VolID;
int ret;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPCloseDT:test201: FPCloseDT call\n");

	if (0xffff == (dir = FPOpenDT(Conn,vol))) {
		nottested();
		goto test_exit;
	}
	ret = FPCloseDT(Conn, dir +1);
	if (not_valid(ret, AFPERR_PARAM, 0)) {
		failed();
	}
	FAIL (FPCloseDT(Conn, dir))
test_exit:
	exit_test("test201");
}

/* ----------- */
void FPCloseDT_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCloseDT page 128\n");
	test201();
}

