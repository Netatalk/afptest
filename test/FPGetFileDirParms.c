/* ----------------------------------------------
*/
#include "specs.h"

static char temp[MAXPATHLEN+1];
static char temp1[MAXPATHLEN+1];

/* ------------------------- */
STATIC void test44()
{
char *name = "t44 dir";
unsigned int  dir, did;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test44: access .. folder\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		goto test_exit;
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "/", 0,(1<< DIRPBIT_DID))) {
		failed();
		goto fin;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	if (dir != did) {
		fprintf(stderr,"\tFAILED DIDs differ\n");
		failed_nomsg();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  dir, "//", 0,(1<< DIRPBIT_DID))) {
		failed();
		goto fin;
	}
	memcpy(&did, dsi->data +3 * sizeof( u_int16_t ), sizeof(did));

	if (DIRDID_ROOT != did) {
		fprintf(stderr,"\tFAILED DID not DIRDID_ROOT\n");
		failed_nomsg();
		goto fin;
	}

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
test_exit:
	exit_test("test44");
}

/* -------------------------- */
STATIC void test58()
{
u_int16_t vol = VolID;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test58: folder 1 (DIRDID_ROOT_PARENT)\n");

	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT_PARENT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS))
	) {
		failed();
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT_PARENT, Vol, 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_OFFCNT))
	) {
		failed();
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "", 0, 
	        (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) | (1<<DIRPBIT_UID) |
	    	(1 << DIRPBIT_GID) |(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_OFFCNT))
	) {
		failed();
	}
	exit_test("test58");
}

/* ----------- */
STATIC void test70()
{
u_int16_t vol = VolID;
int ofs;
u_int16_t bitmap;
int len;
int did = DIRDID_ROOT;
char *name = "t70 bogus cname";
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test70: bogus cname (unknow type)\n");
	if (Conn->afp_version < 30) {
		test_skipped(T_AFP3);
		goto test_exit;
	}

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"GetFileDirParams Vol %d \n\n", vol);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_GETFLDRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(1 << FILPBIT_LNAME);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(DIRPBIT_LNAME);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 4;		/* ERROR !! long name */
	len = strlen(name);
	dsi->commands[ofs++] = len;
	u2mac(&dsi->commands[ofs], name, len);
	ofs += len;

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; // htonl(err);
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_stream_receive(dsi, dsi->data, DSI_DATASIZ, &dsi->datalen);

	dump_header(dsi);
	if (dsi->header.dsi_code != htonl(AFPERR_PARAM)) {
		failed();
	}
test_exit:
	exit_test("test70");
}

/* ------------------------- */
STATIC void test94()
{
int  dir;
char *name = "t94 invisible dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t vol = VolID;
u_int16_t bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE)|(1<< DIRPBIT_OFFCNT);
DSI *dsi;
int offcnt;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test94: test invisible bit attribute\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		goto test_exit;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		nottested();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	offcnt = filedir.offcnt;
	fprintf(stderr,"Modif date parent %x offcnt %d\n", filedir.mdate, offcnt);
	sleep(4);
	
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	fprintf(stderr,"Modif date dir %x \n", filedir.mdate);
	sleep(5);
			
	filedir.attr = ATTRBIT_INVISIBLE | ATTRBIT_SETCLR ;
	bitmap = (1<<DIRPBIT_ATTR);
 	if (FPSetDirParms(Conn, vol, DIRDID_ROOT , name, bitmap, &filedir)) {
		failed();
		goto end;
 	}

	bitmap = (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_MDATE)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) {
		failed();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	fprintf(stderr,"Modif date dir %x\n", filedir.mdate);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , "", 0,bitmap )) {
		failed();
		goto end;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	if (offcnt != filedir.offcnt) {
		fprintf(stderr,"\tFAILED got %d want %d\n",filedir.offcnt, offcnt);
		failed_nomsg();
	}
	
	fprintf(stderr,"Modif date parent %x\n", filedir.mdate);
end:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name))
test_exit:
	exit_test("test94");
}

