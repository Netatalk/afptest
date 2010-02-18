#include "specs.h"

void FPCopyFile_arg(char **argv)
{
    u_int16_t vol = VolID;

    fprintf(stderr,"======================\n");
    fprintf(stderr,"FPCopyFile with args:\n");

    fprintf(stderr,"source: \"%s\" -> dest: \"%s\"\n", argv[0], argv[1]);    

	FAIL (FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, argv[0], "", argv[1]))

}

/* ----------- */
void FPResolveID_arg(char **argv)
{
    int argc = 0;
    unsigned int ret, ofs = 3 * sizeof( u_int16_t );
    u_int16_t bitmap = ( 1 << FILPBIT_PDINFO );
    uint32_t id;
    DSI *dsi;
    struct afp_filedir_parms filedir;

    dsi = &Conn->dsi;

    fprintf(stderr,"======================\n");
    fprintf(stderr,"FPResolveID with args:\n");

    id = atoi(argv[0]);
    fprintf(stderr,"Trying to resolve id %u\n", id);

	if (Conn->afp_version < 30) {
		test_skipped(T_AFP3);
        return;
	}

	if ( !(get_vol_attrib(VolID) & VOLPBIT_ATTR_UTF8)) {
		test_skipped(T_UTF8);
		return;
	}


	if (!(get_vol_attrib(VolID) & VOLPBIT_ATTR_FILEID) ) {
		test_skipped(T_ID);
		return;
	}

	if ( FPResolveID(Conn, VolID, htonl(id), bitmap) ) {
        failed();
        return;
    }
	filedir.isdir = 0;
    afp_filedir_unpack(&filedir, dsi->data + 2, bitmap, 0);

    fprintf(stderr,"Resolved ID %d to: '%s'\n", id, filedir.utf8_name);
}
