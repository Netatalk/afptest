/* ----------------------------------------------
*/
#include "specs.h"

extern char  *Server;
extern int     Port;
extern char    *Password;
extern char *vers;
extern char *uam; 

static int sigp = 0;

static void pipe_handler()
{
	sigp = 1;
}

/* ------------------------- */
STATIC void test223()
{
char *name = "t223 file";
u_int16_t vol = VolID;
unsigned int ret;
struct sigaction action;    

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPzzz:test223: AFP 3.x enter sleep mode\n");
	if (Conn->afp_version < 30 || Conn2) {
		test_skipped(T_AFP3_CONN2);
		return;
	}

    action.sa_handler = pipe_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if ((sigaction(SIGPIPE, &action, NULL) < 0)) {
		nottested();
		return;
    }
	FAIL (FPzzz(Conn)) 
	fprintf(stderr,"sleep more than 2 mn\n");
	sleep(60 *3);
	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	if (sigp) {
		fprintf(stderr,"\tFAILED disconnected\n");
		failed_nomsg();
		/* try to reconnect */
		ret = FPopenLoginExt(Conn, vers, uam, User, Password);
		if (ret) {
    		nottested();
    		goto fin;
		}
		vol = VolID  = FPOpenVol(Conn, Vol);
		if (vol == 0xffff) {
    		nottested();
	    	goto fin;
		}
		FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	}    

	/* always there ? */
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
fin:
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if ((sigaction(SIGPIPE, &action, NULL) < 0)) {
		nottested();
    }
}

/* ------------------------- */
STATIC void test224()
{
char *name = "t224 file";
u_int16_t vol = VolID;
unsigned int ret;
struct sigaction action;    
DSI *dsi;
int sock;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPzzz:test224: disconnected after 2 mn\n");
	if (Conn->afp_version < 30 || Conn2) {
		test_skipped(T_AFP3_CONN2);
		return;
	}

	sigp = 0;
    action.sa_handler = pipe_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if ((sigaction(SIGPIPE, &action, NULL) < 0)) {
		nottested();
		return;
    }
	fprintf(stderr,"sleep more than 2 mn\n");
	sleep(60 *3);
	FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name);
	if (!sigp) {
		fprintf(stderr,"\tFAILED not disconnected\n");
		failed_nomsg();
	}
	else {
		/* try to reconnect */
    	dsi = &Conn->dsi;
		sock = OpenClientSocket(Server, Port);
    	if ( sock < 0) {
    		nottested();
    		goto fin;
    	}
    	dsi->protocol = DSI_TCPIP; 
		dsi->socket = sock;
		ret = FPopenLoginExt(Conn, vers, uam, User, Password);
		if (ret) {
    		nottested();
    		goto fin;
		}
		vol = VolID  = FPOpenVol(Conn, Vol);
		if (vol == 0xffff) {
    		nottested();
	    	goto fin;
		}
		FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) 
	}    

	/* always there ? */
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
fin:
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if ((sigaction(SIGPIPE, &action, NULL) < 0)) {
		nottested();
    }
	
}

/* ----------- */
void FPzzz_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPzzz undocumented\n");
    test223();
    test224();
}

