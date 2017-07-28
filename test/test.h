/* -------------------------------- */
extern unsigned int FPopenLogin(CONN *conn, char *vers, char *uam, char *usr, char *pwd);
extern unsigned int FPopenLoginExt(CONN *conn, char *vers, char *uam, char *usr, char *pwd);
extern unsigned int FPzzz(CONN *conn, int);
extern unsigned int FPLogOut(CONN *conn);
extern unsigned int FPMapID(CONN *conn, char fn, int id);
extern unsigned int FPMapName(CONN *conn, char fn, char *name );
extern unsigned int FPCreateDir(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPGetSessionToken(CONN *conn, int type, u_int32_t time, int len, char *token);
extern unsigned int FPDisconnectOldSession(CONN *conn, u_int16_t type, int len, char *token);

extern unsigned int FPGetSrvrInfo(CONN *conn);
extern unsigned int FPGetSrvrParms(CONN *conn);
extern unsigned int FPGetSrvrMsg(CONN *conn, u_int16_t type, u_int16_t bitmap);

extern u_int16_t    FPOpenVol(CONN *conn, char *vol);
extern u_int16_t FPOpenVolFull(CONN *conn, char *vol, u_int16_t  bitmap);
extern unsigned int FPCloseVol(CONN *conn, u_int16_t vol);
extern u_int16_t    FPOpenDT(CONN *conn, u_int16_t vol);
extern unsigned int FPCloseDT(CONN *conn, u_int16_t vol);
extern unsigned int FPCloseDir(CONN *conn, u_int16_t vol, int did);

extern unsigned int FPByteLock(CONN *conn, u_int16_t fork, int end, int mode, int offset, int size );
extern unsigned int FPByteLock_ext(CONN *conn, u_int16_t fork, int end, int mode, off_t offset, off_t size );

extern unsigned int FPCloseFork(CONN *conn, u_int16_t vol);

extern unsigned int FPFlush(CONN *conn, u_int16_t vol);
extern unsigned int FPFlushFork(CONN *conn, u_int16_t vol);
extern unsigned int FPEnumerate(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);
extern unsigned int FPEnumerateFull(CONN *conn, u_int16_t vol, u_int16_t sindex, u_int16_t reqcnt, u_int16_t size, 
    			int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);

extern unsigned int FPGetFileDirParams(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);
extern unsigned int FPEnumerate_ext(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);
extern unsigned int FPEnumerate_ext2(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap);
extern unsigned int FPEnumerateExt2Full(CONN *conn, uint16_t vol, uint32_t did, char *name, uint16_t f_bitmap, uint16_t d_bitmap, uint32_t startindex, uint16_t reqcount);
extern unsigned int FPDelete(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPOpenDir(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPCreateDir(CONN *conn, u_int16_t vol, int did , char *name);
extern u_int16_t    FPOpenFork(CONN *conn, u_int16_t vol, char type, u_int16_t bitmap, int did , char *name, int access);
extern unsigned int FPCreateFile(CONN *conn, u_int16_t vol, char type, int did , char *name);
extern unsigned int FPGetForkParam(CONN *conn, u_int16_t fork, u_int16_t bitmap);
extern unsigned int FPCopyFile(CONN *conn, u_int16_t svol, int sdid, u_int16_t dvol, int ddid, char *src, char *dstdir, char *dst);
extern unsigned int FPExchangeFile(CONN *conn, u_int16_t vol, int sdid, int ddid, char *src, char *dst);

extern unsigned int FPMoveAndRename(CONN *conn, u_int16_t svol, int sdid, int ddid, char *src, char *dst);
extern unsigned int FPRename(CONN *conn, u_int16_t svol, int sdid, char *src, char *dst);

extern unsigned int FPReadHeader(DSI *dsi, u_int16_t fork, int offset, int size, char *data);
extern unsigned int FPReadFooter(DSI *dsi, u_int16_t fork, int offset, int size, char *data);
extern unsigned int FPRead(CONN *conn, u_int16_t fork, int offset, int size, char *data);

extern unsigned int FPRead_ext (CONN *conn, u_int16_t fork, off_t offset, off_t size, char *data);
extern unsigned int FPRead_ext_async(CONN *conn, u_int16_t fork, off_t offset, off_t size, char *data);

extern unsigned int FPWriteHeader(DSI *dsi, u_int16_t fork, int offset, int size, char *data, char whence);
extern unsigned int FPWriteFooter(DSI *dsi, u_int16_t fork, int offset, int size, char *data, char whence);
extern unsigned int FPWrite(CONN *conn, u_int16_t fork, int offset, int size, char *data, char whence);
extern unsigned int FPWrite_ext(CONN *conn, u_int16_t fork, off_t  offset, off_t size, char *data, char whence);
extern unsigned int FPWrite_ext_async(CONN *conn, u_int16_t fork, off_t  offset, off_t size, char *data, char whence);

extern unsigned int FPSetForkParam(CONN *conn, u_int16_t fork,  u_int16_t bitmap, off_t size);

extern unsigned int FPGetComment(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPRemoveComment(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPAddComment(CONN *conn, u_int16_t vol, int did , char *name, char *cmt);

extern unsigned int FPGetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap);
extern unsigned int FPSetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap, struct afp_volume_parms *parms);

extern unsigned int FPGetUserInfo(CONN *conn, char flag, int id, u_int16_t bitmap);

extern unsigned int FPSetDirParms(CONN *conn, u_int16_t vol, int did, char *name, u_int16_t bitmap, 
                  	struct afp_filedir_parms *dir );
extern unsigned int FPSetFilDirParam(CONN *conn, u_int16_t vol, int did, char *name, u_int16_t bitmap,
                  	struct afp_filedir_parms *fil );

extern unsigned int FPSetFileParams(CONN *, u_int16_t vol, int did, char *name, u_int16_t bitmap, struct afp_filedir_parms *);

extern unsigned int FPCreateID(CONN *conn, u_int16_t vol, int did , char *name);
extern unsigned int FPDeleteID(CONN *conn, u_int16_t vol, int did );
extern unsigned int FPResolveID(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap );

extern unsigned int FPAddIcon(CONN *conn, u_int16_t dt, char *creator, char *type, char itype, u_int32_t tag, 
					u_int16_t size, char *data );
extern unsigned int FPGetIcon(CONN *conn, u_int16_t dt, char *creator, char *type, char itype, u_int16_t size );
extern unsigned int FPGetIconInfo(CONN *conn, u_int16_t dt, char *creator, u_int16_t itype );

extern unsigned int FPGetAppl(CONN *conn, u_int16_t dt, char *name, u_int16_t index, u_int16_t f_bitmap);
extern unsigned int FPAddAPPL(CONN *conn, u_int16_t dt, int did, char *creator, u_int32_t tag, char *name);
extern unsigned int FPRemoveAPPL(CONN *conn, u_int16_t dt, int did, char *creator, char *name);

extern unsigned int FPCatSearch(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap, u_int16_t d_bitmap,
                                u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2);

extern unsigned int FPCatSearchExt(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap, u_int16_t d_bitmap,
                                u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2);

extern unsigned int FPBadPacket(CONN *conn, char fn, char *name );

extern unsigned int FPGetACL(CONN *conn, u_int16_t svol, int did, u_int16_t bitmap, char *name);
extern unsigned int FPGetExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, u_int16_t maxsize, char *name, char *attr);
extern unsigned int FPListExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, int maxsize, char* name);
extern unsigned int FPSetExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, char* name, char* attr, char* data);
extern unsigned int FPRemoveExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, char* name, char* attr);
extern unsigned int FPSyncDir(CONN *conn, u_int16_t vol, int did);

void dump_header(DSI *dsi);
char *afp_error(int error);
const char *AfpNum2name(int num);

#define FILPBIT_EXTDFLEN 11
#define FILPBIT_PDINFO   13    /* ProDOS Info/ UTF8 name */
#define FILPBIT_EXTRFLEN 14

#define ATTRBIT_ROPEN     (1<<4)  /* resource fork already open */  
#define ATTRBIT_DOPEN     (1<<3)  /* data fork already open */  
#define ATTRBIT_NOWRITE   (1<<5)  /* write inhibit(v2)/read-only(v1) bit */ 
#define ATTRBIT_NORENAME  (1<<7)  /* rename inhibit (d) */
#define ATTRBIT_NODELETE  (1<<8)  /* delete inhibit (d) */
#define ATTRBIT_SETCLR    (1<<15) /* set/clear bits (d) */     

extern CONN *Conn, *Conn2;

// extern DSI *dsi; 
// extern u_int16_t vol;
extern char Data[];

extern char *Vol;
extern char *Vol2;

extern char *Path;
extern char *User;
extern int Version;
extern int Quirk;
extern int Verbose;
extern int Quiet;
extern int Locking;
extern int Loglevel;
extern int Color;
