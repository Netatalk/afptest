/* ----------------------------------------------
*/
#include "specs.h"

/* ------------------------- */
STATIC void test220()
{
char *name = "t23 dir";
char *name1 = "t23 subdir";
char *name2 = "t23 file";
int  dir,dir1;
u_int16_t vol = VolID;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken:test220: AFP 3.0 get session token\n");
	if (Conn->afp_version != 30) {
		test_skipped(T_AFP3);
		goto test_exit;
	}

	FAIL (FPGetSessionToken(Conn,0 ,0 ,0 ,NULL))
	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		goto test_exit;
	}

	if (Conn->afp_version < 30) {
		if (ntohl(AFPERR_NOOP) != FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0xffff)) {
			failed();
		}
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
		goto test_exit;
	}	
	FAIL (FPGetSrvrInfo(Conn))
	dir1 = FPCreateDir(Conn,vol, dir , name1);
	if (dir1) {
		FAIL (FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT)))
		FAIL (FPCreateFile(Conn, vol,  0, dir1 , name2))
		FAIL (FPGetFileDirParams(Conn, vol,  dir , name1, 0, (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT)))
		if (FPEnumerate_ext(Conn, vol,  dir1 , "", 
			                (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		        	            |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN),
	                        	(1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT)
		                     )) {
			failed();
		}
		if (FPEnumerate_ext(Conn, vol,  dir , "", 
		                (1 << FILPBIT_PDINFO )| (1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN) | (1 << FILPBIT_LNAME),
	                        (1 << DIRPBIT_PDINFO ) | (1 << DIRPBIT_OFFCNT) | (1 << DIRPBIT_LNAME)
	                     )) {
			failed();
		}	                   
		FAIL (FPDelete(Conn, vol,  dir1, name2))
	}
	else {
		failed();
	}
	if (dir) {
		FAIL (FPDelete(Conn, vol,  dir, name1))
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
	}
test_exit:
	exit_test("test220");
}

/* ------------------------- */
STATIC void test221()
{
	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken:test221: AFP 3.1 get session token\n");
	if (Conn->afp_version != 31) {
		test_skipped(T_AFP3);
		goto test_exit;
	}

	FAIL (FPGetSessionToken(Conn, 3, 0, 5, "token"))
        FAIL (FPzzz(Conn, 0))
test_exit:
	exit_test("test221");
}

/* ------------------------- */
STATIC void test228()
{
#if 0
char *name = "t228 file";
u_int16_t vol = VolID,vol2;
unsigned int ret;
char *token;
u_int32_t len;
CONN *conn;
DSI *dsi3;
int sock;
int fork = 0, fork1;
struct sigaction action;    

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDisconnectOldSession :test228: AFP 3.x disconnect old session\n");
	if (Conn->afp_version < 30 || Conn2) {
		test_skipped(T_AFP3_CONN2);
		goto test_exit;
	}

	ret = FPGetSessionToken(Conn, 0, 0, 0, NULL);
	if (ret) {
		failed();
		goto test_exit;
	}
    if ((conn = (CONN *)calloc(1, sizeof(CONN))) == NULL) {
    	nottested();
		goto test_exit;
    }
    conn->type = 0;
    dsi3 = &conn->dsi;
	sock = OpenClientSocket(Server, Port);
    if ( sock < 0) {
    	nottested();
		goto test_exit;
    }
    dsi3->protocol = DSI_TCPIP; 
	dsi3->socket = sock;
	ret = FPopenLoginExt(conn, vers, uam, User, Password);
	if (ret) {
    	nottested();
		goto test_exit;
	}
	conn->afp_version = Conn->afp_version;

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 

	vol2  = FPOpenVol(conn, Vol);
	if (vol2 == 0xffff) {
		failed();
		goto fin;
	}
	fork = FPOpenFork(conn, vol2, OPENFORK_RSCS , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		failed();
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (fork1) {
		FAIL (FPCloseFork(Conn,fork1))
		failed();
		goto fin;
	}
	
	ret = FPGetSessionToken(conn, 0, 0, 0, NULL);
	if (ret) {
		failed();
		goto fin;
		goto test_exit;
	}
	memcpy(&len, dsi3->data, sizeof(u_int32_t)); 
	len = ntohl(len);
	if (!len) {
		failed();
		goto fin;
	}
	if (!(token = malloc(len +4))) {
		fprintf(stderr, "\tFAILED malloc(%x) %s\n", len, strerror(errno));
		failed_nomsg();
		goto fin;
	}

    action.sa_handler =  SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART | SA_ONESHOT;
    if (sigaction(SIGPIPE, &action, NULL) < 0) {
    	failed();
    	goto fin;
    }

	memcpy(token, dsi3->data + sizeof(u_int32_t), len);
	/* wrong token */
	ret =  FPDisconnectOldSession(Conn, 0, len +4, token);
	if (ret != htonl(AFPERR_MISC)) {
		failed();
	}

	ret =  FPDisconnectOldSession(Conn, 0, len, token);

	if (ret != htonl(AFPERR_SESSCLOS)) {
		failed();
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork1) {
	    /* arg we are there */
		failed();
		goto fin;
	}
	FAIL (FPCloseFork(Conn,fork1))
	
fin:
	FAIL (FPLogOut(conn))
    action.sa_handler =  SIG_DFL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGPIPE, &action, NULL) < 0) {
    	failed();
    }
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
test_exit:
	exit_test("test228");
#endif
}

/* ----------- */
void FPGetSessionToken_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken page 191\n");
    test220();
    test221();
    test228();
}

