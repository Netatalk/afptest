/*
 * $Id: speedtest.c,v 1.1 2004-04-24 18:25:56 didg Exp $
 * MANIFEST
 */
#include "specs.h"
#include <dlfcn.h>
#include <sys/time.h>
#include <unistd.h>
          
int Verbose = 0;
int Interactive = 0;
int Quirk = 0;

u_int16_t VolID;
u_int16_t VolID2;
static DSI *dsi;
CONN *Conn;

int ExitCode = 0;

DSI *Dsi;
char Data[30000];
char *Buffer;
struct timeval Timer_start;
struct timeval Timer_end;
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)

/* ------------------------------- */
static char    *Server = "localhost";
static int     Proto = 0;
static int     Port = 548;
static char    *Password = "";
char    *Vol = "";
static char    *Vol2 = "";
static char    *User;
static int     Version = 21;
static char    *Test = "Write";

static int Count = 1;
static off_t Size = 64* MEGABYTE;
static size_t Quantum = 0;
static int Request = 1;
static int Delete = 0;
static int Sparse = 0;

/* not used */
CONN *Conn2;
int  Mac = 0;

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
int is_there(CONN *conn, u_int16_t vol, int did, char *name)
{
	return FPGetFileDirParams(conn, vol,  did, name, 
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
	         ,
	         (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) 
		);
}

/* ------------------ */
unsigned long long delta (void)
{
unsigned long long s, e;

	s  = Timer_start.tv_sec;
    s *= 1000000;
    s += Timer_start.tv_usec;
    
	e  = Timer_end.tv_sec;
    e *= 1000000;
    e += Timer_end.tv_usec;
    
 return e -s;
}

/* ------------------ */
static void header(void)
{
	fprintf(stderr, "run\t microsec\t  MB/s\n");
}

/* ------------------ */
static void timer_footer(void)
{
unsigned long long d;

	gettimeofday(&Timer_end, NULL);
	d = delta();
	fprintf(stderr, "%9lld\t%.2f\n", d, ((float)Size*MEGABYTE/(float)d)/1000);
}

/* ------------------ */
void Write(void)
{
int dir = 0;
int fork = 0;
int id = getpid();
static char temp[MAXPATHLEN];   
int vol = VolID;
off_t  offset;
off_t  offset_r;
off_t  written;
off_t  written_r;
size_t nbe;
size_t nbe_r;
int i;
int push;
DSI *dsi;

	fprintf(stderr, "Write quantum %d, size %lld\n", Quantum, Size);
	header();

	sprintf(temp,"WriteTest-%d", id);

	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}
	if (FPCreateFile(Conn, vol,  0, dir , "File")){
		failed();
		goto fin;
	}

	dsi = &Conn->dsi;
	for (i = 1; i <= Count; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , (1<<FILPBIT_FNUM), dir, "File", OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork) {
			failed();
			goto fin1;
		}
		fprintf(stderr,"%d\t", i);
		gettimeofday(&Timer_start, NULL);
		nbe = nbe_r = Quantum;
		written = written_r = Size;
		offset = offset_r = 0;
		push = 0;
		while (written ) {
		    if (written < Quantum) {
	    	    nbe = written;
		    }
		    if (push < Request && FPWriteHeader(dsi, fork, offset, nbe, Buffer, 0)) {
				failed();
				goto fin1;
			}
			written -= nbe;
			offset += nbe;
			push++;
		    if (push >= Request) {
		    	if (written_r < Quantum) {
	    	    	nbe_r = written_r;
		    	}
				if (FPWriteFooter(dsi, fork, offset_r, nbe_r, Buffer, 0)) {
		    		failed();
		    		goto fin1;
				}
				push--;
				written_r -= nbe_r;
				offset_r += nbe_r;
		    }
		}
		while (push) {
		    if (written_r < Quantum) {
	    		nbe_r = written_r;
		    }
			if (FPWriteFooter(dsi, fork, offset_r, nbe_r, Buffer, 0)) {
		    	failed();
		    	goto fin1;
			}
			push--;
			written_r -= nbe_r;
			offset_r += nbe_r;
		}
		
		if (FPFlushFork(Conn, fork)) {
			failed();
			goto fin1;
		}
		timer_footer();
		if (FPCloseFork(Conn,fork)) {
			failed();
			goto fin;
		}
		fork = 0;

		if (Delete) {
			if (FPDelete(Conn, vol,  dir, "File") || FPCreateFile(Conn, vol,  0, dir , "File")){
				failed();
				goto fin;
			}
		}
	}

fin1:
	if (fork && FPCloseFork(Conn,fork)) {failed();}
	if (FPDelete(Conn, vol,  dir, "File")) {failed();}
