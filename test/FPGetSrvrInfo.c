/* ----------------------------------------------
*/
#include "specs.h"

/* ----------------------- */
STATIC void test1(void)
{
int ret;
	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSrvrInfo:test1: GetSrvInfo\n");

	ret = FPGetSrvrInfo(Conn);
	if (ret) {
		failed();
	}
	
	exit_test("test1");
}

/* ----------- */
void FPGetSrvrInfo_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSrvInfo page 194\n");
	test1();
}

