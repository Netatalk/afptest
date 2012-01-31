/* ----------------------------------------------
*/
#include "specs.h"

STATIC void test75()
{
char *name = "t75 test get user info";
u_int32_t uid;
u_int32_t gid;
int dir;
int ret;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID);
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;

	enter_test();
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPGetUserInfo:test75: Get User Info\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		goto test_exit;
	}
	FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) 

	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	FAIL (htonl(AFPERR_PARAM) != FPGetUserInfo(Conn, 0, 0, 1)) 
	FAIL (htonl(AFPERR_BITMAP) != FPGetUserInfo(Conn, 1, 0, 0xff))

	ret = FPGetUserInfo(Conn, 1, 0, 3);
	if (ret ) {
		failed();
	}
	else {
		memcpy(&uid, dsi->commands + sizeof(u_int16_t), sizeof(u_int32_t));
		memcpy(&gid, dsi->commands + sizeof(u_int16_t) +sizeof(u_int32_t), sizeof(u_int32_t));
		uid = ntohl(uid);
		gid = ntohl(gid);
		FAIL (FPMapID(Conn, 1, uid))
		FAIL (FPMapID(Conn, 2, gid))
		if (uid != filedir.uid ) {
			fprintf(stdout, "\tFAILED uids differ\n");
			failed_nomsg();
		}
	}

	/* user id */
	ret = FPGetUserInfo(Conn, 1, 0, 1);
	if (ret) {
		failed();
		goto fin;
	}
	memcpy(&uid, dsi->commands + sizeof(u_int16_t), sizeof(u_int32_t));
	uid = ntohl(uid);

	/* primary group */
	ret = FPGetUserInfo(Conn, 1, 0, 2);
	if (ret) {
		failed();
		goto fin;
	}
	memcpy(&gid, dsi->commands + sizeof(u_int16_t), sizeof(u_int32_t));
	gid = ntohl(gid);
	FAIL (FPMapID(Conn, 1, uid))
	FAIL (FPMapID(Conn, 2, gid))
	if (uid != filedir.uid) {
		fprintf(stdout, "\tFAILED uid differ\n");
		failed_nomsg();
	}
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
test_exit:
	exit_test("test75");
}

/* ----------- */
void FPGetUserInfo_test()
{
    fprintf(stdout,"===================\n");
    fprintf(stdout,"FPFPGetUserInfo page 204\n");
	test75();
}

