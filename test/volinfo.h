#ifndef _ATALK_VOLINFO_H
#define _ATALK_VOLINFO_H 1

#include <stdint.h>

#define ADFLAGS_DIR (1<<0)

#define AD_VERSION2   1
#define AD_VERSION_EA 2

#define AFPVOL_EA_AD  1
#define AFPVOL_EA_SYS 2

/* volinfo for shell utilities */
#define VOLINFODIR  ".AppleDesktop"
#define VOLINFOFILE ".volinfo"

typedef struct {
    const uint32_t option;
    const char      *name;
} vol_opt_name_t;

struct volinfo {
    char                *v_name;
    char                *v_path;
    int                 v_flags;
    int                 v_casefold;
    char                *v_cnidscheme;
    char                *v_dbpath;
    int                 v_adouble;  /* default adouble format */
    int                 v_vfs_ea;
    const char          *(*ad_path)(const char *, int);
    char                *v_dbd_host;
    char                *v_dbd_port;
};

extern struct volinfo volinfo;

extern int loadvolinfo(char *path);
extern const char *ad_path(const char *path, int adflags);
extern const char *ad_path_ea(const char *path, int adflags);
extern const char *ad_path_osx(const char *path, int adflags);
#endif
