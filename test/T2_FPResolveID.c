/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

static char temp[MAXPATHLEN];   
static char temp1[MAXPATHLEN];   

/* -------------------------- */
STATIC void test129()
{
int  dir;
u_int16_t vol = VolID;
char *name = "t129 Resolve ID file";
char *name1 = "t129 Resolve ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test129: Resolve ID \n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		failed();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name))

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		FAIL ((FPResolveID(Conn, vol, filedir.did, bitmap)))
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}

	if (delete_unix_dir(Path, name1)) {
		failed();
	}

	FAIL (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap))
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	FAIL (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap)) 
}

/* -------------------------- */
STATIC void test130()
{
u_int16_t vol = VolID;
int  dir;
char *name = "t130 Delete ID file";
char *name1 = "t130 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test130: Delete ID \n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		failed();
		return;
	}

	FAIL (ntohl(AFPERR_BADTYPE ) != FPDeleteID(Conn, vol, dir))

	FAIL (FPCreateFile(Conn, vol,  0, dir , name))

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}

	if (delete_unix_dir(Path, name1)) {
		failed();
	}

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	FAIL (ntohl(AFPERR_PARAM ) != FPDeleteID(Conn, vol, filedir.did))
}

/* -------------------------- */
STATIC void test131()
{
u_int16_t vol = VolID;
int  dir;
char *name = "t131 Delete ID file";
char *name1 = "t131 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test131: Resolve ID \n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		failed();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name)) 

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
		failed_nomsg();
	}

	if (delete_unix_dir(Path, name1)) {
		failed();
	}

	FAIL (ntohl(AFPERR_NOOBJ ) != FPDeleteID(Conn, vol, filedir.did)) 
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	FAIL (ntohl(AFPERR_NOID ) != FPDeleteID(Conn, vol, filedir.did)) 
}


/* ----------- */
void FPResolveID_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID page 252\n");
	test129();
	test130();
	test131();
}

