/* ----------------------------------------------
*/
#include "specs.h"

char icon0_256[] = {
0x1f, 0xff, 0xfc, 0x00, 
0x10, 0x00, 0x06, 0x00, 0x10, 0x00, 0x05, 0x00, 
0x10, 0x00, 0x04, 0x80, 0x10, 0x3f, 0x04, 0x40, 
0x10, 0x41, 0x84, 0x20, 0x10, 0x82, 0x87, 0xf0, 
0x11, 0xfc, 0x80, 0x10, 0x11, 0x04, 0x80, 0x10, 
0x11, 0x04, 0x80, 0x10, 0x11, 0x04, 0x80, 0x10, 
0x11, 0x05, 0x00, 0x10, 0x11, 0x06, 0x00, 0x10, 
0x11, 0xfc, 0x00, 0x10, 0x10, 0x00, 0x1e, 0x10, 
0x10, 0x00, 0x21, 0x10, 0x10, 0x00, 0x40, 0x90, 
0x10, 0x70, 0x80, 0x50, 0x10, 0x50, 0x80, 0x50, 
0x10, 0x88, 0x80, 0x50, 0x10, 0x88, 0x80, 0x50, 
0x11, 0x04, 0x40, 0x90, 0x11, 0x04, 0x21, 0x10, 
0x12, 0x02, 0x1e, 0x10, 0x12, 0x02, 0x00, 0x10, 
0x14, 0x01, 0x00, 0x10, 0x14, 0x01, 0x00, 0x10, 
0x13, 0x06, 0x00, 0x10, 0x10, 0xf8, 0x00, 0x10, 
0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xfc, 0x00, 
0x1f, 0xff, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0x00, 
0x1f, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0xc0, 
0x1f, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 
0x1f, 0xff, 0xff, 0xf0 };

char icon0_64[] = {
0x7f, 0xf0, 0x40, 0x28, 
0x40, 0x24, 0x5c, 0x3c, 0x54, 0x04, 0x5c, 0xe4, 
0x41, 0x14, 0x41, 0x14, 0x41, 0x14, 0x48, 0xe4, 
0x4c, 0x04, 0x54, 0x04, 0x56, 0x04, 0x48, 0x04, 
0x40, 0x04, 0x7f, 0xfc, 0x7f, 0xf0, 0x7f, 0xf8, 
0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 
0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 
0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 
0x7f, 0xfc, 0x7f, 0xfc };

/* -------------------------- */
STATIC void test212()
{
u_int16_t vol = VolID;
u_int16_t dt;
int ret;
DSI *dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
	fprintf(stdout, "FPAddIcon:test212: Add Icon call\n");

	dt = FPOpenDT(Conn,vol);
	FAIL (FPAddIcon(Conn,  dt, "ttxt", "3DMF", 1, 0, 256, icon0_256 ))
	FAIL (htonl(AFPERR_PARAM) != FPAddIcon(Conn,  dt+1, "ttxt", "3DMF", 1, 0, 256, icon0_256 ))

	ret = FPGetIcon(Conn,  dt, "ttxt", "3DMF", 1, 256);
	if (ret) {
		failed();
	}
	else if (memcmp(dsi->commands, icon0_256, 256)) {
		fprintf(stdout,"\tFAILED AddIcon and GetIcon data differ\n");
		failed_nomsg();
	}

	FAIL (FPAddIcon(Conn,  dt, "ttxt", "3DMF", 1, 0, 256, icon0_256 ))

	ret = FPGetIcon(Conn,  dt, "ttxt", "3DMF", 1, 256);
	if (ret) {
		failed();
	}
	else if (memcmp(dsi->commands, icon0_256, 256)) {
		fprintf(stdout,"\tFAILED AddIcon and GetIcon data differ\n");
		failed_nomsg();
	}


	FAIL (FPAddIcon(Conn,  dt, "ttxt", "3DMF", 4, 0, 64, icon0_64 ))

	ret = FPGetIcon(Conn,  dt, "ttxt", "3DMF", 4, 64);
	if (ret) {
		failed();
	}
	else if (memcmp(dsi->commands, icon0_64, 64)) {
		fprintf(stdout,"\tFAILED AddIcon and GetIcon data differ\n");
		failed_nomsg();
	}

	FAIL (htonl(AFPERR_ITYPE) != FPAddIcon(Conn,  dt, "ttxt", "3DMF", 4, 0, 256, icon0_256))
	
	FPCloseDT(Conn,dt);
	exit_test("test212");
}

/* ----------- */
void FPAddIcon_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPAddIcon page 99\n");
	test212();
}

