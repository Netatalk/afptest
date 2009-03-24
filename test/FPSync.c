/* ----------------------------------------------*/
#include "specs.h"

/* ------------------------- */
STATIC void test2()
{
    int  dir;
    u_int16_t vol = VolID;
    DSI *dsi;
    char *name = "t2 sync dir";
    int  ofs =  3 * sizeof( u_int16_t );
    struct afp_filedir_parms filedir;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPSyncDir(test2): sync dir\n");

 	if (FPSyncDir(Conn, vol, DIRDID_ROOT))
		failed();

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		goto test_exit;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0, DIRPBIT_DID )) {
		failed();
        goto fin;
	}

    filedir.isdir = 1;
    afp_filedir_unpack(&filedir, dsi->data +ofs, 0, DIRPBIT_DID);
    printf("did: '%u'\n", filedir.did);
 	if (FPSyncDir(Conn, vol, htonl(filedir.did)))
		failed();

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
test_exit:
	exit_test("test2");
}
