/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test56()
{
u_int16_t bitmap;
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPGetVolParms:test56: Volume parameters\n");
    bitmap = (1 << VOLPBIT_ATTR  )
	    |(1 << VOLPBIT_SIG   )
    	|(1 << VOLPBIT_CDATE )
	    |(1 << VOLPBIT_MDATE )
    	|(1 << VOLPBIT_BDATE )
	    |(1 << VOLPBIT_VID   )
    	|(1 << VOLPBIT_BFREE )
	    |(1 << VOLPBIT_BTOTAL)
    	|(1 << VOLPBIT_NAME);
#if 0
	    |(1 << VOLPBIT_XBFREE)
    	|(1 << VOLPBIT_XBTOTAL)
	    |(1 << VOLPBIT_BSIZE);
#endif	    
 	FAIL (FPGetVolParam(Conn, vol, bitmap)) 
 	FAIL (htonl(AFPERR_PARAM) != FPGetVolParam(Conn, vol +1, bitmap)) 
 	FAIL (htonl(AFPERR_BITMAP) != FPGetVolParam(Conn, vol , 0xffff)) 
} 	

/* ----------- */
void FPGetVolParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetVolParms page 207\n");
	test56();
}

