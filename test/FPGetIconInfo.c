/* ----------------------------------------------
*/
#include "specs.h"

extern char icon0_256[];

/* -------------------------- */
STATIC void test213()
{
u_int16_t vol = VolID;
u_int16_t dt;
unsigned int ret;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetIconInfo:test213: get Icon Info call\n");

	dt = FPOpenDT(Conn,vol);

	ret = FPGetIconInfo(Conn,  dt, "ttxt", 1);
	if (ret == htonl(AFPERR_NOITEM)) {
		FAIL (FPAddIcon(Conn,  dt, "ttxt", "3DMF", 1, 0, 256, icon0_256 ))
	}

	FAIL (FPGetIconInfo(Conn,  dt, "ttxt", 1))
	
	FAIL (htonl(AFPERR_NOITEM) != FPGetIconInfo(Conn,  dt, "ttxt", 256 )) 

	if (!Mac) {
		FAIL (FPGetIconInfo(Conn,  dt, "UNIX", 1 ))
		FAIL (htonl(AFPERR_NOITEM) != FPGetIconInfo(Conn,  dt, "UNIX", 2 ))
	}
	
	FPCloseDT(Conn,dt);
	exit_test("test213");
}

/* ----------- */
void FPGetIconInfo_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetIconInfo page 188\n");
	test213();
}