/* --------------------- */
STATIC void test104()
{
char *name1 = "t104 dir1";
char *name2 = "t104 dir2";
char *name3 = "t104 dir3";
char *name4 = "t104 dir4";
char *name5 = "t104 file";

unsigned int  dir1 = 0, dir2 = 0, dir3 = 0, dir4 = 0;

int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;


	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:t104: cname with trailing 0 \n");

	if (!(dir1 = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		goto fin;
	}
	if (!(dir2 = FPCreateDir(Conn,vol, dir1 , name2))) {
		nottested();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  0, dir2 , name5)) { 
		nottested();
		goto fin;
	}		

	if (!(dir3 = FPCreateDir(Conn,vol, dir2, name3))) {
		nottested();
		goto fin;
	}	
	if (!(dir4 = FPCreateDir(Conn,vol, dir3, name4))) {
		nottested();
		goto fin;
	}	


	if (FPGetFileDirParams(Conn, vol, dir3, "", 0, bitmap)) {
		failed();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	if (filedir.did != dir3) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir3 );
		failed_nomsg();
		goto fin;
	}
	if (strcmp(filedir.lname, name3)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name3);
		failed_nomsg();
		goto fin;
	}

    bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME)|(1<< DIRPBIT_OFFCNT);
	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4///", 0, bitmap)) {
		failed();
		goto fin;
	}
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	if (filedir.did != dir2) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir2 );
		failed_nomsg();
		goto fin;
	}
	if (strcmp(filedir.lname, name2)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name2);
		failed_nomsg();
		goto fin;
	}
	if (filedir.offcnt != 2) {
		fprintf(stderr,"\tFAILED %d\n",filedir.offcnt);
		failed_nomsg();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol, dir3, "t104 dir4/", 0, bitmap)) {
		failed_nomsg();
		goto fin;
	}
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	if (filedir.did != dir4) {
		fprintf(stderr,"\tFAILED %x should be %x\n",filedir.did, dir4 );
		failed_nomsg();
		goto fin;
	}
	if (strcmp(filedir.lname, name4)) {
		fprintf(stderr,"\tFAILED %s should be %s\n",filedir.lname, name4);
		failed_nomsg();
	}
fin:
	FAIL (dir3 && FPDelete(Conn, vol,  dir3 , name4)) 
	FAIL (dir2 && FPDelete(Conn, vol,  dir2 , name3)) 
	FAIL (dir2 && FPDelete(Conn, vol,  dir2 , name5)) 
	FAIL (dir1 && FPDelete(Conn, vol,  dir1 , name2)) 
	FAIL (dir1 && FPDelete(Conn, vol,  dir1 , ""))    
	exit_test("test104");
}

/* -------------------------- */
STATIC void test132()
{
int  dir;
char *name = "t132 file";
char *name1 = "t132 dir";
u_int16_t bitmap;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test132: GetFilDirParams errors\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {
		nottested();
		goto fin;
	}

	FAIL (htonl(AFPERR_BITMAP) != FPGetFileDirParams(Conn, vol,  dir , name, 0xffff,0))
	FAIL (htonl(AFPERR_BITMAP) != FPGetFileDirParams(Conn, vol,  dir , "",0, 0xffff)) 

	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS);

	if (!strcmp("disk1", Vol)) {
		fprintf(stderr, "Volume is disk1 choose other name!\n");
		nottested();
		goto fin;
	}
	if (!FPGetFileDirParams(Conn, vol,  DIRDID_ROOT_PARENT, "disk1", 0, bitmap)) {
		failed();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT_PARENT, Vol, 0, bitmap)) {
		failed();
	}

	FAIL (FPDelete(Conn, vol,  dir , name))
fin:
	FAIL (FPDelete(Conn, vol,  dir , ""))
test_exit:
	exit_test("test132");
}

/* ------------------------- */
STATIC void test194()
{
int  dir = 0;
char *name = "t194 dir";
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test194: dir without access\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		goto test_exit;
	}		

	if (!(dir = no_access_folder(vol, DIRDID_ROOT, name))) {
		goto test_exit;
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
	    failed();
	}
	delete_folder(vol, DIRDID_ROOT, name);
test_exit:
	exit_test("test194");
}

