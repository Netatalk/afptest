/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test202()
{
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPFlush:test202: flush volume call\n");

	FAIL (FPFlush(Conn, vol))
	
	FAIL (htonl(AFPERR_PARAM) != FPFlush(Conn, vol +1))
}

/* ----------- */
void FPFlush_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPFlush page 169\n");
	test202();
}

