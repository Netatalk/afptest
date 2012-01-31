/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test398()
{
u_int16_t vol = VolID;
DSI *dsi;
char *file="test398_file";

    dsi = &Conn->dsi;

    enter_test();
	fprintf(stdout,"===================\n");
	fprintf(stdout,"FPGetACL:test398: check ACL support\n");
    if (Conn->afp_version < 30) {
    	test_skipped(T_AFP3);
        goto test_exit;
    }

    if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_ACLS)) {
       test_skipped(T_ACL);
       goto test_exit;
    }

    if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)) {
    	nottested();
        goto test_exit;
    }

	FAIL(FPGetACL(Conn,vol, DIRDID_ROOT , 7, file))
/*
	FPGetACL(Conn,vol, DIRDID_ROOT , 7, "testdir");
	FPGetACL(Conn,vol, DIRDID_ROOT , 8, "test");
	FPGetACL(Conn,vol, DIRDID_ROOT , 23, "test");
	FPGetACL(Conn,vol, DIRDID_ROOT , 23, "test2");
	FPGetACL(Conn,vol, DIRDID_ROOT , 23, "test3");
	FPGetACL(Conn,vol, DIRDID_ROOT , 23, "testdir");
*/
    FPDelete(Conn, vol,  DIRDID_ROOT , file);
test_exit:
	exit_test("test398");
}


/* ------------------------- */
STATIC void test399()
{
u_int16_t vol = VolID;
u_int16_t bitmap;
DSI *dsi;
char *file="test399_file";
char *attr_name="test399_attribute";

    dsi = &Conn->dsi;

	enter_test();
	fprintf(stdout,"===================\n");
	fprintf(stdout,"FPGetExtAttr:test399: check Extended Attributes Support\n");
    if (Conn->afp_version < 30) {
        test_skipped(T_AFP3);
        goto test_exit;
    }

    if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_EXTATTRS)) {
        test_skipped(T_UTF8);
        goto test_exit;
    }

    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);

    if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)) {
    	nottested();
        goto test_exit;
    }

	FAIL(FPSetExtAttr(Conn,vol, DIRDID_ROOT, 2, file, attr_name, "test399_data"))
	FAIL(FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, file, attr_name))

	/* check create flag */
	if (ntohl(AFPERR_EXIST) != FPSetExtAttr(Conn, vol, DIRDID_ROOT, 2, file, attr_name, "test399_newdata"))
		failed();

	FAIL(FPListExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, file))

	FAIL(FPSetExtAttr(Conn, vol, DIRDID_ROOT, 4, file, attr_name, "test399_newdata"))
	FAIL(FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, file, attr_name))
	FAIL(FPRemoveExtAttr(Conn,vol, DIRDID_ROOT , 0, file, attr_name))
	if (ntohl(AFPERR_MISC) != FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, file, attr_name)) {
		failed();
	}
	if (ntohl(AFPERR_MISC) != FPRemoveExtAttr(Conn,vol, DIRDID_ROOT , 0, file, attr_name))
		failed();

    FPDelete(Conn, vol,  DIRDID_ROOT , file);

test_exit:
	exit_test("test399");
}

/* ----------- */
void FPGetACL_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPGetACL\n");

    test398();
    test399();
}