/* ------------------------- */
STATIC void test229()
{
int  dir = 0;
char *name = "t229 file";
char *ndir = "t229 dir";
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test229: unix access privilege\n");
	if (Conn->afp_version < 30) {
		test_skipped(T_AFP3);
		goto test_exit;
	}

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UNIXPRIV)) {
		test_skipped(T_UNIX_PREV);
		goto test_exit;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, dir , name)) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol, dir, "", 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_UNIXPR))) {
	    failed();
	}

	if (FPGetFileDirParams(Conn, vol, dir, name,  
	    (1 <<  FILPBIT_PDINFO) | (1<< FILPBIT_PDID) | (1<< FILPBIT_FNUM) |
	    (1 << DIRPBIT_UNIXPR), 
	    0)) {
	    failed();
	}

	FAIL (FPDelete(Conn, vol,  dir , name))
fin:	
	FAIL (FPDelete(Conn, vol,  dir , ""))
test_exit:
	exit_test("test229");
}

/* ------------------------- */
STATIC void test307()
{
char *name = "t307 dir#2";
u_int16_t vol = VolID;
DSI *dsi;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;
unsigned int dir;
char *result;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test307: mangled dirname\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		failed();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, bitmap)) {
		nottested();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		result = (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname;
		if (strcmp(result, name)) {
			failed();
		}
	}
	if (!FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "bad name #2", 0, bitmap)) {
		failed();
	}
	if (ntohl(AFPERR_NOOBJ) != FPGetFileDirParams(Conn, vol, DIRDID_ROOT, "???#2", 0, bitmap)) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test307");
}

/* ------------------------- */
STATIC void test308()
{
char *name = "t308 dir";
u_int16_t vol = VolID;
DSI *dsi;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;
unsigned int dir;
char *result;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test308: mangled dirname\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		failed();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, bitmap)) {
		nottested();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		result = (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname;
		if (strcmp(result, name)) {
			failed();
		}
	}
	sprintf(temp,"t307#%X", ntohl(dir));
	if (!FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, 0, bitmap)) {
		failed();
	}
	sprintf(temp,"t308#%X", ntohl(dir));
	if (!FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, 0, bitmap)) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test308");
}

/* ------------------------- */
STATIC void test319()
{
u_int16_t vol = VolID;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test319: get volume access right\n");

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, "", 0,(1 << DIRPBIT_ACCESS) )) {
		failed();
	}
	exit_test("test319");
}

/* ------------------------- */
STATIC void test324()
{
char *name = "t324 very long filename more than 31 bytes.txt";
u_int16_t vol = VolID;
DSI *dsi;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;
unsigned int dir;
char *result;
int ret;
int id;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test324: long file name >31 bytes\n");

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name);
	if (ret) {
		nottested();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	/* hack if filename < 255 it works with afp 2.x too */
	id = get_fid(Conn, vol, DIRDID_ROOT , name);
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap, 0)) {
		nottested();
	}
#if 0
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		result = (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname;
		if (strcmp(result, name)) {
			failed();
		}
	}
#endif	
	sprintf(temp1,"#%X.txt",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	/* for afp3 it's not valid mangled filename */
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if ((Conn->afp_version >= 30 && ret != ntohl(AFPERR_NOOBJ)) 
	    || ( Conn->afp_version < 30 && ret)) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test324");
}


/* ------------------------- */
STATIC void test326()
{
#if 0
char *name = "t326 long filename and extension .longtxt";
u_int16_t vol = VolID;
DSI *dsi;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;
unsigned int dir;
char *result;
int ret;
int id;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test326: long file name >31 bytes\n");

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name);
	if (ret) {
		nottested();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	/* hack if filename < 255 it works with afp 2.x too */
	id = get_fid(Conn, vol, DIRDID_ROOT , name);
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap, 0)) {
		nottested();
	}
#if 0
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
		result = (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname;
		if (strcmp(result, name)) {
			failed();
		}
	}
#endif	
	sprintf(temp1,"#%X.txt",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	/* for afp3 it's not valid mangled filename */
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if ((Conn->afp_version >= 30 && ret != ntohl(AFPERR_NOOBJ)) 
	    || ( Conn->afp_version < 30 && ret)) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
#endif
	exit_test("test326");
}

