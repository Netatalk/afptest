/*
 * $Id: T2_spectest.c,v 1.10 2010-01-21 08:35:34 didg Exp $
 * MANIFEST
 */
#include "afpclient.h"
#include "test.h"
#include "volinfo.h"
#include <dlfcn.h>

int Verbose = 0;
int Quirk = 0;
int Interactive = 0;

u_int16_t VolID;
static DSI *dsi;
CONN *Conn;
CONN *Conn2;
extern void nottested(void);

int ExitCode = 0;
int Locking;

#define FN(a) a ## _test
#define EXT_FN(a) extern void FN(a) (void)
#if 0
EXT_FN(FPAddComment);
EXT_FN(FPByteRangeLock);
EXT_FN(FPByteRangeLockExt);
EXT_FN(FPCloseFork);
EXT_FN(FPCreateDir);
EXT_FN(FPCreateFile);
EXT_FN(FPEnumerate);
EXT_FN(FPExchangeFiles);
EXT_FN(FPGetComment);
EXT_FN(FPGetForkParms);
EXT_FN(FPGetVolParms);
EXT_FN(FPGetUserInfo);
EXT_FN(FPMapName);
EXT_FN(FPOpenDir);
EXT_FN(FPSetFileDirParms);
EXT_FN(FPSetForkParms);
EXT_FN(FPRead);
EXT_FN(FPRemoveComment);
EXT_FN(FPRename);
EXT_FN(Error);
#endif

EXT_FN(FPByteRangeLock);
EXT_FN(FPCopyFile);
EXT_FN(FPCreateFile);
EXT_FN(FPDelete);
EXT_FN(FPGetFileDirParms);
EXT_FN(FPMoveAndRename);
EXT_FN(FPOpenFork);
EXT_FN(FPSetDirParms);
EXT_FN(FPSetFileParms);
EXT_FN(FPResolveID);
EXT_FN(Dircache_attack);

struct test_fn {
char *name;
void (*fn)(void);

};
#define FN_N(a) { # a , FN(a) },

static struct test_fn Test_list[] =
{
#if 0
#ifdef QUIRK
FN_N(FPEnumerate)
#else
FN_N(FPAddComment)
FN_N(FPByteRangeLock)
FN_N(FPByteRangeLockExt)
FN_N(FPCloseFork)
FN_N(FPCreateDir)
FN_N(FPCreateFile)
FN_N(FPCopyFile)
FN_N(FPEnumerate)
FN_N(FPExchangeFiles)
FN_N(FPGetComment)
FN_N(FPGetForkParms)
FN_N(FPGetVolParms)
FN_N(FPGetUserInfo)
FN_N(FPMapName)
FN_N(FPOpenDir)
FN_N(FPOpenFork)
FN_N(FPSetFileDirParms)
FN_N(FPSetForkParms)
FN_N(FPRead)
FN_N(FPRemoveComment)
FN_N(FPRename)
FN_N(FPResolveID)
FN_N(Error)
#endif
#endif
FN_N(FPByteRangeLock)
FN_N(FPCreateFile)
FN_N(FPCopyFile)
FN_N(FPDelete)
FN_N(FPGetFileDirParms)
FN_N(FPMoveAndRename)
FN_N(FPOpenFork)
FN_N(FPSetDirParms)
FN_N(FPSetFileParms)
FN_N(FPResolveID)
FN_N(Dircache_attack)

{NULL, NULL},
};

/* =============================== */
static void press_enter(char *s)
{
    if (!Interactive)
	return;
	
    if (s) 
	fprintf(stderr, "--> Performing: %s\n", s);
    fprintf(stderr, "Press <ENTER> to continue.\n");
    
    while (fgetc(stdin) != '\n') 
	;
}

/* =============================== */
static void list_tests(void)
{
int i = 0;
	while (Test_list[i].name != NULL) {
		fprintf(stderr, "%s\n", Test_list[i].name);
		i++;
	}
}

#if 0
/* ----------- */
static void run_one(char *name)
{
int i = 0;
	while (Test_list[i].name != NULL) {
		if (!strcmp(Test_list[i].name, name))
			break;
		i++;
	}
	if (Test_list[i].name == NULL) {
		nottested();
		return;
	}

	dsi = &Conn->dsi;
	press_enter("Opening volume.");
	VolID = FPOpenVol(Conn, Vol);
	if (VolID == 0xffff) {
		nottested();
		return;
	}
	Test_list[i].fn();

	FPCloseVol(Conn,VolID);
}
#endif

/* ----------- */
static void run_one(char *name)
{
int i = 0;
void *handle = NULL;
void (*fn)(void) = NULL;
char *error;
char *token;

    token = strtok(name, ",");
    
	while (Test_list[i].name != NULL) {
		if (!strcmp(Test_list[i].name, name))
			break;
		i++;
	}
	if (Test_list[i].name == NULL) {
		handle = dlopen (NULL, RTLD_LAZY);
        if (handle) {
			fn = dlsym(handle, token);
			if ((error = dlerror()) != NULL)  {
			    fprintf (stderr, "%s\n", error);
			}
        }
        else {
        	fprintf (stderr, "%s\n", dlerror());
        }
        if (!handle || !fn) {
			nottested();
			return;
		}
	}
	else {
		fn = Test_list[i].fn;
	}

	dsi = &Conn->dsi;
	press_enter("Opening volume.");
	VolID = FPOpenVol(Conn, Vol);
	if (VolID == 0xffff) {
		nottested();
		return;
	}
	
	while (token ) {
	    (*fn)();
	    token = strtok(NULL, ",");
	    if (token && handle) {
			fn = dlsym(handle, token);
			if ((error = dlerror()) != NULL)  {
			    fprintf (stderr, "%s\n", error);
			}
	    }
	}

	if (handle)
		dlclose(handle);

	FPCloseVol(Conn,VolID);
}
                                                                            
