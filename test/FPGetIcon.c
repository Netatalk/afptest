/* ----------------------------------------------
*/
#include "specs.h"

extern char icon0_256[];

/* -------------------------- */
STATIC void test115()
{
u_int16_t vol = VolID;
u_int16_t dt;
unsigned int ret;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetIcon:test115: get Icon call\n");

	dt = FPOpenDT(Conn,vol);

	ret = FPGetIcon(Conn,  dt, "ttxt", "3DMF", 1, 256);
	if (ret == htonl(AFPERR_NOITEM)) {
		FAIL (FPAddIcon(Conn,  dt, "ttxt", "3DMF", 1, 0, 256, icon0_256 ))
	}

	FAIL (FPGetIcon(Conn,  dt, "ttxt", "3DMF", 1, 256))
	
	if (!Mac) {
		FAIL (htonl(AFPERR_NOITEM) != FPGetIcon(Conn,  dt, "UNIX", "TEXT",  2 ,512))
		FAIL (FPGetIcon(Conn,  dt, "UNIX", "TEXT", 1, 256 ))
	}
	
	FPCloseDT(Conn,dt);
	exit_test("test115");
}

/* ----------- */
void FPGetIcon_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetIcon page 186\n");
	test115();
}

