/*
 * $Id: loadtest.c,v 1.7 2010-03-25 07:56:34 franklahm Exp $
 * MANIFEST
 */

#include <sys/time.h>
#include <time.h>

#include "afpclient.h"
#include "test.h"
#include "specs.h"

int Verbose = 0;
int Quirk = 0;

static u_int16_t vol;
static DSI *dsi;
CONN *Conn;

int ExitCode = 0;

/* =============================== */

DSI *Dsi;

char Data[300000] = "";
/* ------------------------------- */
char    *Server = NULL;
int     Proto = 0;
int     Port = 548;
char    *Password = "";
char    *Vol = "";
char    *User;
char    *Path;
int     Version = 21;
int     Mac = 0;
int     Iterations = 1;

extern  int     Throttle;

struct timeval tv_start;
struct timeval tv_end;
struct timeval tv_dif;
#define RESBUF 64000
char results[RESBUF+1];

static void starttimer(void)
{
    gettimeofday(&tv_start, NULL);
}

static void stoptimer(void)
{
    gettimeofday(&tv_end, NULL);
}

static char *timerstr(void)
{
    static char buf[100];

    if (tv_end.tv_usec < tv_start.tv_usec) {
        tv_end.tv_usec += 1000000;
        tv_end.tv_sec -= 1;
    }
    tv_dif.tv_sec = tv_end.tv_sec - tv_start.tv_sec;
    tv_dif.tv_usec = tv_end.tv_usec - tv_start.tv_usec;

    snprintf(buf, 99, "%u.%03u s", tv_dif.tv_sec, tv_dif.tv_usec / 1000);
    return buf;
}

static void addemptyline(void)
{
    strncat(results, "\n", RESBUF);
}

static void addresult(const char *msg)
{
    int i;
    strncat(results, msg, RESBUF);
    for (i = 60 - strlen(msg); i > 0; i--)
        strncat(results, " ", RESBUF);
    strncat(results, timerstr(), RESBUF);
    strncat(results, "\n", RESBUF);
}

static void displayresults(void)
{
    printf("Netatalk Lantest Results\n");
    printf("========================\n\n");
    printf("%s", results);
}

/* ------------------------- */
void failed(void)
{
	fprintf(stderr,"\tFAILED\n");
	if (!ExitCode)
		ExitCode = 1;
}

/* ------------------------- */
void fatal_failed(void)
{
	fprintf(stderr,"\tFAILED\n");
	exit(1);
}
/* ------------------------- */
void nottested(void)
{
	fprintf(stderr,"\tNOT TESTED\n");
	if (!ExitCode)
		ExitCode = 2;
}

/* --------------------------------- */
int is_there(CONN *conn, int did, char *name)
{
	return FPGetFileDirParams(conn, vol,  did, name, 
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
	         ,
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
		);
}

