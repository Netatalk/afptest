/* ----------------------------------------------
*/
#include "specs.h"

static char temp[MAXPATHLEN];

/* ------------------------- */
STATIC void test162()
{
char ndir[4];

int dir;
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;
	
    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test162: illegal UTF8 name\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}

    ndir[0] = 'e';
    ndir[1] = 0xc3;
    ndir[2] = 0;
	if ((dir = FPCreateDir(Conn,vol, DIRDID_ROOT , ndir))) {
		failed();
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , ndir)) 
	}
	else if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {	
		failed();
	}
}

/* ------------------------- */
STATIC void test166()
{
char nfile[8];
u_int16_t bitmap;
u_int16_t vol = VolID;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test166: utf8 precompose decompose\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}

    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);
	strcpy(nfile, "ee.rtf");
	nfile[0] = 0xc3;         /* é.rtf precompose */
	nfile[1] = 0xa9;
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		nottested();
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME)|(1 << FILPBIT_PDINFO );

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, "\216.rtf")) {
		    fprintf(stderr,"\tFAILED %s should be \\216.rtf\n",filedir.lname);
		    failed_nomsg();
	    }
	}
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);

	strcpy(nfile, "eee.rtf");
	nfile[1] = 0xcc;         /* é.rtf decompose */
	nfile[2] = 0x81;
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , nfile))
}

/* ------------------------- */
STATIC void test167()
{
char nfile[8];
u_int16_t bitmap;
u_int16_t vol = VolID;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi;

	if (Conn->afp_version < 30) {
	    return;
	}
    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test167: utf8 precompose decompose\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}

    bitmap = (1<< FILPBIT_PDID) | (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_RFLEN);
	strcpy(nfile, "laa");
	nfile[1] = 0xc3;         /* là */
	nfile[2] = 0xa0;
	
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , nfile)) {
		nottested();
		return;
	}
    bitmap = (1<< FILPBIT_PDID)|(1<< FILPBIT_LNAME)|(1 << FILPBIT_PDINFO );

	if (FPGetFileDirParams(Conn, vol, DIRDID_ROOT, nfile, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
	    if (strcmp(filedir.lname, "l\210")) {
		    fprintf(stderr,"\tFAILED %s should be l\\210\n",filedir.lname);
		    failed_nomsg();
	    }
	}
	FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0);

	strcpy(nfile, "laaa");
	nfile[2] = 0xcc;         /* là */
	nfile[3] = 0x80;
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , nfile))
}

/* ------------------------- */
STATIC void test181()
{
char *name  = "t181 folder";
char *name1 = "t181 donne\314\201es"; /* decomposed données */
char *name2 = "t181 foo";
u_int16_t vol = VolID;
int  dir;
int  dir1;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test181: test search by ID UTF8\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}
	dir   = FPCreateDir(Conn, vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
	}

	dir1  = FPCreateDir(Conn,vol, dir , name1);
	if (!dir1) {
		failed();
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
		return;
	}

	FAIL (FPCreateFile(Conn, vol,  0, dir1 , name2))

	FAIL (FPCloseVol(Conn,vol))

	vol = VolID = FPOpenVol(Conn, Vol);
	if (FPEnumerate(Conn, vol,  dir1 , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		)
	) {
		failed();
		/* warm the cache */
		FPEnumerate(Conn, vol,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);
		FPEnumerate(Conn, vol,  dir , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) | (1<<FILPBIT_FINFO)|
	         (1<<FILPBIT_CDATE) | (1<<FILPBIT_BDATE) | (1<<FILPBIT_MDATE)
	         ,
		     (1<< DIRPBIT_ATTR) |  (1<<DIRPBIT_ATTR) | (1<<DIRPBIT_FINFO) |
	         (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE) |
		    (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		);
	}

	FAIL (FPDelete(Conn, vol,  dir1 , name2)) 
	FAIL (FPDelete(Conn, vol,  dir , name1))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}
/* ------------------------- 
 */
STATIC void test185()
{
char *name = "t185.txt";
char *name1 = "t185 donne\314"; /* decomposed données */
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test185: rename utf8 name\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	
	FAIL (ntohl(AFPERR_PARAM) != FPRename(Conn, vol, DIRDID_ROOT, name, name1))

	FAIL (ntohl(AFPERR_PARAM) != FPMoveAndRename(Conn, vol, DIRDID_ROOT, DIRDID_ROOT, name, name1))
	
	FAIL (FPDelete(Conn, vol, DIRDID_ROOT , name))

	FPFlush(Conn, vol);
}

/* ------------------------- */
STATIC void test233()
{
char *name = "t233 dir\314\201";
u_int16_t vol = VolID;
DSI *dsi;
int  dir;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test233: mangled UTF8 dirname\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}
	dir   = FPCreateDir(Conn, vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		return;
	}
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, 0,
	     (1<< DIRPBIT_DID) | (1<< DIRPBIT_PDID)  | (1<< DIRPBIT_PDINFO) )) {
		failed();
	}
	sprintf(temp,"t23#%X", ntohl(dir));

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, temp, 0,
	     (1<< DIRPBIT_DID) | (1<< DIRPBIT_PDID)  | (1<< DIRPBIT_PDINFO) )) {
		failed();
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* ------------------------- */
STATIC void test234()
{
char *name = "t234 file\314\201";
u_int16_t vol = VolID;
DSI *dsi;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap = 0;

	dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"Utf8:test233: mangled UTF8 filename\n");

	if ( !(get_vol_attrib(vol) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
	    return;
	}
	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		return;
	}
	bitmap = (1 <<  FILPBIT_PDINFO) | (1<< FILPBIT_PDID) | (1<< FILPBIT_FNUM);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, name, bitmap,0 )) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
		sprintf(temp,"t23#%X", ntohl(filedir.did));
		if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT, temp, bitmap,0 )) {
			failed();
		}
		else {
		    filedir.isdir = 0;
		    afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap,0);
		    if (strcmp(filedir.utf8_name, name)) {
				fprintf(stderr,"\tFAILED %s should be %s\n",filedir.utf8_name, name); 		    
		    }

		}
	}
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
}

/* ----------- */
void Utf8_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"UTF8 tests\n");
    test162();
    test166();
    test167();
    test181();
    test185();
	test233();
	test234();
}

