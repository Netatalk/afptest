/* ----------------------------------------------
*/
#include "specs.h"

/* ----------------------- */
STATIC void test209(void)
{
int ret;
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSrvrParms:test209: GetSrvrParms call\n");

	ret = FPGetSrvrParms(Conn);
	if (ret) {
		failed();
	}
	
}

/* ----------- */
void FPGetSrvrParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSrvrParms page 203\n");
	test209();
}