/* ------------------------- */
void test143()
{
int id = getpid();
char *ndir = NULL;
int dir;
int i,maxi = 0;
int j, k;
int fork;
char *data = NULL;
int nowrite;
int numread = /*2*/ 469;
static char temp[MAXPATHLEN];   

    fprintf(stderr,"===================\n");
    fprintf(stderr,"test143: LanTest\n");
	sprintf(temp,"LanTest.tmp.32.-%d", id);
	
	ndir = strdup(temp);
	data = calloc(1, 65536);
	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, DIRDID_ROOT, ndir)) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, "", 0
	         , (1<< DIRPBIT_DID) )) {
		nottested();
		goto fin;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		nottested();
		goto fin;
	}

	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, dir, "File.big")) {
		failed();
		goto fin1;
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "", 0
	         , (1<< DIRPBIT_DID) )) {
		failed();
		goto fin1;
	}
	if (FPCreateFile(Conn, vol,  0, dir , "File.big")){
		failed();
		goto fin1;
	}

	/* --------------- */
	for (i=1; i <= 100; i++) {
		sprintf(temp, "File.small%d", i);
		if (ntohl(AFPERR_NOOBJ) != is_there(Conn, dir, temp)) {
			fatal_failed();
		}
		else {
			if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
				fatal_failed();
			}
		    if (FPCreateFile(Conn, vol,  0, dir , temp)){
				fatal_failed();
			}
			if (is_there(Conn, dir, temp)) {
				fatal_failed();
			}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	    	     	(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
		    	     (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	            , 0)) {
				fatal_failed();
			}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	    	     	(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
	    	     	 (1<< DIRPBIT_ATTR)|(1<<DIRPBIT_BDATE)|
		    	     (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	            , 0)) {
				fatal_failed();
			}
			fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			            (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
			if (!fork) {
				fatal_failed();
			}
			else {
				if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))
				) {
					fatal_failed();
				}
				if (FPWrite(Conn, fork, 0, 20480, data, 0 )) {
					fatal_failed();
				}
				if (FPCloseFork(Conn,fork)) {fatal_failed();}
			}
		}
		maxi = i;
	}

	if (FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fatal_failed();
	}

    starttimer();
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.small%d", i);
		if (is_there(Conn, dir, temp)) {
			fatal_failed();
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
			fatal_failed();
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
			fatal_failed();
		}
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342, dir, temp, OPENACC_RD);
		if (!fork) {
			fatal_failed();
		}
		else {
			if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fatal_failed();}
			if (FPRead(Conn, fork, 0, 512, data)) {fatal_failed();}
			if (FPCloseFork(Conn,fork)) {fatal_failed();}
		}
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_DWR);
		if (!fork) {
			fatal_failed();
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fatal_failed();}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
				fatal_failed();
			}
			if (FPCloseFork(Conn,fork)) {fatal_failed();}
		}
	}
    stoptimer();
    addresult("Opening, stating and reading 512 bytes from 200 files: ");

	/* ---------------- */
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.small%d", i);
		if (is_there(Conn, dir, temp)) {
			fatal_failed();
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0, (1<< FILPBIT_FNUM) )) {
			fatal_failed();
		}

		if (FPDelete(Conn, vol,  dir, temp)) {fatal_failed();}
	}
	    
 	if (FPGetVolParam(Conn, vol, (1 << VOLPBIT_MDATE )|(1 << VOLPBIT_XBFREE))) {
		fatal_failed();
	}

	/* --------------- */
	strcpy(temp, "File.big");
	if (is_there(Conn, dir, temp)) {fatal_failed();}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {fatal_failed();}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
		fatal_failed();
	}
	nowrite = 0;
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			            (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
	if (!fork) {
		fatal_failed();
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))) {
			fatal_failed();
		}
        starttimer();
		for (i=0; i <= numread ; i++) {
			if (FPWrite(Conn, fork, i*65536, 65536, data, 0 )) {
				fatal_failed();
				nowrite = 1;
			}
		}
		if (FPCloseFork(Conn,fork)) {fatal_failed();}
        stoptimer();
        addresult("Writing 30 MB to one file: ");        
	}

	if (is_there(Conn, dir, temp)) {fatal_failed();}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d, 0)) {fatal_failed();}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f )) {
		fatal_failed();
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD);

	if (!fork) {
		fatal_failed();
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fatal_failed();}
		if (FPRead(Conn, fork, 0, 512, data)) {fatal_failed();}
		if (FPCloseFork(Conn,fork)) {fatal_failed();}
	}
	if (!nowrite) {	
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_DWR);
		if (!fork) {
			fatal_failed();
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fatal_failed();}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d,0)) {
				fatal_failed();
			}
            starttimer();
			for (i=0; i <= numread ; i++) {
				if (FPRead(Conn, fork, i*65536, 65536, data)) {
					fatal_failed();
				}
			}
			if (FPCloseFork(Conn,fork)) {fatal_failed();}
            stoptimer();
            addresult("Reading 30 MB from one file: ");
		}
	}

	/* --------------- */
	strcpy(temp, "File.lock");
	
	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, dir, temp)) {
		fatal_failed();
	}
	if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
		fatal_failed();
	}
	if (FPCreateFile(Conn, vol,  0, dir , temp)){ fatal_failed();}

	if (is_there(Conn, dir, temp)) {fatal_failed();}

	if (FPGetFileDirParams(Conn, vol,  dir, temp, 
	 		(1<<FILPBIT_FNUM )|(1<<FILPBIT_PDID)|(1<<FILPBIT_FINFO)|
		    (1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
	        , 0)) {
		fatal_failed();
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 
			(1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_FNUM)|(1<<FILPBIT_DFLEN)
			            , dir, temp, OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);

	if (!fork) {
		fatal_failed();
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_PDID)|(1<< DIRPBIT_LNAME)|(1<<FILPBIT_DFLEN))
				) {
			fatal_failed();
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 
				(1<< DIRPBIT_ATTR)|(1<<FILPBIT_CDATE)|(1<<FILPBIT_MDATE)|
				(1<<FILPBIT_FNUM)|
		 		(1<<FILPBIT_FINFO)|(1<<FILPBIT_DFLEN)|(1<<FILPBIT_RFLEN)
		        , 0)) {
			fatal_failed();
		}
		if (FPWrite(Conn, fork, 0, 40000, data, 0 )) {
			fatal_failed();
		}
		if (FPCloseFork(Conn,fork)) {fatal_failed();}
	}
	/* -------------- */
	if (is_there(Conn, dir, temp)) {fatal_failed();}
	if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x73f, 0x133f)) {
		fatal_failed();
	}
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD);

	if (!fork) {
		fatal_failed();
	}
	else {
		if (FPGetForkParam(Conn, fork, (1<<FILPBIT_DFLEN))) {fatal_failed();}
		if (FPRead(Conn, fork, 0, 512, data)) {fatal_failed();}
		if (FPCloseFork(Conn,fork)) {fatal_failed();}
		/* ----------------- */
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , 0x342 , dir, temp,OPENACC_RD| OPENACC_WR);

		if (!fork) {
			fatal_failed();
		}
		else {
			if (FPGetForkParam(Conn, fork, 0x242)) {fatal_failed();}
			if (FPGetFileDirParams(Conn, vol,  dir, temp, 0x72d, 0)) {
				fatal_failed();
			}
            starttimer();
			for (j = 0; j < 100; j++) {
				for (i = 0;i <= 390; i += 10) {
					if (FPByteLock(Conn, fork, 0, 0 , i , 10)) {fatal_failed();}
				}	
				for (i = 390;i >= 0; i -= 10) {
					if (FPByteLock(Conn, fork, 0, 1 , i , 10)) {fatal_failed();}
				}
			}	
            stoptimer();
            addresult("Locking/Unlocking 4000 times each: ");

			if (is_there(Conn, dir, temp)) {fatal_failed();}
			if (FPCloseFork(Conn,fork)) {fatal_failed();}
			if (FPDelete(Conn, vol,  dir, "File.lock")) {fatal_failed();}
		}
	}		

	/* --------------- */
	for (i=1; i <= 320; i++) {
		sprintf(temp, "File.0k%d", i);
		if (ntohl(AFPERR_NOOBJ) != is_there(Conn, dir, temp)) {
			fatal_failed();
		}
		else {
			if (FPGetFileDirParams(Conn, vol,  dir, "", 0, (1<< DIRPBIT_DID) )) {
				fatal_failed();
			}
		    if (FPCreateFile(Conn, vol,  0, dir , temp)){
				fatal_failed();
				break;
			}
		}
		maxi = i;
	}

    starttimer();
	if (FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE) |
	         (1<<FILPBIT_DFLEN) | (1<<FILPBIT_RFLEN)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)) {
		fatal_failed();
	}
    stoptimer();
    addresult("Enumerate dir with 320 files: ");

	/* ---------------- */
	for (i=1; i <= maxi; i++) {
		sprintf(temp, "File.0k%d", i);
		if (is_there(Conn, dir, temp)) {
			fatal_failed();
		}
		if (FPGetFileDirParams(Conn, vol,  dir, temp, 0, (1<< FILPBIT_FNUM) )) {
			fatal_failed();
		}

		if (FPDelete(Conn, vol,  dir, temp)) {fatal_failed();}
	}

	/* ---------------- */
    /* Create a DIRNUM^3 nested dirtree */