fin:
	if (FPDelete(Conn, vol,  dir, "")) {failed();}
    fprintf(stderr, "\n");
	return;
}

/* ------------------------ */
int init_fork(int fork)
{
off_t  written;
off_t  offset;
size_t nbe;

	if (Sparse) {
		/* assume server will create a sparse file */
		if (FPSetForkParam(Conn, fork, (1<<FILPBIT_DFLEN), Size))
			return -1;
		return 0;
	}
	nbe = Quantum;
	written = Size;
	offset = 0;
	while (written ) {
		if (written < Quantum) {
	    	nbe = written;
		}
		if (FPWrite(Conn, fork, offset, nbe, Buffer, 0 )) {
			return -1;
		}
		written -= nbe;
		offset += nbe;
	}
	if (FPFlushFork(Conn, fork)) {
		return -1;
	}
	return 0;
}

/* ------------------ */
void Copy(void)
{
int dir = 0;
int dir2 = 0;
int fork = 0;
int fork2 = 0;
int id = getpid();
static char temp[MAXPATHLEN];   
int vol = VolID;
int vol2 = VolID2;
off_t  written;
off_t  offset = 0;
size_t nbe;
int i;

	fprintf(stderr, "Copy qantum %d, size %lld %s\n", Quantum, Size, Sparse?"sparse file":"");
	header();

	sprintf(temp,"CopyTest-%d", id);
	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	if (VolID != VolID2 && ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID2, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}
	if (VolID == VolID2) {
		dir2 = dir;
	}
	else if (!(dir2 = FPCreateDir(Conn, vol2, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  0, dir , "Source")){
		failed();
		goto fin;
	}
	if (FPCreateFile(Conn, vol2,  0, dir2 , "Destination")){
		failed();
		goto fin;
	}
	
	for (i = 1; i <= Count; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , (1<<FILPBIT_FNUM), dir, "Source", OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork) {
			failed();
			goto fin1;
		}
		fork2 = FPOpenFork(Conn, vol2, OPENFORK_DATA , (1<<FILPBIT_FNUM), dir2, "Destination", OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork2) {
			failed();
			goto fin1;
		}
		if (i == 1 || Delete) {
			if (init_fork(fork)) {
				failed();
				goto fin1;
			}
		}
		fprintf(stderr,"%d\t", i);
		gettimeofday(&Timer_start, NULL);
		nbe = Quantum;
		written = Size;
		offset = 0;
		while (written ) {
		    if (written < Quantum) {
	    	    nbe = written;
		    }
		    if (FPRead(Conn, fork, offset, nbe, Buffer)) {
		    	failed();
		    	goto fin1;
		    }
			if (FPWrite(Conn, fork2, offset, nbe, Buffer, 0 )) {
				failed();
				goto fin1;
			}
			written -= nbe;
			offset += nbe;
		}
		timer_footer();
		if (FPCloseFork(Conn,fork)) {
			failed();
			goto fin1;
		}
		fork = 0;
		if (FPCloseFork(Conn,fork2)) {
			failed();
			goto fin1;
		}
		fork2 = 0;

		if (Delete) {
			if (FPDelete(Conn, vol,  dir, "Source")) {
				goto fin;
			}
			if (FPDelete(Conn, vol2,  dir2, "Destination")) {
				goto fin;
			}
			
			if (FPCreateFile(Conn, vol,  0, dir , "Source")){
				failed();
				goto fin;
			}
			if (FPCreateFile(Conn, vol2,  0, dir2 , "Destination")){
				failed();
				goto fin;
			}
		}
	}

fin1:
	if (fork && FPCloseFork(Conn,fork)) {failed();}
	if (fork2 && FPCloseFork(Conn,fork2)) {failed();}
	if (FPDelete(Conn, vol,  dir, "Source")) {failed();}
	if (FPDelete(Conn, vol2,  dir2, "Destination")) {failed();}
fin:
	if (dir && FPDelete(Conn, vol,  dir, "")) {failed();}
	if (dir2 && dir2 != dir && FPDelete(Conn, vol2,  dir2, "")) {failed();}
    
	return;
}

