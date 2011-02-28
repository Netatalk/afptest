/* ---------------------------------------------------
*/
#ifndef AFPCLIENT_H
#define AFPCLIENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <signal.h>

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <sys/ioctl.h>
#ifdef TRU64
#include <sys/mbuf.h>
#include <net/route.h>
#endif /* TRU64 */
#include <net/if.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h> 
#if 0
#include <netatalk/endian.h>
#include <netatalk/at.h>
#include <atalk/compat.h>
#include <atalk/dsi.h>
#include <atalk/atp.h>
#include <atalk/asp.h>
#include <atalk/afp.h>
#endif

#ifndef SA_ONESHOT
#define SA_ONESHOT SA_RESETHAND
#endif


#ifndef u_int16_t
#define u_int16_t uint16_t
#endif
#ifndef u_int32_t
#define u_int32_t uint32_t
#endif
#ifndef u_int64_t
#define u_int64_t uint64_t
#endif
#ifndef u_int8_t
#define u_int8_t uint8_t
#endif

#ifndef HAVE_BSWAP_64
#define bswap_64(x) \
    ((((x) & 0xff00000000000000ull) >> 56)                                   \
     | (((x) & 0x00ff000000000000ull) >> 40)                                 \
     | (((x) & 0x0000ff0000000000ull) >> 24)                                 \
     | (((x) & 0x000000ff00000000ull) >> 8)                                  \
     | (((x) & 0x00000000ff000000ull) << 8)                                  \
     | (((x) & 0x0000000000ff0000ull) << 24)                                 \
     | (((x) & 0x000000000000ff00ull) << 40)                                 \
     | (((x) & 0x00000000000000ffull) << 56))
#endif	/* bswap_64 */

#include "dsi.h"
#include "afp.h"
/* AFP functions */
#ifndef AFP_CLOSEVOL
#define AFP_CLOSEVOL     2
#define AFP_ENUMERATE    9
 
#define AFP_GETSRVINFO  15
#define AFP_GETSRVPARAM 16
#define AFP_LOGIN       18
 
#define AFP_LOGOUT      20
#define AFP_OPENVOL     24
#define AFP_OPENDIR     25
#define AFP_OPENFORK    26
 
#define AFP_OPENDT      48
#define AFP_CLOSEDT     49
#endif

/* ----------------------------- */
/* from etc/afpd/directory.h volume attributes */
#define DIRPBIT_ATTR    0
#define DIRPBIT_PDID    1
#define DIRPBIT_CDATE   2
#define DIRPBIT_MDATE   3
#define DIRPBIT_BDATE   4
#define DIRPBIT_FINFO   5
#define DIRPBIT_LNAME   6
#define DIRPBIT_SNAME   7
#define DIRPBIT_DID     8
#define DIRPBIT_OFFCNT  9
#define DIRPBIT_UID     10
#define DIRPBIT_GID     11
#define DIRPBIT_ACCESS  12
#define DIRPBIT_PDINFO  13         /* ProDOS Info /UTF8 name */
#define DIRPBIT_UNIXPR  15 
 
/* directory attribute bits (see file.h for other bits) */
#define ATTRBIT_EXPFOLDER   (1 << 1) /* shared point */
#define ATTRBIT_MOUNTED     (1 << 3) /* mounted share point by non-admin */
#define ATTRBIT_INEXPFOLDER (1 << 4) /* folder in a shared area */
 
#define FILDIRBIT_ISDIR        (1 << 7) /* is a directory */
#define FILDIRBIT_ISFILE       (0)      /* is a file */
 
/* reserved directory id's */
#define DIRDID_ROOT_PARENT    htonl(1)  /* parent directory of root */
#define DIRDID_ROOT           htonl(2)  /* root directory */

