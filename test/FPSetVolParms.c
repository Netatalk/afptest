/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test206()
{
u_int16_t bitmap;
u_int16_t vol = VolID;
struct afp_volume_parms parms;
DSI *dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPSetVolParms:test206: Set Volume parameters\n");
    bitmap = (1 << VOLPBIT_ATTR  )
	    |(1 << VOLPBIT_SIG   )
    	|(1 << VOLPBIT_CDATE )
	    |(1 << VOLPBIT_MDATE )
    	|(1 << VOLPBIT_BDATE )
	    |(1 << VOLPBIT_VID   )
    	|(1 << VOLPBIT_BFREE )
	    |(1 << VOLPBIT_BTOTAL)
    	|(1 << VOLPBIT_NAME);

 	FAIL (FPGetVolParam(Conn, vol, bitmap)) 
 	if (parms.bdate == parms.mdate) {
 		fprintf(stderr,"Backup and modification date are the same!\n");
 		nottested();
		goto test_exit;
 	}
	afp_volume_unpack(&parms, dsi->commands +sizeof( u_int16_t ), bitmap);
	bitmap = (1 << VOLPBIT_BDATE );
 	FAIL (htonl(AFPERR_PARAM) != FPSetVolParam(Conn, vol +1, bitmap, &parms)) 
 	FAIL (htonl(AFPERR_BITMAP) != FPSetVolParam(Conn, vol , 0xffff, &parms))
 	parms.bdate = parms.mdate;
 	FAIL (FPSetVolParam(Conn, vol, bitmap, &parms)) 
 	parms.bdate = 0;
	bitmap = (1 << VOLPBIT_BDATE )|(1 << VOLPBIT_MDATE );
 	FAIL (FPGetVolParam(Conn, vol, bitmap)) 
	afp_volume_unpack(&parms, dsi->commands +sizeof( u_int16_t ), bitmap);
 	if (parms.bdate != parms.mdate) {
 		fprintf(stderr,"\tFAILED Backup %x and modification %x date are not the same!\n",parms.bdate, parms.mdate );
 		failed_nomsg();
 	}
 	
test_exit:
	exit_test("test206");
} 	

/* ----------- */
void FPSetVolParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSetVolParms page 268\n");
	test206();
}

