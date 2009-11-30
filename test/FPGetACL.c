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
	fprintf(stderr,"===================\n");
	fprintf(stderr,"FPGetACL:test398: enumerate arguments\n");
    if (Conn->afp_version < 30) {
    	test_skipped(T_AFP3);
        goto test_exit;
    }

    if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_ACLS)) {
       test_skipped(T_UTF8);
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
	fprintf(stderr,"===================\n");
	fprintf(stderr,"FPGetExtAttr:test399: enumerate arguments\n");
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

	FAIL(FPSetExtAttr(Conn,vol, DIRDID_ROOT, 2, "file", attr_name, "test399_data"))
	FAIL(FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, "file", attr_name))

	/* check create flag */
	if (ntohl(AFPERR_EXIST) != FPSetExtAttr(Conn, vol, DIRDID_ROOT, 2, "file", attr_name, "test399_newdata"))
		failed();

	FAIL(FPListExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, "file"))

	FAIL(FPSetExtAttr(Conn, vol, DIRDID_ROOT, 4, "file", attr_name, "test399_newdata"))
	FAIL(FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, "file", attr_name))
	FAIL(FPRemoveExtAttr(Conn,vol, DIRDID_ROOT , 0, "file", attr_name))
	if (ntohl(AFPERR_PARAM) != FPGetExtAttr(Conn,vol, DIRDID_ROOT , 0, 4096, "file", attr_name)) {
		failed();
	}
	if (ntohl(AFPERR_PARAM) != FPRemoveExtAttr(Conn,vol, DIRDID_ROOT , 0, "file", attr_name))
		failed();

    FPDelete(Conn, vol,  DIRDID_ROOT , file);

test_exit:
	exit_test("test399");
}

/* ----------- */
void FPGetACL_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetACL\n");

    test398();
    test399();
}

