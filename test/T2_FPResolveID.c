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


/* -------------------------- */
STATIC void test331()
{
u_int16_t vol = VolID;
int  dir;
char *name  = "t331 file";
char *name2 = "t331 file new name";
char *name1 = "t331 dir";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM ) | (1<<DIRPBIT_FINFO);
struct afp_filedir_parms filedir;
int fid = 0;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test331: Resolve ID file modified with local fs\n");

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
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		fid = filedir.did;
		FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
	}
	if (!Mac) {
		sprintf(temp, "%s/%s/%s", Path, name1, name);
		sprintf(temp1,"%s/%s/%s", Path, name1, name2);
		fprintf(stderr,"rename %s %s\n", temp, temp1);
		if (rename(temp, temp1) < 0) {
			fprintf(stderr,"\tFAILED unable to rename %s to %s :%s\n", temp, temp1, strerror(errno));
			failed_nomsg();
		}
		
		sprintf(temp, "%s/%s/.AppleDouble/%s", Path, name1, name);
		sprintf(temp1,"%s/%s/.AppleDouble/%s", Path, name1, name2);
		fprintf(stderr,"rename %s %s\n", temp, temp1);
		if (rename(temp, temp1) < 0) {
			fprintf(stderr,"\tFAILED unable to rename %s to %s :%s\n", temp, temp1, strerror(errno));
			failed_nomsg();
		}

	}
	else {
		FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2))
	}
	if (FPGetFileDirParams(Conn, vol,  dir , name2, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (fid != filedir.did) {
			fprintf(stderr,"\tFAILED FPGetFileDirParams id differ %x %x\n", fid, filedir.did);
			failed_nomsg();
		
		}
		else {
			FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
		}
	}

fin:
	FAIL (FPDelete(Conn, vol,  dir, name2))
	FPDelete(Conn, vol,  dir, name);
	FAIL (FPDelete(Conn, vol,  dir, ""))

}

/* -------------------------- */
static int get_fs_lock(char *folder, char *file)
{
int fd;
struct flock lock;
int ret;

	sprintf(temp, "%s/%s/.AppleDouble/%s", Path, folder, file);
	fprintf(stderr," \n---------------------\n");
	fprintf(stderr, "open(\"%s\", O_RDWR)\n", temp);
	fd = open(temp, O_RDWR, 0);	
	if (fd >= 0) {
		lock.l_start = 0;		/* after meta data */
    	lock.l_type = F_WRLCK;
	    lock.l_whence = SEEK_SET;
    	lock.l_len = 1024;
         
		fprintf(stderr, "fcntl(1024)\n");
    	if ((ret = fcntl(fd, F_SETLK, &lock)) >= 0 || (errno != EACCES && errno != EAGAIN)) {
    		if (!ret >= 0) 
    	    	errno = 0;
    		perror("fcntl ");
			fprintf(stderr,"\tFAILED\n");
			failed_nomsg();
    	}
    	fcntl(fd, F_UNLCK, &lock);
    	close(fd);
    	return 0;
    }
    else {
    	perror("open ");
		fprintf(stderr,"\tFAILED\n");
		failed_nomsg();
    }
    return -1;
}

/* -------------------------- */
STATIC void test360()
{
u_int16_t vol = VolID;
int  dir;
char *name  = "t360 file";
char *name2 = "t360 file new name";
char *name1 = "t360 dir";
char *name3 = "t360 open file";
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM ) | (1<<DIRPBIT_FINFO);
struct afp_filedir_parms filedir;
int fid = 0;
int fork = 0;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID:test360: Resolve ID file modified with local fs and a file is opened\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (Locking) {
		test_skipped(T_LOCKING);
		return;
	}		

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		failed();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name)) 
	FAIL (FPCreateFile(Conn, vol,  0, dir , name3)) 

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS, bitmap , dir, name3, OPENACC_WR |OPENACC_RD|OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPByteLock(Conn, fork, 0, 0 /* set */, 0, 100)) {
		failed();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		fid = filedir.did;
		FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
	}
	if (!Mac) {
		sprintf(temp, "%s/%s/%s", Path, name1, name);
		sprintf(temp1,"%s/%s/%s", Path, name1, name2);
		fprintf(stderr,"rename %s %s\n", temp, temp1);
		if (rename(temp, temp1) < 0) {
			fprintf(stderr,"\tFAILED unable to rename %s to %s :%s\n", temp, temp1, strerror(errno));
			failed_nomsg();
		}
		
		sprintf(temp, "%s/%s/.AppleDouble/%s", Path, name1, name);
		sprintf(temp1,"%s/%s/.AppleDouble/%s", Path, name1, name2);
		fprintf(stderr,"rename %s %s\n", temp, temp1);
		if (rename(temp, temp1) < 0) {
			fprintf(stderr,"\tFAILED unable to rename %s to %s :%s\n", temp, temp1, strerror(errno));
			failed_nomsg();
		}
		if (get_fs_lock(name1, name3) < 0) {
			goto fin;
		}

	}
	else {
		FAIL (FPMoveAndRename(Conn, vol, DIRDID_ROOT, dir, name, name2))
	}
	if (FPGetFileDirParams(Conn, vol,  dir , name2, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (fid != filedir.did) {
			fprintf(stderr,"\tFAILED FPGetFileDirParams id differ %x %x\n", fid, filedir.did);
			failed_nomsg();
		
		}
		else {
			FAIL (FPResolveID(Conn, vol, filedir.did, bitmap)) 
		}
	}
	if (!Mac) {
		if (get_fs_lock(name1, name3) < 0) {
			goto fin;
		}
	}

fin:
	FAIL (fork && FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  dir, name3))
	FAIL (FPDelete(Conn, vol,  dir, name2))
	FPDelete(Conn, vol,  dir, name);
	FAIL (FPDelete(Conn, vol,  dir, ""))

}

/* ----------- */
void FPResolveID_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPResolveID page 252\n");
	test129();
	test130();
	test131();
	test331();
	test360();
}

