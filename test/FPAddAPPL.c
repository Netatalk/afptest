/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test214()
{
u_int16_t vol = VolID;
u_int16_t dt;
unsigned int ret;
char *file = "t214 file";
char *name = "t214 dir";
int dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPAddAPPL:test214: test appl\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name)) ) {
		nottested();
		return;
	}

	dt = FPOpenDT(Conn,vol);

	ret = FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 );
	if   (htonl(AFPERR_NOITEM) != ret) {
		goto fin;
	}
	FAIL (htonl(AFPERR_BADTYPE) != FPAddAPPL(Conn , dt, dir, "ttxt", 0xafb471c0, ""))
	FAIL (htonl(AFPERR_BADTYPE) != FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, name))

	/*"SimpleText"*/
	ret = FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, file);
	if (not_valid(ret, /* MAC */AFPERR_NOOBJ, 0)) {
		failed();
	}
	if (!ret) {
		FAIL ( FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttxt", file))
	}
	
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file))
	FAIL (FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, file))
	
	FAIL ( FPRemoveAPPL(Conn , dt, DIRDID_ROOT, "ttxt", file))

fin:
	FAIL (dir && FPDelete(Conn, vol,  dir , ""))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , file))
	FAIL (FPCloseDT(Conn,dt))
}

/* ----------- */
void FPAddAPPL_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPAddAPPL page 94\n");
	test214();
}

