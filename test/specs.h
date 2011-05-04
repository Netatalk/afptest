/* -------------------------------------
*/
#include "afpclient.h"
#include "test.h"

extern u_int16_t VolID;
extern int Mac;

extern int ExitCode;
extern int Exclude;

#include <signal.h>  

#define FAIL(a) if ((a)) { failed();}
#define FAILEXIT(a, label) if ((a)) { failed(); goto label;}
#define STATIC 

extern void illegal_fork(DSI * dsi, char cmd, char *name);
extern int no_access_folder(u_int16_t vol, int did, char *name);
extern int read_only_folder(u_int16_t vol, int did, char *name);
extern int delete_folder(u_int16_t vol, int did, char *name);

extern int get_did(CONN *conn, u_int16_t vol, int dir, char *name);
extern int get_fid(CONN *conn, u_int16_t vol, int dir, char *name);
extern u_int32_t get_forklen(DSI *dsi, int type);

extern void write_fork(CONN *conn, u_int16_t vol,int dir, char *name, char *data);
extern void read_fork(CONN *conn, u_int16_t vol,int dir, char *name,int len);

extern int read_only_folder_with_file(u_int16_t vol, int did, char *name, char *file);
extern int delete_folder_with_file(u_int16_t vol, int did, char *name, char *file);
extern int get_vol_attrib(u_int16_t vol) ;
extern int group_folder(u_int16_t vol, int did, char *name);
extern unsigned int get_vol_free(u_int16_t vol) ;
extern ssize_t get_sessiontoken(const char *buf, char **token);

extern void failed_nomsg(void);
extern void skipped_nomsg(void);
extern void failed(void);
extern void known_failure(char *why);
extern void enter_test(void);
extern void exit_test(char *name);

extern void nottested(void);
extern int not_valid(unsigned int ret, int mac_error, int afpd_error);
extern int not_valid_bitmap(unsigned int ret, unsigned int bitmap, int afpd_error);
extern void test_skipped(int why);

#define T_CONN2      1
#define T_PATH       2
#define T_AFP3       3
#define T_AFP3_CONN2 4
#define T_MAC_PATH   5
#define T_UNIX_PREV  6
#define T_UTF8       7
#define T_VOL2       8
#define T_LOCKING    9
#define T_VOL_SMALL  10
#define T_ID         11
#define T_AFP2       12
#define T_MAC        13
#define T_ACL        14
#define T_EA         15
#define T_UNIX_GROUP 16

/* ---------------------------------
*/
