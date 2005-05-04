/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

static char temp[MAXPATHLEN];   

/* ------------------------- */
STATIC void test47()
{
char *name = "t47 folder";
char *file = "t47 file.txt";
u_int16_t vol = VolID;
u_int16_t bitmap = 0;
int fork = 0, fork1 = 0;
int dir;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test47: open read only file read only then read write\n");
    fprintf(stderr,"FPOpenFork:test47: in a read only folder\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

 	if (!folder_with_ro_adouble(vol, DIRDID_ROOT, name, file)) {
		nottested();
		goto test_exit;
 	}

	dir = get_did(Conn, vol, DIRDID_ROOT, name);

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , dir, file, OPENACC_RD);

	if (!fork) {
		failed();
		goto fin1;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap , dir, file, OPENACC_WR | OPENACC_RD);

	if (fork1) {
		failed();
		goto fin1;
	}		

	FAIL (FPCloseFork(Conn,fork))

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap , dir, file , OPENACC_RD);

	if (!fork) {
		failed();
		goto fin1;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		failed();
		goto fin1;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap , dir, file,OPENACC_WR | OPENACC_RD);

	if (fork1) {
		failed();
		goto fin1;
	}

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		failed();
		goto fin1;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap , dir, file, OPENACC_RD);

	if (!fork1) {
		failed();
		goto fin1;
	}
	FAIL (FPCloseFork(Conn,fork1))
	fork1 = 0;

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		failed();
		goto fin1;
	}

	FAIL (FPCloseFork(Conn,fork))
	fork = 0;
#if 0	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"test47: in a read/write folder\n");


	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);

	if (!fork) {
		failed();
		goto fin1;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		failed();
		goto fin1;
	}		

	FAIL (FPCloseFork(Conn,fork))
	fork = 0;

	strcpy(temp, Path);
	strcat(temp,"/test folder/.AppleDouble/toto.txt");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED Ressource fork not there\n");
		failed_nomsg();
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);

	if (!fork) {
		failed();
		goto fin1;
	}		

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);

	if (fork1) {
		failed();
		goto fin1;
	}		

	FAIL (FPCloseFork(Conn,fork))
	fork = 0;

	strcpy(temp, Path);strcat(temp,"/test folder/.AppleDouble/toto.txt");
	if (unlink(temp)) {
		fprintf(stderr,"\tFAILED Ressource fork not there\n");
		failed_nomsg();
		goto fin1;
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "test folder/toto.txt", OPENACC_RD);
	if (!fork) {
		failed();
		goto fin1;
	}		

	if (ntohl(AFPERR_EOF) != FPRead(Conn, fork, 0, 100, Data)) {
		failed();
		goto fin1;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, "test folder/toto.txt",OPENACC_WR | OPENACC_RD);
    /* bad, but we are able to open read-write the ressource for of a read-only file (data fork)
     * difficult to fix.
     */
	if (!fork1) {
		failed();
		goto fin1;
	}
	
	if (FPWrite(Conn, fork1, 0, 10, Data, 0 )) {
		failed();
		goto fin1;
	}

	if (FPRead(Conn, fork, 0, 10, Data)) {
		failed();
		goto fin1;
	}

	FAIL (FPCloseFork(Conn,fork))
	fork = 0;
	if (FPWrite(Conn, fork1, 0, 20, Data, 0x80 )) {
		failed();
		goto fin1;
	}
	
	if (ntohl(AFPERR_PARAM) != FPRead(Conn, fork, 0, 30, Data)) {
		failed();
	}
#endif
fin1:
	if (fork1) FPCloseFork(Conn,fork1);
	if (fork) FPCloseFork(Conn,fork);
fin:
	delete_ro_adouble(vol, dir, file);
test_exit:
	exit_test("test47");
}

