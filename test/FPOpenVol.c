/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test205()
{
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;
u_int16_t ret;
char *tp;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenVol:t205: Open Volume call\n");

    FAIL (FPCloseVol(Conn, vol));
	/* --------- */
    ret = FPOpenVolFull(Conn, Vol, 0);
    if (ret != 0xffff || dsi->header.dsi_code != htonl(AFPERR_BITMAP)) {
		failed();    
    }
	
	if (ret != 0xffff) {
    	FAIL (FPCloseVol(Conn, ret));
	}
	/* --------- */
    ret = FPOpenVolFull(Conn, Vol, 0xffff);
    if (ret != 0xffff || dsi->header.dsi_code != htonl(AFPERR_BITMAP)) {
		failed();    
    }
	if (ret != 0xffff) {
    	FAIL (FPCloseVol(Conn, ret));
	}
	/* --------- */
	tp = strdup(Vol);
	if (!tp) {
		goto fin;
	}
	*tp = *tp +1;
    ret = FPOpenVol(Conn, tp);
	free(tp);
    if (ret != 0xffff || dsi->header.dsi_code != htonl(AFPERR_PARAM)) {
		failed();    
    }
	if (ret != 0xffff) {
    	FAIL (FPCloseVol(Conn, ret));
	}
	/* -------------- */
	ret = FPOpenVol(Conn, Vol);
	if (ret == 0xffff) {
		failed();
	}
	vol = FPOpenVol(Conn, Vol);
	if (vol != ret) {
		fprintf(stderr,"\tFAILED double open != volume id\n");
		failed_nomsg();
	}	
    FAIL (FPCloseVol(Conn, ret));
    FAIL (!FPCloseVol(Conn, ret));
fin:
	ret = VolID = FPOpenVol(Conn, Vol);
	if (ret == 0xffff) {
		failed();
	}
	exit_test("test205");
}

/* ----------- */
void FPOpenVol_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenVol page 235\n");
    
    test205();
}

