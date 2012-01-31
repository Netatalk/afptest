/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

/* ------------------------- */
STATIC void test89()
{
int  dir;
char *file = "t89 test error setfilparam";
char *name = "t89 error setfilparams dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;
unsigned ret;

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPSetFileParms:test89: test set file setfilparam\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
 	if (!(dir = folder_with_ro_adouble(vol, DIRDID_ROOT, name, file))) {
		nottested();
		goto test_exit;
 	}

	if (FPGetFileDirParams(Conn, vol,  dir , file, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		ret = FPSetFileParams(Conn, vol, dir , file, bitmap, &filedir);
		if (not_valid(ret, 0, AFPERR_ACCESS)) {
			failed();
		}
	}
	bitmap = (1<<FILPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  dir, file, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
 		FAIL (FPSetFileParams(Conn, vol, dir , file, bitmap, &filedir))
	}
	delete_ro_adouble(vol, dir, file);
test_exit:
	exit_test("test89");
}

/* ------------------------- */
STATIC void test120()
{
char *name = "t120 test file setfilparam";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)| (1<<FILPBIT_CDATE) | 
					(1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPSetFileParms:t120: test set file setfilparam (create .AppleDouble)\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto test_exit;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (!Mac) {
            delete_unix_rf(Path,"", name);
		}
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir))
	}

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
test_exit:
	exit_test("test120");
}

/* ----------- */
void FPSetFileParms_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPSetFileParms page 262\n");
    test89();
    test120();
}