/* ------------------------- */
STATIC void test49()
{
char *name = "t49 folder";
char *file = "t49 file.txt";
u_int16_t bitmap = 0;
int fork, fork1;
u_int16_t vol = VolID;
int dir;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test49: open read-write file without ressource fork\n");
    fprintf(stderr,"FPOpenFork:test49: in a read-write folder\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		goto test_exit;
	}
	if (FPCreateFile(Conn, vol,  0, dir , file)){ 
		nottested();
		goto fin;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, file, OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		
	if (!Mac) {
		sprintf(temp,"%s/%s/.AppleDouble/%s", Path, name, file);
		unlink(temp);
	}
	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, file, OPENACC_WR | OPENACC_RD);

	if (!fork1) {
		failed();
	}		

	FAIL (FPCloseFork(Conn,fork))
	FAIL (FPCloseFork(Conn,fork1))

	if (!unlink(temp)) {
		fprintf(stderr,"\tRessource fork there!\n");
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, file,OPENACC_WR | OPENACC_RD);

	if (!fork1) {
		failed();
	}		
	FAIL (FPCloseFork(Conn,fork1))

	if (!unlink(temp)) {
		fprintf(stderr,"\tRessource fork there!\n");
	}
fin:
	FAIL (FPDelete(Conn, vol,  dir , file))
	FAIL (FPDelete(Conn, vol,  dir , ""))
test_exit:
	exit_test("test49");

}

/* ------------------------- */
STATIC void test152()
{
int dir;
u_int16_t bitmap = 0;
char *name  = "t152 ro AppleDouble";
char *file  = "t152 test.pdf";
int fork;
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;
unsigned int ret;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test152: Error when no write access to .AppleDouble\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

 	if (!folder_with_ro_adouble(vol, DIRDID_ROOT, name, file)) {
		nottested();
		goto test_exit;
 	}
	
	dir = get_did(Conn, vol, DIRDID_ROOT, name);

	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,dir, file ,OPENACC_RD|OPENACC_WR);
	ret = dsi->header.dsi_code;
	if (not_valid(ret, 0, AFPERR_LOCK)) {
		failed();
	}
	if (!Mac && ret != htonl(AFPERR_LOCK)) {
		failed();
	}
	if (fork) {
		FPCloseFork(Conn, fork);
	}
	delete_ro_adouble(vol, dir, file);
test_exit:
	exit_test("test152");
}

/* ------------------------- */
STATIC void test153()
{
char *name  = "t153.txt";
u_int16_t bitmap = 0;
int fork;
u_int16_t vol = VolID;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test153: open data fork without ressource fork\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		nottested();
		goto test_exit;
	}
	if (!Mac && delete_unix_rf(Path, "", name)) {
		nottested();
		goto fin;	
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);

	if (!fork) {
		failed();
		goto fin;
	}		
	FAIL (FPWrite(Conn, fork, 0, 2000, Data, 0 ))

	FAIL (FPRead(Conn, fork, 0, 2000, Data)) 

	FAIL (FPCloseFork(Conn, fork)) 
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name)) 
test_exit:
	exit_test("test153");
}

/* ------------------------- */
STATIC void test156()
{
int dir;
u_int16_t bitmap = 0;
int fork;
char *name  = "t156 ro AppleDouble";
char *file  = "t156 test.pdf";
u_int16_t vol = VolID;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test156: Open data fork with no write access to .AppleDouble\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

 	if (!(dir = folder_with_ro_adouble(vol, DIRDID_ROOT, name, file))) {
		nottested();
		goto test_exit;
 	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, file ,OPENACC_RD|OPENACC_WR);

	if (!fork) {
		failed();
		goto fin;
	}
	if (fork && FPCloseFork(Conn, fork)) {
		failed();
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,dir, file ,OPENACC_RD|OPENACC_WR);
	if (!fork) {
		failed();
	}
	if (fork && FPCloseFork(Conn, fork)) {
		failed();
	}
fin:
	delete_ro_adouble(vol, dir, file);
test_exit:
	exit_test("test156");
}