/* ----------------------------- */
/* from etc/afpd/file.h volume attributes */
#define FILPBIT_ATTR     0
#define FILPBIT_PDID     1
#define FILPBIT_CDATE    2
#define FILPBIT_MDATE    3
#define FILPBIT_BDATE    4
#define FILPBIT_FINFO    5
#define FILPBIT_LNAME    6
#define FILPBIT_SNAME    7
#define FILPBIT_FNUM     8
#define FILPBIT_DFLEN    9
#define FILPBIT_RFLEN    10
#define FILPBIT_EXTDFLEN 11
#define FILPBIT_PDINFO   13    /* ProDOS Info/ UTF8 name */
#define FILPBIT_EXTRFLEN 14
#define FILPBIT_UNIXPR   15   

/* attribute bits. (d) = directory attribute bit as well. */
#define ATTRBIT_INVISIBLE (1<<0)  /* invisible (d) */
#define ATTRBIT_MULTIUSER (1<<1)  /* multiuser */
#define ATTRBIT_SYSTEM    (1<<2)  /* system (d) */
#define ATTRBIT_DOPEN     (1<<3)  /* data fork already open */
#define ATTRBIT_ROPEN     (1<<4)  /* resource fork already open */
#define ATTRBIT_SHARED    (1<<4)  /* shared area (d) */
#define ATTRBIT_NOWRITE   (1<<5)  /* write inhibit(v2)/read-only(v1) bit */
#define ATTRBIT_BACKUP    (1<<6)  /* backup needed (d) */
#define ATTRBIT_NORENAME  (1<<7)  /* rename inhibit (d) */
#define ATTRBIT_NODELETE  (1<<8)  /* delete inhibit (d) */
#define ATTRBIT_NOCOPY    (1<<10) /* copy protect */
#define ATTRBIT_SETCLR    (1<<15) /* set/clear bits (d) */

/* ----------------------------- */
/* from etc/afpd/volume.h volume attributes */
#define VOLPBIT_ATTR_RO           (1 << 0)
#define VOLPBIT_ATTR_PASSWD       (1 << 1)
#define VOLPBIT_ATTR_FILEID       (1 << 2)
#define VOLPBIT_ATTR_CATSEARCH    (1 << 3)
#define VOLPBIT_ATTR_BLANKACCESS  (1 << 4)
#define VOLPBIT_ATTR_UNIXPRIV     (1 << 5)
#define VOLPBIT_ATTR_UTF8         (1 << 6)
#define VOLPBIT_ATTR_NONETUID     (1 << 7)
#define VOLPBIT_ATTR_PRIVPARENT   (1 << 8)
#define VOLPBIT_ATTR_NOEXCHANGE   (1 << 9)
#define VOLPBIT_ATTR_EXTATTRS     (1 << 10)
#define VOLPBIT_ATTR_ACLS         (1 << 11)
 
#define VOLPBIT_ATTR    0
#define VOLPBIT_SIG     1
#define VOLPBIT_CDATE   2
#define VOLPBIT_MDATE   3
#define VOLPBIT_BDATE   4
#define VOLPBIT_VID     5
#define VOLPBIT_BFREE   6
#define VOLPBIT_BTOTAL  7
#define VOLPBIT_NAME    8
/* handle > 4GB volumes */
#define VOLPBIT_XBFREE  9
#define VOLPBIT_XBTOTAL 10
#define VOLPBIT_BSIZE   11        /* block size */
/* ----------------------------- */

/* from etc/afpd/fork.h */
#define AFPOF_DFORK 0x00
#define AFPOF_RFORK 0x80
 
#define OPENFORK_DATA   (0)
#define OPENFORK_RSCS   (1<<7)
  
#define OPENACC_RD      (1<<0)
#define OPENACC_WR      (1<<1)
#define OPENACC_DRD     (1<<4)
#define OPENACC_DWR     (1<<5)
   
