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

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken:test220: AFP 3.0 get session token\n");
	if (Conn->afp_version != 30) {
		test_skipped(T_AFP3);
		return;
	}

	FAIL (FPGetSessionToken(Conn,0 ,0 ,0 ,NULL))
	dir  = FPCreateDir(Conn,vol, DIRDID_ROOT , name);
	if (!dir) {
		nottested();
		return;
	}

	if (Conn->afp_version < 30) {
		if (ntohl(AFPERR_NOOP) != FPEnumerate_ext(Conn, vol,  DIRDID_ROOT , "", 
		                    (1 << FILPBIT_PDINFO )|(1 << FILPBIT_EXTDFLEN) | (1 << FILPBIT_EXTRFLEN)
		                    |(1 << FILPBIT_DFLEN) |(1 << FILPBIT_RFLEN), 0xffff)) {
			failed();
		}
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
		return;
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
}

/* ------------------------- */
STATIC void test221()
{
char *name = "t23 dir";
char *name1 = "t23 subdir";
char *name2 = "t23 file";
int  dir,dir1;
u_int16_t vol = VolID;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken:test221: AFP 3.1 get session token\n");
	if (Conn->afp_version != 31) {
		test_skipped(T_AFP3);
		return;
	}

	FAIL (FPGetSessionToken(Conn, 3, 0, 5, "token"))
	FAIL (FPzzz(Conn))
	    
}

/* ----------- */
void FPGetSessionToken_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPGetSessionToken page 191\n");
    test220();
    test221();
}