/* ------------------------- */
STATIC void test321()
{
u_int16_t bitmap = 0;
int fork;
char *file  = "t321 test.txt";
u_int16_t vol = VolID;
int fd;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test321: Bogus (empty) resource fork\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)){ 
		nottested();
		goto test_exit;
	}
	
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, file);
		if (chmod(temp, 0444) < 0) {
			fprintf(stderr,"\tFAILED unable to chmod %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
		sprintf(temp,"%s/.AppleDouble/%s", Path, file);
		fprintf(stderr,"unlink %s \n", temp);
		unlink(temp);
		fd = open(temp, O_RDWR | O_CREAT, 0666);
		if (fd < 0) {
			fprintf(stderr,"\tFAILED unable to create %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
		close(fd);
		if (chmod(temp, 0444) < 0) {
			fprintf(stderr,"\tFAILED unable to chmod %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin;
		}
	}
	bitmap = 0x693f;
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, file, bitmap, 0 )) {
		failed();
		goto fin;
	}
	
	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, file, bitmap, 0 )) {
		failed();
		goto fin;
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,DIRDID_ROOT, file, OPENACC_RD|OPENACC_DWR);
	if (!fork) {
		failed();
	}
	if (fork && FPCloseFork(Conn, fork)) {
		failed();
	}

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, file)) 
test_exit:
	exit_test("test321");
}

