/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

static char temp[MAXPATHLEN];   

STATIC void test136()
{
int dir;
char *name  = "t136 move, rename ";
char *name1 = "t136 dir/new file name";
char *name2 = "new file name";
char *ndir  = "t136 dir";
u_int16_t vol = VolID;
unsigned int ret;


	if (!Path && !Mac) {
		return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test136: move and rename in a dir without .AppleDouble\n");

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name))
	if (!Mac) {
		sprintf(temp, "%s/%s", Path, ndir);
		fprintf(stderr, "mkdir(%s)\n", temp);
		if (mkdir(temp, 0777)) {
			fprintf(stderr,"\tFAILED mkdir %s %s\n", temp, strerror(errno));
			failed_nomsg();
		}
	}
	else {
		dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir);
		if (!dir) {
			failed();
		}
	}

	ret = FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1);
	if (not_valid(ret, AFPERR_MISC, AFPERR_PARAM)) {
		failed();
	}
	
	dir = get_did(Conn, vol, DIRDID_ROOT, ndir);

	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2)) 
	
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name1))
	FAIL (!FPDelete(Conn, vol,  DIRDID_ROOT, name))
	FAIL (FPDelete(Conn, vol,  dir, ""))
}

/* ----------------------- */
STATIC void test137()
{
int fork;
int dir;
u_int16_t bitmap = 0;
char *name  = "t137 move, rename ";
char *name1 = "t137 dir/new file name";
char *name2 = "new file name";
char *ndir  = "t137 dir";
u_int16_t vol = VolID;
unsigned int ret;


	if (!Path && !Mac) {
		return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test137: move and rename open file in dir without .AppleDouble\n");

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name))
	if (!Mac) {
		sprintf(temp, "%s/%s", Path, ndir);
		fprintf(stderr, "mkdir(%s)\n", temp);
		if (mkdir(temp, 0777)) {
			fprintf(stderr,"\tFAILED mkdir %s %s\n", temp, strerror(errno));
			failed_nomsg();
		}
	}
	else {
		dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir);
		if (!dir) {
			failed();
		}
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
	}
	ret = FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1);
	if (not_valid(ret, AFPERR_MISC, AFPERR_PARAM)) {
		failed();
	}
	
	dir = get_did(Conn, vol, DIRDID_ROOT, ndir);

	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2)) 
	
	FAIL (fork && FPCloseFork(Conn,fork)) 

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name1))
	FAIL (!FPDelete(Conn, vol,  DIRDID_ROOT, name))
	FAIL (FPDelete(Conn, vol,  dir, ""))
}

/* -------------------------- */
STATIC void test139()
{
int  dir;
char *name = "t139 file";
char *name1 = "t139 dir";
u_int16_t vol = VolID;

	if (!Path && !Mac) {
		return;
	}

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename:test139: Move And Rename \n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	
	if (!Mac) {
		delete_unix_rf(Path,"", name);
	}
	FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, "")) 

	FAIL (FPDelete(Conn, vol,  dir , name))
	FAIL (!FPDelete(Conn, vol,  DIRDID_ROOT, name))
	FAIL (FPDelete(Conn, vol,  dir , ""))
}

/* ----------- */
void FPMoveAndRename_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMoveAndRename page 223\n");
    test136();
    test137();
    test139();
}

