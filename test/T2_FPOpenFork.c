/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

static char temp[MAXPATHLEN];   

/* ------------------------- */
STATIC void test49()
{
char *name = "t49 folder";
char *file = "t49 file.txt";
u_int16_t bitmap = 0;
int fork, fork1;
u_int16_t vol = VolID;
int dir;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test49: open read-write file without ressource fork\n");
    fprintf(stderr,"FPOpenFork:test49: in a read-write folder\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
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

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test152: Error when no write access to .AppleDouble\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

 	if (!folder_with_ro_adouble(vol, DIRDID_ROOT, name, file)) {
		nottested();
		return;
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
}

/* ------------------------- */
STATIC void test153()
{
char *name  = "t153.txt";
u_int16_t bitmap = 0;
int fork;
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test153: open data fork without ressource fork\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)){ 
		nottested();
		return;
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

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test156: Open data fork with no write access to .AppleDouble\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}

 	if (!(dir = folder_with_ro_adouble(vol, DIRDID_ROOT, name, file))) {
		nottested();
		return;
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
}
/* ------------------------- */
STATIC void test321()
{
u_int16_t bitmap = 0;
int fork;
char *file  = "t321 test.txt";
u_int16_t vol = VolID;
int fd;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork:test321: Bogus (empty) resource fork\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		return;
	}
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , file)){ 
		nottested();
		return;
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
	bitmap = 0xe93f;
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
}

/* ----------- */
void FPOpenFork_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPOpenFork page 230\n");
    test49();
	test152();    
	test153();
	test156();
	test321();
}