/* --------------------- */
STATIC void test372()
{
char *name  = "t372 file name.txt";
char data[20];
u_int16_t vol = VolID;
int fork;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap;
int fd;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test372: no crlf convertion for TEXT file\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}
	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<FILPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
		goto fin;
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		memcpy(filedir.finder_info, "TEXTttxt", 8);
		
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, (1<<FILPBIT_FINFO), &filedir)) 
	    FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0))
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPWrite(Conn, fork, 0, 5, "test\r", 0 )) {
		failed();
		goto fin1;
	}
	
	if (FPRead(Conn, fork, 0, 5, data)) {
		failed();
		goto fin1;
	}
	if (memcmp(data, "test\r", 5)) {
		fprintf(stderr, "\tFAILED wrote \"test\\r\" get \"%s\"\n", data);
	    failed_nomsg();
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fd = open(temp, O_RDWR , 0666);
		if (fd < 0) {
			fprintf(stderr,"\tFAILED unable to open %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin1;
		}
		if (read(fd, data, 5) != 5) {
			fprintf(stderr,"\tFAILED unable to read data:%s\n", strerror(errno));
			failed_nomsg();
		}
		if (memcmp(data, "test\r", 5)) {
		    fprintf(stderr, "\tFAILED not \"test\\r\" get 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		    data[0],data[1],data[2],data[3],data[4]);
		    failed_nomsg();
		}
		close(fd);
	}

fin1:
	FAIL (FPCloseFork(Conn,fork))
fin:
	FPDelete(Conn, vol,  DIRDID_ROOT , name);
test_exit:
	exit_test("test372");
}

/* ------------------------- 
 * for this test you need a volume with options:crlf
 * in AppleVolumes.default
 *
 * not run by default (need -f test387)
*/

/* --------------------- */
STATIC void test387()
{
char *name  = "t387 file name.txt";
char data[20];
u_int16_t vol = VolID;
int fork;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap;
int fd;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test387: crlf convertion for TEXT file\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}
	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<FILPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
		goto fin;
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		memcpy(filedir.finder_info, "TEXTttxt", 8);
		
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, (1<<FILPBIT_FINFO), &filedir)) 
	    FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0))
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPWrite(Conn, fork, 0, 5, "test\r", 0 )) {
		failed();
		goto fin1;
	}
	
	if (FPRead(Conn, fork, 0, 5, data)) {
		failed();
		goto fin1;
	}
	if (memcmp(data, "test\r", 5)) {
		fprintf(stderr, "\tFAILED wrote \"test\\r\" get \"%s\"\n", data);
	    failed_nomsg();
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fd = open(temp, O_RDWR , 0666);
		if (fd < 0) {
			fprintf(stderr,"\tFAILED unable to open %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin1;
		}
		if (read(fd, data, 5) != 5) {
			fprintf(stderr,"\tFAILED unable to read data:%s\n", strerror(errno));
			failed_nomsg();
		}
		if (memcmp(data, "test\n", 5)) {
		    fprintf(stderr, "\tFAILED not \"test\\n\" get 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		    data[0],data[1],data[2],data[3],data[4]);
		    failed_nomsg();
		}
		close(fd);
	}

fin1:
	FAIL (FPCloseFork(Conn,fork))
fin:
	FPDelete(Conn, vol,  DIRDID_ROOT , name);
test_exit:
	exit_test("test387");
}

/* --------------------- */
STATIC void test388()
{
char *name  = "t388 file name.rtf";
char data[20];
u_int16_t vol = VolID;
int fork;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap;
int fd;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test388: crlf convertion for TEXT file (not default type)\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}
	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<FILPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
		goto fin;
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		memcpy(filedir.finder_info, "TEXTttxt", 8);
		
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, (1<<FILPBIT_FINFO), &filedir)) 
	    FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0))
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPWrite(Conn, fork, 0, 5, "test\r", 0 )) {
		failed();
		goto fin1;
	}
	
	if (FPRead(Conn, fork, 0, 5, data)) {
		failed();
		goto fin1;
	}
	if (memcmp(data, "test\r", 5)) {
		fprintf(stderr, "\tFAILED wrote \"test\\r\" get \"%s\"\n", data);
	    failed_nomsg();
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fd = open(temp, O_RDWR , 0666);
		if (fd < 0) {
			fprintf(stderr,"\tFAILED unable to open %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin1;
		}
		if (read(fd, data, 5) != 5) {
			fprintf(stderr,"\tFAILED unable to read data:%s\n", strerror(errno));
			failed_nomsg();
		}
		if (memcmp(data, "test\n", 5)) {
		    fprintf(stderr, "\tFAILED not \"test\\n\" get 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		    data[0],data[1],data[2],data[3],data[4]);
		    failed_nomsg();
		}
		close(fd);
	}

fin1:
	FAIL (FPCloseFork(Conn,fork))
fin:
	FPDelete(Conn, vol,  DIRDID_ROOT , name);
test_exit:
	exit_test("test388");
}

/* --------------------- */
STATIC void test392()
{
char *name  = "t392 file name.pdf";
char data[20];
u_int16_t vol = VolID;
int fork;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap;
int fd;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPRead:test392: no crlf convertion for no TEXT file\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}
	bitmap = (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<FILPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		     (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID);

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
		goto fin;
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		memcpy(filedir.finder_info, "PDF CARO", 8);
		
 		FAIL (FPSetFileParams(Conn, vol, DIRDID_ROOT , name, (1<<FILPBIT_FINFO), &filedir)) 
	    FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0))
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		goto fin;
	}
	if (FPWrite(Conn, fork, 0, 5, "test\r", 0 )) {
		failed();
		goto fin1;
	}
	
	if (FPRead(Conn, fork, 0, 5, data)) {
		failed();
		goto fin1;
	}
	if (memcmp(data, "test\r", 5)) {
		fprintf(stderr, "\tFAILED wrote \"test\\r\" get \"%s\"\n", data);
	    failed_nomsg();
	}
	if (!Mac) {
		sprintf(temp,"%s/%s", Path, name);
		fd = open(temp, O_RDWR , 0666);
		if (fd < 0) {
			fprintf(stderr,"\tFAILED unable to open %s :%s\n", temp, strerror(errno));
			failed_nomsg();
			goto fin1;
		}
		if (read(fd, data, 5) != 5) {
			fprintf(stderr,"\tFAILED unable to read data:%s\n", strerror(errno));
			failed_nomsg();
		}
		if (memcmp(data, "test\r", 5)) {
		    fprintf(stderr, "\tFAILED not \"test\\r\" get 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		    data[0],data[1],data[2],data[3],data[4]);
		    failed_nomsg();
		}
		close(fd);
	}

fin1:
	FAIL (FPCloseFork(Conn,fork))
fin:
	FPDelete(Conn, vol,  DIRDID_ROOT , name);
test_exit:
	exit_test("test392");
}

/* ----------- */
void FPOpenFork_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork page 230\n");
#if 0
    test47();
#endif    
    test49();
	test152();    
	test153();
	test156();
	test321();
	test372();
	test392();
}

