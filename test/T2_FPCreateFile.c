/* ----------------------------------------------
*/
#include "specs.h"

static char temp[MAXPATHLEN];   

/* -------------------------- */
STATIC void test325()
{
char *name = "t325 file";
u_int16_t vol = VolID;
DSI *dsi;
int ret;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPCreateFile:test325:  recreate a file with dangling symlink and no right\n");

	if (!Path && !Mac) {
        test_skipped(T_MAC_PATH);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fprintf(stderr,"unlink data fork\n");
		if (unlink(temp) <0) {
			fprintf(stderr,"\tFAILED unlink(%s) %s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
		
		sprintf(temp,"%s/.AppleDouble/%s", Path, name);
		fprintf(stderr,"chmod 444 resource fork\n");
		if (chmod(temp, 0444) <0) {
			fprintf(stderr,"\tFAILED chmod(%s) %s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
	}
	else {
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
	}

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name);
	if (not_valid(ret, /* MAC */0, AFPERR_ACCESS)) {
		failed();
	}
	
	ret = FPDelete(Conn, vol,  DIRDID_ROOT , name);
	if (not_valid(ret, /* MAC */0, AFPERR_NOOBJ)) {
		failed();
	}

fin:
	FPDelete(Conn, vol,  DIRDID_ROOT , name);
	if (!Mac) {
		unlink(temp);
	}
}

/* ----------- */
void FPCreateFile_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCreateFile page 138\n");
    test325();
}

