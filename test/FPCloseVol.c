/* ----------------------------------------------
*/
#include "specs.h"

/* ----------- */
STATIC void test204()
{
u_int16_t vol = VolID;
int ret;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCloseVol:test204: Close Volume call\n");

	FAIL (FPCloseVol(Conn,vol))
	/* double close */
	ret = FPCloseVol(Conn, vol);
	if (not_valid(ret, AFPERR_ACCESS, AFPERR_PARAM)) {
		failed();
	}
	FAIL (htonl(AFPERR_PARAM) != FPCloseVol(Conn, vol +1))
	
	vol = VolID = FPOpenVol(Conn, Vol);
	if (vol == 0xffff) {
		failed();
		return;
	}

}

/* ----------- */
void FPCloseVol_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCloseVol page 130\n");
	test204();
}

