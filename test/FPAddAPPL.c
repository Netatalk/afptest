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
char *ndir  = "t214 dir no access";
unsigned int pdir;

int dir;

	if (!Conn2) 
		return;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPAddAPPL:test214: test appl\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name)) ) {
		nottested();
		return;
	}

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, ndir))) {
		goto fin;
	}

	dt = FPOpenDT(Conn,vol);

	ret = FPGetAppl(Conn,  dt, "ttxt", 1, 0x42 );
	if   (htonl(AFPERR_NOITEM) != ret) {
		goto fin;
	}
	FAIL (htonl(AFPERR_BADTYPE) != FPAddAPPL(Conn , dt, dir, "ttxt", 0xafb471c0, ""))
	FAIL (htonl(AFPERR_BADTYPE) != FPAddAPPL(Conn , dt, DIRDID_ROOT, "ttxt", 0xafb471c0, name))

	FAIL (htonl(AFPERR_NOOBJ) != FPAddAPPL(Conn , dt, DIRDID_ROOT_PARENT, "ttxt", 0xafb471c0, name))
	FAIL (htonl(AFPERR_ACCESS) != FPAddAPPL(Conn , dt, pdir, "ttxt", 0xafb471c0, file))

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
	delete_folder(vol, DIRDID_ROOT, ndir);

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