/* ------------------ */
void ServerCopy(void)
{
int dir = 0;
int dir2 = 0;
int fork = 0;
int id = getpid();
static char temp[MAXPATHLEN];   
int vol = VolID;
int vol2 = VolID2;
int i;

	fprintf(stderr, "ServerCopy qantum %d, size %lld %s\n", Quantum, Size, Sparse?"sparse file":"");
	header();

	sprintf(temp,"ServerCopyTest-%d", id);
	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	if (VolID != VolID2 && ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID2, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}
	if (VolID == VolID2) {
		dir2 = dir;
	}
	else if (!(dir2 = FPCreateDir(Conn, vol2, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  0, dir , "Source")){
		failed();
		goto fin;
	}
	for (i = 1; i <= Count; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , (1<<FILPBIT_FNUM), dir, "Source", OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork) {
			failed();
			goto fin1;
		}
		if (i == 1 || Delete) {
			if (init_fork(fork)) {
				failed();
				goto fin1;
			}
			if (FPCloseFork(Conn,fork)) {
				failed();
				goto fin1;
			}
			fork = 0;
		}
		fprintf(stderr,"%d\t", i);
		gettimeofday(&Timer_start, NULL);
		if (FPCopyFile(Conn, vol, dir, vol2, dir2, "Source", "Destination")) {
			failed();
			goto fin1;
		}
		timer_footer();
		if (Delete) {
			if (FPDelete(Conn, vol,  dir, "Source")) {
				failed();
				goto fin;
			}
			if (FPCreateFile(Conn, vol,  0, dir , "Source")){
				failed();
				goto fin;
			}
		}
		if (FPDelete(Conn, vol2,  dir2, "Destination")) {
			failed();
			goto fin;
		}
	}

fin1:
	if (fork && FPCloseFork(Conn,fork)) {failed();}
	if (FPDelete(Conn, vol,  dir, "Source")) {failed();}
	FPDelete(Conn, vol2,  dir2, "Destination");
fin:
	if (dir && FPDelete(Conn, vol,  dir, "")) {failed();}
	if (dir2 && dir2 != dir && FPDelete(Conn, vol2,  dir2, "")) {failed();}
    
	return;
}

/* ------------------ */
void Read(void)
{
int dir = 0;
int fork = 0;
int id = getpid();
static char temp[MAXPATHLEN];   
int vol = VolID;
off_t  written;
off_t  written_r;
off_t  offset = 0;
off_t  offset_r = 0;
size_t nbe;
size_t nbe_r;
DSI *dsi;
int i;
int push;

	fprintf(stderr, "Read qantum %d, size %lld %s\n", Quantum, Size, Sparse?"sparse file":"");
	header();

	sprintf(temp,"ReadTest-%d", id);
	if (ntohl(AFPERR_NOOBJ) != is_there(Conn, VolID, DIRDID_ROOT, temp)) {
		nottested();
		return;
	}
	
	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , temp))) {
		nottested();
		goto fin;
	}

	if (FPCreateFile(Conn, vol,  0, dir , "File")){
		failed();
		goto fin;
	}
	dsi = &Conn->dsi;
	for (i = 1; i <= Count; i++) {
		fork = FPOpenFork(Conn, vol, OPENFORK_DATA , (1<<FILPBIT_FNUM), dir, "File", OPENACC_WR |OPENACC_RD| OPENACC_DWR| OPENACC_DRD);
		if (!fork) {
			failed();
			goto fin1;
		}
		if (i == 1 || Delete) {
			if (init_fork(fork)) {
				failed();
				goto fin1;
			}
		}
		nbe = nbe_r = Quantum;
		written = Size;
		written_r = Size;
		offset = 0;
		offset_r = 0;
		push = 0;
		fprintf(stderr,"%d\t", i);
		gettimeofday(&Timer_start, NULL);
		while (written ) {
		    if (written < Quantum) {
	    	    nbe = written;
		    }
		    if (push < Request && FPReadHeader(dsi, fork, offset, nbe, Buffer)) {
		    	failed();
		    	goto fin1;
		    }
			written -= nbe;
			offset += nbe;
		    push++;
		    if (push >= Request) {
		    	if (written_r < Quantum) {
	    	    	nbe_r = written_r;
		    	}
				if (FPReadFooter(dsi, fork, offset_r, nbe_r, Buffer)) {
		    		failed();
		    		goto fin1;
				}
				push--;
				written_r -= nbe_r;
				offset_r += nbe_r;
		    }
		}
		while (push) {
		    if (written_r < Quantum) {
	    		nbe_r = written_r;
		    }
			if (FPReadFooter(dsi, fork, offset_r, nbe_r, Buffer)) {
		    	failed();
		    	goto fin1;
			}
			push--;
			written_r -= nbe_r;
			offset_r += nbe_r;
		}
		timer_footer();
		
		if (FPCloseFork(Conn,fork)) {failed();}
		fork = 0;

		if (Delete) {
			if (FPDelete(Conn, vol,  dir, "File")) {
				goto fin;
			}
			
			if (FPCreateFile(Conn, vol,  0, dir , "File")){
				failed();
				goto fin;
			}
		}
	}

fin1:
	if (fork && FPCloseFork(Conn,fork)) {failed();}
	if (FPDelete(Conn, vol,  dir, "File")) {failed();}
