/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test44()
{
char *name = "t44 dir";
int  dir;
int did;
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test44: access .. folder\n");

	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		return;
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
}

/* -------------------------- */
STATIC void test58()
{
u_int16_t vol = VolID;

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

	if (Conn->afp_version >= 30)
		return;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test70: bogus cname (unknow type)\n");

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

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test94: test invisible bit attribute\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
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
}

/* --------------------- */
STATIC void test104()
{
char *name1 = "t104 dir1";
char *name2 = "t104 dir2";
char *name3 = "t104 dir3";
char *name4 = "t104 dir4";
char *name5 = "t104 file";

int  dir1 = 0, dir2 = 0, dir3 = 0, dir4 = 0;

int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = (1<< DIRPBIT_DID)|(1<< DIRPBIT_LNAME);
u_int16_t vol = VolID;
DSI *dsi;

	dsi = &Conn->dsi;


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

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test132: GetFilDirParams errors\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
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
}

/* ------------------------- */
STATIC void test194()
{
int  dir = 0;
char *name = "t194 dir";
u_int16_t vol = VolID;
DSI *dsi;

	if (!Conn2) 
		return;
	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetFileDirParms:test194: dir without access\n");

	if (!(dir = no_access_folder(vol, DIRDID_ROOT, name))) {
		return;
	}

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, 0, 
	    (1 <<  DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID) |
	    (1 << DIRPBIT_ACCESS))) {
	    failed();
	}
	delete_folder(vol, DIRDID_ROOT, name);
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
}

