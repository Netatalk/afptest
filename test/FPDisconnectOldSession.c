/* ----------------------------------------------
*/
#include "specs.h"

extern char  *Server;
extern int     Port;
extern char    *Password;
extern char *vers;
extern char *uam; 

/* ------------------------- */
STATIC void test222()
{
char *name = "t222 file";
u_int16_t vol = VolID,vol2;
unsigned int ret;
char *token;
u_int32_t len;
CONN *conn;
DSI *dsi3;
int sock;
int fork = 0, fork1;
struct sigaction action;    

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDisconnectOldSession :test222: AFP 3.x disconnect old session\n");
	if (Conn->afp_version < 30 || Conn2) {
		test_skipped(T_AFP3_CONN2);
		return;
	}

	ret = FPGetSessionToken(Conn, 0, 0, 0, NULL);
	if (ret) {
		failed();
		return;
	}
    if ((conn = (CONN *)calloc(1, sizeof(CONN))) == NULL) {
    	nottested();
    	return;
    }
    conn->type = 0;
    dsi3 = &conn->dsi;
	sock = OpenClientSocket(Server, Port);
    if ( sock < 0) {
    	nottested();
    	return;
    }
    dsi3->protocol = DSI_TCPIP; 
	dsi3->socket = sock;
	ret = FPopenLoginExt(conn, vers, uam, User, Password);
	if (ret) {
    	nottested();
    	return;
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
		return;
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
	ret =  FPDisconnectOldSession(Conn, 0, len +4, token);
	if (ret != htonl(AFPERR_MISC)) {
		failed();
		goto fin;
	}

	ret =  FPDisconnectOldSession(Conn, 0, len, token);


	if (ret != htonl(AFPERR_SESSCLOS)) {
		failed();
		goto fin;
	}

	fork1 = FPOpenFork(Conn, vol, OPENFORK_RSCS , 0 ,DIRDID_ROOT, name, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork1) {
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
	    
}

/* ----------- */
void FPDisconnectOldSession_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPDisconnectOldSession page 148\n");
    test222();
}