#define DIRNUM 10
    uint32_t idirs[DIRNUM];
    uint32_t jdirs[DIRNUM][DIRNUM];
    uint32_t kdirs[DIRNUM][DIRNUM][DIRNUM];

    starttimer();
    for (i=0; i < DIRNUM; i++) {
        sprintf(temp, "dir%02u", i+1);
        FAILEXIT(!(idirs[i] = FPCreateDir(Conn,vol, dir, temp)), fin1);

        for (j=0; j < DIRNUM; j++) {
            sprintf(temp, "dir%02u", j+1);
            FAILEXIT(!(jdirs[i][j] = FPCreateDir(Conn,vol, idirs[i], temp)), fin1);

            for (k=0; k < DIRNUM; k++) {

                sprintf(temp, "dir%02u", k+1);
                FAILEXIT(!(kdirs[i][j][k] = FPCreateDir(Conn,vol, jdirs[i][j], temp)), fin1);
            }
        }
    }
    stoptimer();
    addresult("Created directory tree with 10^3 dirs: ");
    
    for (i=0; i < DIRNUM; i++) {
        for (j=0; j < DIRNUM; j++) {
            for (k=0; k < DIRNUM; k++) {
                FAILEXIT(FPDelete(Conn,vol, kdirs[i][j][k], "") != 0, fin1);
            }
            FAILEXIT(FPDelete(Conn,vol, jdirs[i][j], "") != 0, fin1);
        }
        FAILEXIT(FPDelete(Conn,vol, idirs[i], "") != 0, fin1);
    }

