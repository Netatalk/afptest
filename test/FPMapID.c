/* ----------------------------------------------
*/
#include "specs.h"

STATIC void test208()
{
int  dir;
char *name = "t208 test Map ID";
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int16_t bitmap =  (1<<DIRPBIT_CDATE) | (1<<DIRPBIT_BDATE) | (1<<DIRPBIT_MDATE)
|(1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_FINFO)| (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID) ;
unsigned int ret;
u_int16_t vol = VolID;
DSI *dsi = &Conn->dsi;

    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMapID:test208: test Map ID call\n");

	if (!(dir = FPCreateDir(Conn,vol, DIRDID_ROOT , name))) {
		nottested();
		return;
	}
	FAIL (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, 0,bitmap )) 

	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);

	FAIL (FPMapID(Conn, 1, 0))  /* user to Mac roman */

	FAIL (FPMapID(Conn, 1, filedir.uid))  /* user to Mac roman */
	FAIL (htonl(AFPERR_NOITEM) != FPMapID(Conn, 1, -filedir.uid))  /* user to Mac roman */

	FAIL (htonl(AFPERR_NOITEM) != FPMapID(Conn, 2, -filedir.gid))  /* group to Mac roman */

	FAIL (FPMapID(Conn, 2, filedir.gid))  /* group to Mac roman */
	ret = FPMapID(Conn, 3, filedir.uid); /* user to UTF8 */
	if (Conn->afp_version >= 30 && ret) {
		failed();
	}
	else if (Conn->afp_version < 30 && ret != htonl(AFPERR_PARAM)) {
		failed();
	}
	ret = FPMapID(Conn, 4, filedir.gid); /* group to UTF8 */
	if (Conn->afp_version >= 30 && ret) {
		failed();
	}
	else if (Conn->afp_version < 30 && ret != htonl(AFPERR_PARAM)) {
		failed();
	}

	FAIL ((htonl(AFPERR_PARAM) != FPMapID(Conn, 5, filedir.gid))) 
	/* --------------------- */
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
}

/* ----------- */
void FPMapID_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPMapID page 220\n");
	test208();
}

