/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
STATIC void test76()
{
int  dir;
char *name = "t76 Resolve ID file";
char *name1 = "t76 Resolve ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test76: Resolve ID\n");

	if (!(get_vol_attrib(vol) & VOLPBIT_ATTR_FILEID) ) {
		fprintf(stderr,"FileID calls Not supported\n");
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name))

	FAIL (FPCreateID(Conn,vol, dir, name))

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
		FAIL (htonl(AFPERR_BITMAP) != FPResolveID(Conn, vol, filedir.did, 0xffff)) 
	}
		
	FAIL (FPDelete(Conn, vol,  dir , name))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))
}

/* ------------------------- 
*/
STATIC void test91()
{
int  dir;
int  dir1;
char *name = "t91 test ID file";
char *name1 = "t91 test ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
int ret;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;


    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test91: Resolve ID errors\n");

	if (!(get_vol_attrib(vol) & VOLPBIT_ATTR_FILEID) ) {
		fprintf(stderr,"FileID calls Not supported\n");
		return;
	}

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))

	ret = FPDeleteID(Conn, vol,  dir1);
	if (htonl(AFPERR_NOID) != ret) {
		failed();
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		failed();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name))

	FAIL (htonl(AFPERR_NOID) != FPResolveID(Conn, vol, dir1, bitmap))
	FAIL (htonl(AFPERR_BADTYPE) != FPResolveID(Conn, vol, dir, bitmap))

	FAIL (FPCreateID(Conn,vol, dir, name)) 
	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		FAIL (FPDeleteID(Conn, vol, filedir.did)) 
	}	
	ret = FPCreateID(Conn,vol, dir, name);
	if (not_valid(ret, /* MAC */AFPERR_EXISTID, 0)) {
		failed();
	}
	
	FAIL (htonl(AFPERR_EXISTID) != FPCreateID(Conn,vol, dir, name)) 
	FAIL (FPDelete(Conn, vol,  dir , name)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) 
}

/* -------------------------- */
void test129()
{
int i;
int fork;
int  dir;
int  dir1;
char *name = "t129 Resolve ID file";
char *name1 = "t129 Resolve ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

int type = OPENFORK_DATA;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test129: Resolve ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);

	if (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_NOID ) != FPResolveID(Conn, vol, filedir.did, bitmap)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test130()
{
int i;
int fork;
int  dir;
int  dir1;
char *name = "t130 Delete ID file";
char *name1 = "t130 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

int type = OPENFORK_DATA;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test130: Delete ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (ntohl(AFPERR_BADTYPE ) != FPDeleteID(Conn, vol, dir)) {
		fprintf(stderr,"\tFAILED\n");
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);

	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_PARAM ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
}

/* -------------------------- */
void test131()
{
int i;
int fork;
int  dir;
int  dir1;
char *name = "t131 Delete ID file";
char *name1 = "t131 Delete ID dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;

int type = OPENFORK_DATA;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test131: Resolve ID \n");

	if (!Path) {
		fprintf(stderr,"\tNOT TESTED\n");
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {fprintf(stderr,"\tFAILED\n");}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		fprintf(stderr,"\tFAILED\n");
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (FPResolveID(Conn, vol, filedir.did, bitmap)) {
			fprintf(stderr,"\tFAILED\n");
		}
	}

	sprintf(temp1, "%s/%s/.AppleDouble/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}
	sprintf(temp1, "%s/%s/%s", Path, name1, name);
	if (unlink(temp1) <0) {
		fprintf(stderr,"\tFAILED unlink %s %s\n", temp, strerror(errno));
	}

	delete_dir(name1);
	if (ntohl(AFPERR_NOOBJ ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
	FPCloseVol(Conn,vol);
	vol  = FPOpenVol(Conn, Vol);
	if (ntohl(AFPERR_NOID ) != FPDeleteID(Conn, vol, filedir.did)) {
		fprintf(stderr,"\tFAILED\n");
	}
}


/* ----------- */
void FPResolveID_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID page 252\n");
	test76();
	test91();
}

