/* ----------------------------------------------
*/
#include "specs.h"
#include "volinfo.h"

static char temp[MAXPATHLEN];   

/* -------------------------- */
STATIC void test325()
{
char *name = "t325 file";
u_int16_t vol = VolID;
DSI *dsi;
int ret;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
	fprintf(stdout, "FPCreateFile:test325:  recreate a file with dangling symlink and no right\n");

	if ((!Path && !Mac) || (volinfo.v_adouble == AD_VERSION_EA)) {
        test_skipped(T_MAC_PATH);
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto test_exit;
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fprintf(stdout,"unlink data fork\n");
		if (unlink(temp) <0) {
			fprintf(stdout,"\tFAILED unlink(%s) %s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
		
		sprintf(temp,"%s/.AppleDouble/%s", Path, name);
		fprintf(stdout,"chmod 444 resource fork\n");
		if (chmod(temp, 0444) <0) {
			fprintf(stdout,"\tFAILED chmod(%s) %s\n", temp, strerror(errno));
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
test_exit:
	exit_test("test325");
}

/* ----------- */
void FPCreateFile_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPCreateFile page 138\n");
    test325();
}