/* ------------------------- */
STATIC void test333()
{
char *name = "t333 very long filename (more than 31 bytes).txt";
u_int16_t vol = VolID;
DSI *dsi;
u_int16_t bitmap = 0;
int ret;
int id;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test333: long file name >31 bytes\n");

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name);
	if (ret) {
		nottested();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	/* hack if filename < 255 it works with afp 2.x too */
	id = get_fid(Conn, vol, DIRDID_ROOT , name);
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap, 0)) {
		nottested();
	}
	sprintf(temp1,"#%X.txt",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	/* for afp3 it's not valid mangled filename */
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if ((Conn->afp_version >= 30 && ret != ntohl(AFPERR_NOOBJ)) 
	    || ( Conn->afp_version < 30 && ret)) {
		failed();
	}

	sprintf(temp1,"#0%X.txt",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if (!ret) {
		failed();
	}
	
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test333");
}

/* ------------------------- */
STATIC void test334()
{
char *name = "t334 very long filename (more than 31 bytes)";
u_int16_t vol = VolID;
DSI *dsi;
u_int16_t bitmap = 0;
int ret;
int id;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test334: long file name >31 bytes (no ext)\n");

	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name);
	if (ret) {
		nottested();
		goto test_exit;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	/* hack if filename < 255 it works with afp 2.x too */
	id = get_fid(Conn, vol, DIRDID_ROOT , name);
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap, 0)) {
		nottested();
	}
	sprintf(temp1,"#%X",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	/* for afp3 it's not valid mangled filename */
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if ((Conn->afp_version >= 30 && ret != ntohl(AFPERR_NOOBJ)) 
	    || ( Conn->afp_version < 30 && ret)) {
		failed();
	}

	sprintf(temp1,"#%X.",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if (!ret) {
		failed();
	}
	
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test334");
}

/* ------------------------- */
STATIC void test335()
{
char *name = "t335 very long filename (more than 31 bytes).txt";
char *ndir = "t335 dir";
u_int16_t vol = VolID;
DSI *dsi;
unsigned int  dir;
u_int16_t bitmap = 0;
int ret;
int id;

	dsi = &Conn->dsi;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms::test335: long file name >31 bytes\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir);
	if (!dir) {
		nottested();
		goto test_exit;
	}
	ret = FPCreateFile(Conn, vol,  0, DIRDID_ROOT, name);
	if (ret) {
		nottested();
		goto fin;
	}
	if (Conn->afp_version >= 30) {
		bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		bitmap = (1<<DIRPBIT_LNAME);
	}
	/* hack if filename < 255 it works with afp 2.x too */
	id = get_fid(Conn, vol, DIRDID_ROOT , name);
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, bitmap, 0)) {
		nottested();
	}
	sprintf(temp1,"#%X.txt",ntohl(id));
	memset(temp, 0, sizeof(temp));
	strncpy(temp, name, 31 - strlen(temp1));
	strcat(temp, temp1);
	/* for afp3 it's not valid mangled filename */
	ret = FPGetFileDirParams(Conn, vol, DIRDID_ROOT, temp, bitmap, 0);
	if ((Conn->afp_version >= 30 && ret != ntohl(AFPERR_NOOBJ)) 
	    || ( Conn->afp_version < 30 && ret)) {
		failed();
	}

	ret = FPCreateFile(Conn, vol,  0, dir, temp);
	if (ret) {
		failed();
	}
	FAIL (FPGetFileDirParams(Conn, vol, dir, temp, bitmap, 0))
	
	FAIL (FPDelete(Conn, vol,  dir, temp))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
fin:
	FAIL (FPDelete(Conn, vol,  dir, ""))
test_exit:
	exit_test("test335");
}

/* ----------- */
void FPGetFileDirParms_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms page 179\n");
	test44();
	test58();
	test70();    
	test94();
	test104();
	test132();
	test194();
	test229();
	test307();
	test308();
	test319();
	test324();
	test333();
	test334();
	test335();
}