fin:
	if (FPDelete(Conn, vol,  dir, "")) {failed();}
    
	return;
}

/* ----------- */
static void run_one(char *name)
{
void *handle = NULL;
void (*fn)(void) = NULL;
char *error;
char *token;
char *tp = strdup(name);
    token = strtok(tp, ",");
    
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

	dsi = &Conn->dsi;
	press_enter("Opening volume.");
	VolID = FPOpenVol(Conn, Vol);
	if (VolID == 0xffff) {
		nottested();
		return;
	}
	if (*Vol2) {
		VolID2 = FPOpenVol(Conn, Vol2);
		if (VolID2 == 0xffff) {
			nottested();
			return;
		}
	}
	else {
	    VolID2 = VolID;
	}
	/* check server quantum size */
	if (!Quantum) {
	    Quantum = dsi->server_quantum;
	}
	else if (Quantum > dsi->server_quantum) {
		fprintf(stderr,"\t server quantum (%d) too small\n", dsi->server_quantum);
		return;
	}
	Buffer = malloc(Quantum);
	if (!Buffer) {
		fprintf(stderr,"\t can't allocate (%d) bytes\n", Quantum);
		return;
	}

	/* loop */
	while (token ) {
		press_enter(token);
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
	if (*Vol2) {
		FPCloseVol(Conn,VolID2);
	}
}
                                                                            
/* =============================== */
void usage( char * av0 )
{
    fprintf( stderr, "usage:\t%s [-h host] [-p port] [-s vol] [-S Vol2] [-u user] [-w password] [-f test] [-c count] - \n", av0 );
    fprintf( stderr,"\t-h\tserver host name (default localhost)\n");
    fprintf( stderr,"\t-p\tserver port (default 548)\n");
    fprintf( stderr,"\t-s\tvolume to mount (default home)\n");
    fprintf( stderr,"\t-S\tsecond volume to mount (default none)\n");
    fprintf( stderr,"\t-u\tuser name (default uid)\n");
    
    fprintf( stderr,"\t-w\tpassword (default none)\n");
    fprintf( stderr,"\t-2\tAFP 2.2 version (default 2.1)\n");
    fprintf( stderr,"\t-3\tAFP 3.0 version\n");
    fprintf( stderr,"\t-4\tAFP 3.1 version\n");

    fprintf( stderr,"\t-c\trun test count times\n");
    fprintf( stderr,"\t-d\tfile size (Mbytes, default 64)\n");
    fprintf( stderr,"\t-q\tpacket size (Kbytes, default server quantum)\n");
    fprintf( stderr,"\t-r\tnumber of outstanding requests (default 1)\n");
    fprintf( stderr,"\t-y\tuse a new file for each run (default same file)\n");
    fprintf( stderr,"\t-e\tsparse file (default no)\n");
    
    fprintf( stderr,"\t-v\tverbose (default no)\n");
    fprintf( stderr,"\t-V\tvery verbose (default no)\n");

    fprintf( stderr,"\t-f\ttest to run (Read, Write, Copy, ServerCopy, default Write)  \n");
    fprintf( stderr,"\t-i\tinteractive mode, prompts before every test (debug purposes)\n");
    exit (1);
}

char *vers = "AFPVersion 2.1";
char *uam = "Cleartxt Passwrd";
/* ------------------------------- */
int main( ac, av )
int		ac;
char	**av;
{
int cc;

	Quiet = 1;
    while (( cc = getopt( ac, av, "Vv234h:p:s:S:u:d:w:f:i""c:q:r:ye" )) != EOF ) {
        switch ( cc ) {
		case 'c':
			Count = atoi(optarg);
			break;
		case 'd':
			Size = atoi(optarg) * MEGABYTE;
			break;
		case 'q':
			Quantum = atoi(optarg) *KILOBYTE;
			break;
		case 'r':
			Request = atoi(optarg);
			break;
		case 'y':
			Delete = 1;
			break;
		case 'e':
			Sparse = 1;
			break;
			
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
        case 'w':
            Password = strdup(optarg);
            break;
        case 'f' :
            Test = strdup(optarg);
            break;
        case 'p' :
            Port = atoi( optarg );
            if (Port <= 0) {
                fprintf(stderr, "Bad port.\n");
                exit(1);
            }
            break;
	case 'v':
		Quiet = 0;
		break;
	case 'V':
		Verbose = 1;
		break;
	case 'i':
		Interactive = 1;
		break;
			
        default :
            usage( av[ 0 ] );
        }
    }
    
	/**************************
	 Connection */

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
	
	/*********************************
	*/
	if (Verbose) 
		Quiet = 0;
	run_one(Test);

   	FPLogOut(Conn);

	return ExitCode;
}
