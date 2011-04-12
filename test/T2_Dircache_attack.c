#include "specs.h"

STATIC void test500()
{
    DSI *dsi = &Conn->dsi;
    uint16_t vol1 = VolID;
    uint16_t vol2;
    char *dir = "t500 dir";
    char *subdir1 = "t500 subdir1";
    char *subdir2 = "t500 subdir2";
    char *renamedsubdir1 = "t500 renamedsubdir1";
    uint32_t dir_id, subdir1_id, subdir2_id;
    int ofs = 3 * sizeof(uint16_t);
    struct afp_filedir_parms filedir;
    uint16_t bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME);

	enter_test();
    fprintf(stderr, "===================\n");
    fprintf(stderr, "Dircache:test500: \n");

	if (!Conn2) {
		test_skipped(T_CONN2);
		goto test_exit;
	}		
	if ((vol2 = FPOpenVol(Conn2, Vol)) == 0xffff) {
        failed();
        goto test_exit;
    }

    /* Create directories with first connection */
    if ((dir_id = FPCreateDir(Conn, vol1, DIRDID_ROOT, dir)) == 0)
        FAILED;
    if ((subdir1_id = FPCreateDir(Conn, vol1, dir_id, subdir1)) == 0)
        FAILED;
    if ((subdir2_id = FPCreateDir(Conn, vol1, dir_id, subdir2)) == 0)
        FAILED;

    /* Move and rename dir with second connection */
    FAIL( FPMoveAndRename(Conn, vol1, dir_id, subdir2_id, subdir1, renamedsubdir1) );

    /* Enumerate with first connection, does it crash or similar ? */
	if (FPEnumerate(Conn, vol1, subdir2_id, "", (1<<FILPBIT_FNUM), (1<< DIRPBIT_PDID)))
        FAILED;

    /* Manually check name and CNID */
	if (FPGetFileDirParams(Conn, vol1, subdir2_id, renamedsubdir1, 0, bitmap))
        FAILED;

	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data + ofs, 0, bitmap);

	if (filedir.did != subdir1_id) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, subdir1_id);
        FAILED;
	}
	if (strcmp(filedir.lname, renamedsubdir1)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, renamedsubdir1);
		FAILED;
	}


fin:
	FAIL( FPCloseVol(Conn2, vol2) );
    FAIL( FPDelete(Conn, vol1, subdir1_id, "") );
    FAIL( FPDelete(Conn, vol1, subdir2_id, "") );
    FAIL( FPDelete(Conn, vol1, dir_id, "") );

test_exit:
	exit_test("test500");
}

void Dircache_attack_test()
{
    fprintf(stderr, "===================\n");
    fprintf(stderr, "Dircache attack\n");
    test500();
}