#define AFPFORK_OPEN    (1<<0)
#define AFPFORK_RSRC    (1<<1)
#define AFPFORK_DATA    (1<<2)
#define AFPFORK_DIRTY   (1<<3)
#define AFPFORK_ACCRD   (1<<4)
#define AFPFORK_ACCWR   (1<<5)
#define AFPFORK_ACCMASK (AFPFORK_ACCRD | AFPFORK_ACCWR)

   
typedef struct CONN {
	DSI	dsi;
#if 0
	ASP asp;
#endif	
	int type;
	int afp_version;
} CONN;

#define min(a,b)  ((a) < (b) ? (a) : (b)) 

#define PASSWDLEN 8

#define dsi_clientID(x)   ((x)->clientID++)    

#define my_dsi_send(x)       do { \
    (x)->header.dsi_len = htonl((x)->cmdlen); \
    my_dsi_stream_send((x), (x)->commands, (x)->cmdlen); \
} while (0)

int my_dsi_cmd_receive(DSI *x);
int my_dsi_data_receive(DSI *x);

/* from
   modified
 */
/* Files and directories */
struct afp_filedir_parms {
    int isdir;
    u_int16_t bitmap;  /* Parameters already taken from svr */
    u_int16_t attr;
    u_int16_t vid;
    u_int32_t pdid;
    u_int32_t did;
    u_int32_t bdate, mdate, cdate;
    u_int32_t dflen, rflen;
    u_int64_t ext_dflen;
    u_int64_t ext_rflen;
    u_int16_t offcnt;
    u_int32_t uid, gid;
    u_int32_t unix_priv;   /* FIXME what if mode_t != u_int32_t */
    u_int8_t access[4];    /* Access bits */
    u_int8_t pdinfo[6];    /* ProDOS info... */
    char finder_info[32];            // FIXME: Finder info !
    int  name_type;
    char *lname;
    char *sname;
    char *utf8_name;
};

struct afp_volume_parms {
    u_int8_t state;   // FIXME: keep state across calls here (OPENED/CLOSED)
    u_int8_t flags;
    u_int16_t attr;
    u_int16_t sig;
    u_int32_t cdate, bdate, mdate;
    u_int16_t vid;
    u_int32_t bfree, btotal, bsize;
    char *name;
    char *utf8_name;
};

void afp_volume_unpack(struct afp_volume_parms *parms, unsigned char *b, u_int16_t rbitmap);

void afp_filedir_unpack(struct afp_filedir_parms *filedir, unsigned char *b, u_int16_t rfbitmap, u_int16_t rdbitmap);
int afp_filedir_pack(unsigned char *b, struct afp_filedir_parms *filedir, u_int16_t rfbitmap, u_int16_t rdbitmap);

/* 
 afpcli.c
*/
int OpenClientSocket(char* host,int port);
int CloseClientSocket(int fd);


size_t my_dsi_stream_read(DSI *dsi, void *data, const size_t length);
int my_dsi_stream_receive(DSI *dsi, void *buf, const size_t ilength, size_t *rlength);
size_t my_dsi_stream_write(DSI *dsi, void *data, const size_t length);
int my_dsi_stream_send(DSI *dsi, void *buf, size_t length);

int DSIOpenSession(CONN *conn);
int DSIGetStatus(CONN *conn);
int DSICloseSession(CONN *conn);

int AFPopenLogin(CONN *conn, char *vers, char *uam, char *usr, char *pwd);
int AFPopenLoginExt(CONN *conn, char *vers, char *uam, char *usr, char *pwd);
int AFPLogOut(CONN *conn);
int AFPChangePW(CONN *conn, char *uam, char *usr, char *opwd, char *pwd);

int AFPzzz(CONN *conn, int);

int AFPGetSrvrInfo(CONN *conn);
int AFPGetSrvrParms(CONN *conn);
int AFPGetSrvrMsg(CONN *conn, u_int16_t type, u_int16_t bitmap);

int AFPCloseVol(CONN *conn, u_int16_t vol);
int AFPCloseDT(CONN *conn, u_int16_t vol);