/* ----------- */
static void run_all()
{
int i = 0;

	dsi = &Conn->dsi;
	press_enter("Opening volume.");
	VolID = FPOpenVol(Conn, Vol);
	if (VolID == 0xffff) {
		nottested();
		return;
	}
	while (Test_list[i].name != NULL) {
		Test_list[i].fn();
		i++;
	}

	FPCloseVol(Conn,VolID);
}

DSI *Dsi, *Dsi2;

char Data[300000] = "";
/* ------------------------------- */
char    *Server = "localhost";
char    *Server2;
int     Proto = 0;
int     Port = 548;
char    *Password = "";
char    *Vol = "";
char    *Vol2;
char    *User;
char    *User2;
char    *Path;
int     Version = 21;
int     List = 0;
int     Mac = 0;
char    *Test;
int     Manuel = 0;

/* =============================== */
void usage( char * av0 )
{
    fprintf( stderr, "usage:\t%s [-m] [-n] [-t] [-h host] [-p port] [-s vol] [-u user] [-w password] -f [call]\n", av0 );
    fprintf( stderr,"\t-L\tserver without working fcntl locking, skip tests using it\n");
    fprintf( stderr,"\t-m\tserver is a Mac\n");
    fprintf( stderr,"\t-h\tserver host name (default localhost)\n");
    fprintf( stderr,"\t-p\tserver port (default 548)\n");
    fprintf( stderr,"\t-s\tvolume to mount (default home)\n");
    fprintf( stderr,"\t-c\tvolume path on the server\n");
    fprintf( stderr,"\t-u\tuser name (default uid)\n");
    fprintf( stderr,"\t-d\tsecond user for two connections (same password!)\n");
    fprintf( stderr,"\t-H\tsecond server for two connections (default use only one server)\n");
    fprintf( stderr,"\t-S\tsecond volume (default none)\n");

    fprintf( stderr,"\t-w\tpassword (default none)\n");
    fprintf( stderr,"\t-2\tAFP 2.2 version (default 2.1)\n");
    fprintf( stderr,"\t-3\tAFP 3.0 version\n");
    fprintf( stderr,"\t-4\tAFP 3.1 version\n");
    fprintf( stderr,"\t-5\tAFP 3.2 version\n");
    fprintf( stderr,"\t-v\tverbose\n");

    fprintf( stderr,"\t-f\ttest to run\n");
    fprintf( stderr,"\t-l\tlist tests\n");
    fprintf( stderr,"\t-i\tinteractive mode, prompts before every test (debug purposes)\n");
    exit (1);
}

/* ------------------------------- */
int main( ac, av )
int		ac;
char	**av;
{
int cc;
static char *vers = "AFPVersion 2.1";
static char *uam = "Cleartxt Passwrd";

    while (( cc = getopt( ac, av, "iv2345h:H:p:s:u:d:w:c:f:lmMS:L" )) != EOF ) {
        switch ( cc ) {
        case '2':
			vers = "AFP2.2";
			Version = 22;
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
            loadvolinfo(Path);
			break;
		case 'm':
			Mac = 1;
			break;
		case 'L':
			Locking = 1;
			break;
        case 'n':
            Proto = 1;
            break;
        case 'h':
            Server = strdup(optarg);
            break;
        case 's':
            Vol = strdup(optarg);
            break;
        case 'S':
            Vol2 = strdup(optarg);
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
            List = 1;
            break;
        case 'f' :
            Test = strdup(optarg);
            break;
	case 'i':
		Interactive = 1;
		break;
        case 'p' :
            Port = atoi( optarg );
            if (Port <= 0) {
                fprintf(stderr, "Bad port.\n");
                exit(1);
            }
            break;
		case 'v':
			Verbose = 1;
			break;
		case 'M':
			Manuel = 1;
			break;
        default :
            usage( av[ 0 ] );
        }
    }
	if (List) {
		list_tests();
		exit (2);
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
	    	return 2;
        }
     	Dsi->protocol = DSI_TCPIP; 
	    Dsi->socket = sock;
    }
    else {
	}

    /* login */	
    if (Version >= 30) {
		FPopenLoginExt(Conn, vers, uam, User, Password);
	}
	else {
		FPopenLogin(Conn, vers, uam, User, Password);
	}
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
         
	    	sock = OpenClientSocket(Server2?Server2:Server, Port);
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
	/*********************************
	*/
	if (Test != NULL) {
		run_one(Test);
	}
	else {
		run_all();
	}

   	FPLogOut(Conn);

	if (User2) {
		FPLogOut(Conn2);
	}
	return ExitCode;
}
