/*
 * $Id: encoding_test.c,v 1.1 2003-09-12 18:30:50 didg Exp $
 * MANIFEST
 */

#include "afpclient.h"
#include "test.h"
#include "specs.h"
#include <time.h>

int Verbose = 0;
int Quirk = 0;
char    *Dir = "";

u_int16_t VolID;
static DSI *dsi;
CONN *Conn;
CONN *Conn2;

int ExitCode = 0;
int Recurse = 0;
char *Encoding = "western";
extern int Convert;

/* ------------------------- */
int empty_volume()
{
u_int16_t vol = VolID;
u_int16_t d_bitmap;
u_int16_t f_bitmap;
unsigned int ret;
int  dir;
u_int16_t  tp;
int  i, j;
char *b;
struct afp_filedir_parms filedir;
int *stack;
int cnt = 0;
int size = 1000;
DSI *dsi;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Delete all files\n");
    f_bitmap = (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE);

	d_bitmap = (1<< DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |  (1 << DIRPBIT_OFFCNT) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS);
		   
	if (Conn->afp_version >= 30) {
		f_bitmap |= (1<<FILPBIT_PDINFO);
		d_bitmap |= (1<<FILPBIT_PDINFO);
	}
	else {
		f_bitmap |= (1<<FILPBIT_LNAME);
		d_bitmap |= (1<<FILPBIT_LNAME);
	}
	dir = get_did(Conn, vol, DIRDID_ROOT, Dir);
	if (!dir) {
		nottested();
	    return -1;
	}
	if (!(stack = calloc(size, sizeof(int)) )) {
		nottested();
	    return -1;
	}
	stack[cnt] = dir;
	cnt++;
	
	while (cnt) {
	    cnt--;
	    dir = stack[cnt];
		i = 1;
		if (FPGetFileDirParams(Conn, vol,  dir , "", 0, d_bitmap)) {
			nottested();
			return -1;
		}
		while (!(ret = FPEnumerateFull(Conn, vol, i, 150, 8000,  dir , "", f_bitmap, d_bitmap))) {
			memcpy(&tp, dsi->data +4, sizeof(tp));
			tp = ntohs(tp);
		    i += tp;
		    b = dsi->data +6;
			for (j = 1; j <= tp; j++, b += b[0]) {
			    if (b[1]) {
	    		   filedir.isdir = 1;
	    		}
	    		else {
	    		   filedir.isdir = 0;
	    		}
	    		afp_filedir_unpack(&filedir, b + 2, f_bitmap, d_bitmap);
	    		if (FPDelete(Conn, vol,  DIRDID_ROOT , (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname)) {
	    		    nottested();
	    		    return -1;
	    		}
	    	}
	    }
	    if (ret != ntohl(AFPERR_NOOBJ)) {
			nottested();
			return -1;
	    }
	}
	return 0;
}

/* ------------------ 
 * client encoding western 1 byte [1..255]
*/
static void test_western()
{
u_int16_t vol = VolID;
u_int16_t f_bitmap;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
char name[30];
char *result;
int  i, j = 0;
DSI *dsi;

	dsi = &Conn->dsi;

    for (i = 1; i < 8; i++) {
    	name[i -1] = i;
    }
    name[i -1] = 0;
	if (Conn->afp_version >= 30) {
		f_bitmap = (1<<FILPBIT_PDINFO);
	}
	else {
		f_bitmap = (1<<FILPBIT_LNAME);
	}
    while (j < 32) {
        j++;
		if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
			nottested();
			return;
		}
		if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, name, f_bitmap, 0)) {
			nottested();
			return;
		}
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, f_bitmap, 0);
		result = (Conn->afp_version >= 30)?filedir.utf8_name:filedir.lname;
		if (strcmp(result, name)) {
			failed();
			return;
		}
		
    	for (i = 0; i < 8; i++) {
    		name[i] = j*8 +i;
	    }
    	name[i] = 0;
	}
}

/* ------------------ */
static void run_one()
{
	Convert = 0;
	dsi = &Conn->dsi;
	VolID = FPOpenVol(Conn, Vol);
	if (VolID == 0xffff) {
		nottested();
		return;
	}
	if (empty_volume() < 0) {
	    return;
	}
	if (!strcasecmp(Encoding, "western")) {
	    test_western();
	}
	else {
	}
}

/* =============================== */

DSI *Dsi;

char Data[300000] = "";
/* ------------------------------- */
char    *Server = "localhost";
int     Proto = 0;
int     Port = 548;
char    *Password = "";
char    *Vol = "";
char    *User;
int     Version = 21;
int     Mac = 0;

/* =============================== */
void usage( char * av0 )
{
    fprintf( stderr, "usage:\t%s [-m] [-e encoding] [-h host] [-p port] [-s vol] [-u user] [-w password]\n", av0 );
    fprintf( stderr,"\t-m\tserver is a Mac\n");
    fprintf( stderr,"\t-e\tclient encoding western|utf8|hebrew|\n");
    fprintf( stderr,"\t-h\tserver host name (default localhost)\n");
    fprintf( stderr,"\t-p\tserver port (default 548)\n");
    fprintf( stderr,"\t-s\tvolume to mount (default home)\n");
    fprintf( stderr,"\t-u\tuser name (default uid)\n");
    fprintf( stderr,"\t-w\tpassword (default none)\n");
    fprintf( stderr,"\t-3\tAFP 3.0 version\n");
    fprintf( stderr,"\t-4\tAFP 3.1 version\n");
    fprintf( stderr,"\t-v\tverbose\n");

    exit (1);
}

/* ------------------------------- */
int main( ac, av )
int		ac;
char	**av;
{
int cc;
//	static char *vers = "AFP2.2";
static char *vers = "AFPVersion 2.1";
static char *uam = "Cleartxt Passwrd";

    while (( cc = getopt( ac, av, "Rmlv34h:p:s:u:w:d:" )) != EOF ) {
        switch ( cc ) {
        case '3':
			vers = "AFPX03";
			Version = 30;
			break;
        case '4':
			vers = "AFP3.1";
			Version = 31;
			break;
		case 'R':
			Recurse = 1;
			break;
		case 'm':
			Mac = 1;
			break;
		case 'e':
			Encoding = strdup(optarg);
			break;
		
        case 'd':
            Dir = strdup(optarg);
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
	    	return 2;
        }
     	Dsi->protocol = DSI_TCPIP; 
	    Dsi->socket = sock;
    }
    else {
	}

    /* login */	
	FPopenLogin(Conn, vers, uam, User, Password);
	Conn->afp_version = Version;

	
	run_one();

   	FPLogOut(Conn);
	return ExitCode;
}