fin1:
	if (FPDelete(Conn, vol,  dir, "File.big")) {failed();}
	if (FPDelete(Conn, vol,  dir, "")) {failed();}
fin:
	free(ndir);
	free(data);

    addemptyline();
}

/* ------------------ */
static void run_one()
{
	dsi = &Conn->dsi;
	vol  = FPOpenVol(Conn, Vol);
	if (vol == 0xffff) {
		nottested();
		return;
	}
    while (Iterations--)
        test143();
}

/* =============================== */
void usage( char * av0 )
{
    fprintf( stderr, "usage:\t%s -h host [-m|t|v|V] [-3|4|5] [-p port] [-s vol] [-u user] [-w password] [-n iterations]\n", av0 );
    fprintf( stderr,"\t-t\tthrottle execution (ignore this!)\n");
    fprintf( stderr,"\t-m\tserver is a Mac (ignore this too!)\n");
    fprintf( stderr,"\t-h\tserver host name (default localhost)\n");
    fprintf( stderr,"\t-p\tserver port (default 548)\n");
    fprintf( stderr,"\t-s\tvolume to mount (default home)\n");
    fprintf( stderr,"\t-u\tuser name (default uid)\n");
    fprintf( stderr,"\t-w\tpassword (default none)\n");
    fprintf( stderr,"\t-3\tAFP 3.0 version\n");
    fprintf( stderr,"\t-4\tAFP 3.1 version (default)\n");
    fprintf( stderr,"\t-5\tAFP 3.2 version\n");
    fprintf( stderr,"\t-n\thow often to run (default: 1)\n");
    fprintf( stderr,"\t-v\tverbose\n");
    fprintf( stderr,"\t-V\tvery verbose\n");

    exit (1);
}

/* ------------------------------- */
int main(int ac, char **av)
{
    int cc;
    int Debug = 0;
    static char *vers = "AFP3.1";
    static char *uam = "Cleartxt Passwrd";

    while (( cc = getopt( ac, av, "tmvV345h:n:p:s:u:w:c:" )) != EOF ) {
        switch ( cc ) {
        case 't':
        	Throttle = 1;
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
		case 'm':
			Mac = 1;
			break;
        case 'n':
            Iterations = atoi(optarg);
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
        case 'w':
            Password = strdup(optarg);
            break;
        case 'p' :
            Port = atoi( optarg );
            if (Port <= 0) {
                fprintf(stderr, "Bad port.\n");
                exit(1);
            }
            break;
        case 'v':
            Debug = 1;
            break;
		case 'V':
			Verbose = 1;
			break;
        default :
            usage( av[ 0 ] );
        }
    }

    if (! Server)
        usage( av[ 0 ] );

    if (! Debug) {
        Verbose = 0;
        freopen("/dev/null", "w", stderr);
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

    /* login */	
    ExitCode = ntohs(FPopenLogin(Conn, vers, uam, User, Password));

	if (ExitCode == AFP_OK) {
        Conn->afp_version = Version;
        run_one();
        FPLogOut(Conn);
    }

    if (ExitCode) {
        if (! Debug)
            printf("Error, ExitCode: %u. Run with -v to see what went wrong.\n", ExitCode);
    } else
        displayresults();

	return ExitCode;
}
