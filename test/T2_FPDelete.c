/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

static char temp[MAXPATHLEN];   

/* ------------------------- */
STATIC void test146()
{
int fork,fork1;
int  dir;
char *name = "t146 file";
char *name1 = "t146 dir";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
u_int16_t vol2;
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;
DSI *dsi2;
int ret;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPDelete:test146: delete read only open file\n");

	if (!Conn2 &&(!Path || !Mac)) {
                test_skipped(T_MAC_PATH);                                                             
		return;
	}

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name1))) {
		nottested();
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir , name))

	if (FPGetFileDirParams(Conn, vol,  dir , "", 0,bitmap )) {
		failed();
	}
	else {
		filedir.isdir = 1;
		afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
        filedir.access[0] = 0; 
        filedir.access[1] = 3; /* everyone */
        filedir.access[2] = 3; /* group */
        filedir.access[3] = 7; /* owner */
 		FAIL (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) 
	}

	/* ------------------ */	
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		failed();
	}
	FAIL (FPEnumerate(Conn2, vol2,  DIRDID_ROOT , "", 0, bitmap))
	FAIL (ntohl(AFPERR_ACCESS) != FPDelete(Conn2, vol2,  dir , name)) 
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
	if (!fork) {
		failed();
	}
	ret = FPDelete(Conn2, vol2,  dir , name);
	if (not_valid(ret, /* MAC */AFPERR_ACCESS, AFPERR_BUSY)) {
		failed();
	}

    filedir.access[1] = 3; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	FAIL (FPSetDirParms(Conn, vol, dir , "", bitmap, &filedir)) 
	FAIL (ntohl(AFPERR_BUSY) != FPDelete(Conn2, vol2,  dir , name))  
	if (!Mac) {
		sprintf(temp,"%s/%s/.AppleDouble/%s", Path, name1, name);
		if (chmod(temp, 0644) <0) {
			fprintf(stderr,"\tFAILED chmod(%s) %s\n", temp, strerror(errno));
			failed_nomsg();
		}
	}
	if (ntohl(AFPERR_BUSY) != FPDelete(Conn2, vol2,  dir , name)) { 
		fprintf(stderr,"\tFIXME FAILED open but deleted\n");
		failed_nomsg();
		
		FAIL (FPCloseFork(Conn,fork))
		FAIL (FPCreateFile(Conn, vol,  0, dir , name))
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
		if (!fork) {
			failed();
		}
	}

	FAIL (FPCloseFork(Conn,fork))
	fork1 = FPOpenFork(Conn2, vol2, OPENFORK_DATA , 0 ,dir , name,OPENACC_WR |OPENACC_RD);
	if (!fork1) {
		failed();
	}
	FAIL (ntohl(AFPERR_BUSY) != FPDelete(Conn, vol,  dir , name))
	FAIL (FPCloseFork(Conn2,fork1))
	
	FAIL (FPDelete(Conn, vol,  dir , name))
	FAIL (FPDelete(Conn, vol,  dir , ""))
	FAIL (FPCloseVol(Conn2,vol2))
}

/* ----------- */
void FPDelete_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDelete page 143\n");
    test146();
}

