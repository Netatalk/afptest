/*
 * $Id: main.c,v 1.3 2005-05-25 18:03:32 didg Exp $
 *
 */

#include "afpclient.h"
#include "test.h"

int Verbose = 0;
int Quirk = 0;


/* =============================== */
CONN *Conn;
CONN *Conn2;

DSI *Dsi, *Dsi2;
DSI *dsi;

u_int16_t vol;
char Data[300000] = "Un essai un peu long -----------------------------\r---------------------------------\r"
                    "---------------------------------------\r";
/* ------------------------------- */
char    *Server = "localhost";
int     Proto = 0;
int     Port = 548;
char    *Password = "";
char    *Vol = "";
char    *User;
char    *User2;
char    *Path;
int     Version = 21;
int     Loop = 0;
int     Noadouble = 0;
int		Utf8 = 0;
int		Root = 0;

void test2(void)
{
    /* DSIGetStatus */
    fprintf(stderr,"===================\r");
    fprintf(stderr,"test2\r");
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_STAT; // DSIFUNC_OPEN;

	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	dsi->header.dsi_code = 0;
	dsi->header.dsi_len = 0;
	my_dsi_stream_send(dsi, NULL, 0);
	my_dsi_cmd_receive(dsi);
	dump_header(dsi);
}
/* ------------------------- */
void test4(int ac)
{
u_int16_t bitmap = 0;
int fork;

    fprintf(stderr,"===================\r");
    fprintf(stderr,"test4\r");
	if (ac >1) {
		fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto", OPENACC_WR | OPENACC_RD);
		if (0 != fork) {
			FPWrite(Conn,fork, 8000, 2048, Data, 0 /*0x80 */);
			FPFlushFork(Conn,fork);
			FPCloseFork(Conn,fork);
			return;
		}
	}
	else {
		FPDelete(Conn,vol,  DIRDID_ROOT , "toto");
		FPCreateFile(Conn,vol,  0, DIRDID_ROOT , "toto");
		fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
		FPWrite(Conn,fork, 0, 9000, Data, 0 /*0x80 */);
		FPFlushFork(Conn,fork);
				
//		fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "toto",OPENACC_WR | OPENACC_RD);
		FPRead(Conn,fork, 0, 10000, Data);
		getchar();
		FPCloseFork(Conn,fork);
	}
}

/* -------------------------- */
void test7()
{
u_int16_t bitmap = 0;
int fork;

	fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "mc-osdi.ps",OPENACC_RD);
	bitmap = 1 << FILPBIT_FNUM;
	FPGetForkParam(Conn,fork, bitmap);
	FPFlushFork(Conn,fork);
	FPRead(Conn,fork, 0, 277500, Data);
	FPRead(Conn,fork, 0, 277416, Data);
	FPCloseFork(Conn,fork);
#if 0
			FPCopyFile(Conn,vol, DIRDID_ROOT, vol, DIRDID_ROOT, "mc-osdi.ps", "mc-osdi.ps1");
			fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, ".pynche",OPENACC_RD);
			if (0 != fork) {
				FPCloseFork(Conn,fork);
			}
#endif			
}

/* -------------------------- */
void test8()
{
static char *uam = "Cleartxt Passwrd";
static char *usr = "didier";   /* FIXME username */
static char *opwd = "";  /* FIXME password */
static char *pwd =  "";  /* FIXME password */
int ret;

	ret = AFPChangePW(Conn,uam, usr, opwd, pwd);

	dump_header(dsi);

}

/* -------------------------- */
void test9()
{
u_int16_t bitmap = 0;
int fork;

	fork = FPOpenFork(Conn,vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, "spu/NOTES",OPENACC_RD);
	bitmap = 1 << FILPBIT_FNUM;
	FPGetForkParam(Conn,fork, bitmap);
	FPFlushFork(Conn,fork);
//	FPRead(Conn,fork, 0, 277500, data);
	FPCloseFork(Conn,fork);
}

/* ------------------------- */
void test10()
{
int  dir;
char *name = "test4";
	dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
}

/* ------------------------- */
void loginguest(void)
{
static char *vers = "AFPX03";
static char *uam = "No User Authent";

    fprintf(stderr,"===================\r");
    fprintf(stderr,"test3\r");
	FPopenLogin(Conn,vers, uam, "", "");
}