int AFPByteLock(CONN *conn, u_int16_t fork, int end, int mode, int offset, int size );
int AFPByteLock_ext(CONN *conn, u_int16_t fork, int end, int mode, off_t offset, off_t size );
int AFPCloseFork(CONN *conn, u_int16_t vol);
int AFPFlush(CONN *conn, u_int16_t vol);
int AFPFlushFork(CONN *conn, u_int16_t vol);
unsigned int AFPDelete(CONN *conn, u_int16_t vol, int did , char *name);

int AFPGetComment(CONN *conn, u_int16_t vol, int did , char *name);
int AFPRemoveComment(CONN *conn, u_int16_t vol, int did , char *name);
int AFPAddComment(CONN *conn, u_int16_t vol, int did , char *name, char *cmt);

u_int16_t AFPOpenVol(CONN *conn, char *vol, u_int16_t bitmap);
u_int16_t AFPOpenFork(CONN *conn, u_int16_t vol, char type, u_int16_t bitmap, int did , char *name,u_int16_t access);

int AFPGetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap);
int AFPSetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap, struct afp_volume_parms *parms);

unsigned int  AFPCreateFile(CONN *conn, u_int16_t vol, char type, int did , char *name);
int  AFPCreateDir(CONN *conn, u_int16_t vol, int did , char *name);

int AFPWriteHeader(DSI *dsi, u_int16_t fork, int offset, int size, char *data, char whence);
int AFPWriteFooter(DSI *dsi, u_int16_t fork, int offset, int size, char *data, char whence);
int AFPWrite(CONN *conn, u_int16_t fork, int offset, int size, char *data, char whence);
int AFPWrite_ext(CONN *conn, u_int16_t fork, off_t offset, off_t size, char *data, char whence);

int AFPReadHeader(DSI *dsi, u_int16_t fork, int offset, int size, char *data);
int AFPReadFooter(DSI *dsi, u_int16_t fork, int offset, int size, char *data);
int AFPRead(CONN *conn, u_int16_t fork, int offset, int size, char *data);
int AFPRead_ext(CONN *conn, u_int16_t fork, off_t offset, off_t size, char *data);

int AFPGetForkParam(CONN *conn, u_int16_t fork, u_int16_t bitmap);

int AFPGetSessionToken(CONN *conn, int type, u_int32_t time, int len, char *token);
int AFPDisconnectOldSession(CONN *conn, u_int16_t type, int len, char *token);

int AFPMapID(CONN *conn, char fn, int id);
int AFPMapName(CONN *conn, char fn, char *name );

int AFPAddAPPL(CONN *conn, u_int16_t dt, int did, char *creator, u_int32_t tag, char *name);
int AFPGetAPPL(CONN *conn, u_int16_t dt, char *name, u_int16_t index, u_int16_t f_bitmap);
int AFPRemoveAPPL(CONN *conn, u_int16_t dt, int did, char *creator, char *name);

int AFPGetUserInfo(CONN *conn, char flag, int id, u_int16_t bitmap);
int AFPBadPacket(CONN *conn, char fn, char *name );

int AFPCatSearch(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap,u_int16_t d_bitmap,
u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2);

int AFPCatSearchExt(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap,u_int16_t d_bitmap,
u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2);

unsigned int AFPSetFileParams(CONN *, u_int16_t vol, int did, char *name, u_int16_t bitmap, struct afp_filedir_parms *);
unsigned int AFPSetForkParam(CONN *conn, u_int16_t fork,  u_int16_t bitmap, off_t size);

int AFPGetACL(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, char *name);
int AFPListExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, int maxsize, char* pathname);
int AFPGetExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, int maxsize, char* pathname, char* attrname);
int AFPSetExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, char* pathname, char* attrname, char* data);
int AFPRemoveExtAttr(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap, char* pathname, char* attrname);

int FPset_name(CONN *conn, int ofs, char *name);
void u2mac(char *dst, char *name, int len);

char *strp2cdup(char *src);

#endif

/* ---------------------------------
*/