/* =============================== */
void usage( char * av0 )
{
    fprintf( stderr, "usage:\t%s [-n] [-t] [-h host] [-p port] [-s vol] [-u user] [-w password]\n", av0 );
    fprintf( stderr,"\t-n\tasp protocol (default tcp/ip)\n");
    fprintf( stderr,"\t-h\tserver host name (default localhost)\n");
    fprintf( stderr,"\t-p\tserver port (default 548)\n");

    fprintf( stderr,"\t-s\tvolume to mount (default home)\n");
    fprintf( stderr,"\t-c\tvolume path on the server\n");

    fprintf( stderr,"\t-u\tuser name (default uid)\n");
    fprintf( stderr,"\t-w\tpassword (default none)\n");

    fprintf( stderr,"\t-3\tAFP 3.0 version\n");
    fprintf( stderr,"\t-4\tAFP 3.1 version\n");
    fprintf( stderr,"\t-5\tAFP 3.2 version\n");
    fprintf( stderr,"\t-v\tverbose\n");

    fprintf( stderr,"\t-a\tall tests\n");
    fprintf( stderr,"\t-m\ttests passing on a Mac\n");
    fprintf( stderr,"\t-l\trun test in loop\n");
    fprintf( stderr,"\t-d\tSecond user for two connections (same password!)\n");
    fprintf( stderr,"\t-o\tnoadouble tests\n");
    fprintf( stderr,"\t-x\tutf8 tests\n");
    fprintf( stderr,"\t-Q\enable Mac OS quirks\n");
    exit (1);
}

/* ------------------------------- */
int main( ac, av )
int		ac;
char	**av;
{
int cc;
int all = 0;
int mac = 0;
//	static char *vers = "AFP2.2";
static char *vers = "AFPVersion 2.1";
static char *uam = "Cleartxt Passwrd";

    while (( cc = getopt( ac, av, "Qlvxrmd:345anoth:p:s:u:w:c:" )) != EOF ) {
        switch ( cc ) {
		case 'Q':
			Quirk = 1;
			break;
        case 'm':
        	mac = 1;
        	break;
        case '3':
			vers = "AFPX03";
			Version = 30;
			break;
        case '4':
			vers = "AFP3.1";
			Version = 31;
			break;
        case '5':
			vers = "AFP3.2";
			Version = 32;
			break;
		case 'c':
			Path = strdup(optarg);
			break;
        case 'n':
            Proto = 1;
            break;
        case 'o':
        	Noadouble = 1;
        	break;
        case 'r':
        	Root = 1;
        	break;
        case 'x':
        	Utf8 = 1;
        	break;
        case 'h':
            Server = strdup(optarg);
            break;
        case 's':
            Vol = strdup(optarg);
            break;
        case 'u':
            User = strdup(optarg);
            break;
        case 'd':
            User2 = strdup(optarg);
            break;
        case 'w':
            Password = strdup(optarg);
            break;
        case 'l' :
            Loop = 1;
            break;
        case 'p' :
            Port = atoi( optarg );
            if (Port <= 0) {
                fprintf(stderr, "Bad port.\n");
                exit(1);
            }
            break;
		case 'a':
			all = 1;
			break; 
		case 'v':
			Verbose = 1;
			break;
        default :
            usage( av[ 0 ] );
        }
    }

	/************************************
	 *                                  *
	 * Connection user 1                *
	 *                                  *
	 ************************************/

    if ((Conn = (CONN *)calloc(1, sizeof(CONN))) == NULL) {
    	return 1;
    }
    Conn->type = Proto;
    if (!Proto) {
	int sock;
    	Dsi = &Conn->dsi;
		dsi = Dsi;         
	    sock = OpenClientSocket(Server, Port);
        if ( sock < 0) {
	    	return 1;
        }
     	Dsi->protocol = DSI_TCPIP; 
	    Dsi->socket = sock;
    }
    else {
	}

    /* login */	
	FPopenLogin(Conn, vers, uam, User, Password);
	Conn->afp_version = Version;

	
	/***************************************
	 *                                     *
	 * User 2                              *
	 *                                     *
	 ***************************************/
	/* user 2 */
	if (User2) {
    	if ((Conn2 = (CONN *)calloc(1, sizeof(CONN))) == NULL) {
    		return 1;
    	}
	    Conn2->type = Proto;
    	if (!Proto) {
		int sock;
    		Dsi2 = &Conn2->dsi;
         
	    	sock = OpenClientSocket(Server, Port);
	        if ( sock < 0) {
		    	return 1;
        	}
	     	Dsi2->protocol = DSI_TCPIP; 
		    Dsi2->socket = sock;
	    }
    	else {
		}
    	/* login */	
    	if (Version >= 30) {
			FPopenLoginExt(Conn2, vers, uam, User2, Password);
		}
    	else {
			FPopenLogin(Conn2, vers, uam, User2, Password);
		}
		Conn2->afp_version = Version;
	}
    /***********************************
     *                                 *
     * Run test                        *
     *                                 *
     **********************************/
    if (Utf8) {
    	run_utf8();
    }
    else if (Root) {
    	run_root();
    }
    else if (Noadouble) {
    	run_noadouble();
    }
    else if (Loop) {
    	run_loop();
    }
	else if (mac) {
		run_common();
	} 
	else if (all) {
	    if (User2) {
	    	run_all_double();
	    }
	    else {
	    	run_all();
	    }
	}
	else  {
		run_one();
	}

   	FPLogOut(Conn);
#if 0
	if (User2) {
		FPLogOut(Conn2);
	}
#endif	
	return 0;
}
