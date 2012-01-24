/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Copied from Netatalk's volinfo.c
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>

#include "volinfo.h"

/* Globally visible */
struct volinfo volinfo;

typedef struct {
    const char *name;
    int type;
} info_option_t;

#define MAC_CHARSET 0
#define VOL_CHARSET 1
#define ADOUBLE_VER 2
#define CNIDBACKEND 3
#define CNIDDBDHOST 4
#define CNIDDBDPORT 5
#define CNID_DBPATH 6
#define VOLUME_OPTS 7
#define VOLCASEFOLD 8
#define EXTATTRTYPE 9

static const info_option_t info_options[] = {
    {"MAC_CHARSET", MAC_CHARSET},
    {"VOL_CHARSET", VOL_CHARSET},
    {"ADOUBLE_VER", ADOUBLE_VER},
    {"CNIDBACKEND", CNIDBACKEND},
    {"CNIDDBDHOST", CNIDDBDHOST},
    {"CNIDDBDPORT", CNIDDBDPORT},
    {"CNID_DBPATH", CNID_DBPATH},
    {"VOLUME_OPTS", VOLUME_OPTS},
    {"VOLCASEFOLD", VOLCASEFOLD},
    {"EXTATTRTYPE", EXTATTRTYPE},
   {NULL, 0}
};

#ifndef HAVE_STRLCPY
/* like strncpy but does not 0 fill the buffer and always null
   terminates. bufsize is the size of the destination buffer */
static size_t strlcpy(char *d, const char *s, size_t bufsize)
{
        size_t len = strlen(s);
        size_t ret = len;

        if (bufsize <= 0) 
        	return 0;

        if (len >= bufsize) 
        	len = bufsize-1;

        memcpy(d, s, len);
        d[len] = 0;
        return ret;
}
#endif
 
#ifndef HAVE_STRLCAT
/* like strncat but does not 0 fill the buffer and always null
   terminates. bufsize is the length of the buffer, which should
   be one more than the maximum resulting string length */
static size_t strlcat(char *d, const char *s, size_t bufsize)
{
        size_t len1 = strlen(d);
        size_t len2 = strlen(s);
        size_t ret = len1 + len2;

	if (len1 >= bufsize) {
		return 0;
	} 
        if (len1+len2 >= bufsize) {
                len2 = bufsize - (len1+1);
        }
        if (len2 > 0) {
                memcpy(d+len1, s, len2);
                d[len1+len2] = 0;
        }
        return ret;
}
#endif

static char* find_in_path( char *path, char *subdir, size_t maxlen)
{
    char 	*pos;
    struct stat st;

    strlcat(path, subdir, maxlen);
    pos = strrchr(path, '/');

    while ( stat(path, &st) != 0) {
        path[pos-path]=0;
        if ((pos = strrchr(path, '/'))) {
            path[pos-path]=0;
            strlcat(path, subdir, maxlen);
        }
        else {
            return NULL;
        }
    }

    path[pos-path] = '/';
    path[pos-path+1] = 0;

    return path;
}

static char * make_path_absolute(char *path, size_t bufsize)
{
    struct	stat st;
    char	savecwd[MAXPATHLEN];
    char 	abspath[MAXPATHLEN];
    char	*p;

    strlcpy(abspath, path, sizeof(abspath));

    /* we might be called from `ad cp ...` with non existing target */
    if (stat(abspath, &st) != 0) {
        if (errno != ENOENT)
            return NULL;

        if (NULL == (p = strrchr(abspath, '/')) )
            /* single component `ad cp SOURCEFILE TARGETFILE`, use "." instead */
            strcpy(abspath, ".");
        else
            /* try without the last path element */
            *p = '\0';

        if (stat(abspath, &st) != 0) {
            return NULL;
        }
    }

    if (S_ISREG(st.st_mode)) {
        /* single file copy SOURCE */
        if (NULL == (p = strrchr(abspath, '/')) )
            /* no path, use "." instead */
            strcpy(abspath, ".");
        else
            /* try without the last path element */
            *p = '\0';
    }

    if (!getcwd(savecwd, sizeof(savecwd)) || chdir(abspath) < 0)	
        return NULL;

    if (!getcwd(abspath, sizeof(abspath)) || chdir (savecwd) < 0)
        return NULL;
    
    if (strlen(abspath) > bufsize)
        return NULL;

    strlcpy(path, abspath, bufsize);
    return path;
}

static char * find_volumeroot(char *path, size_t maxlen)
{
    char *volume = make_path_absolute(path, maxlen);
        
    if (volume == NULL)
       return NULL;

    if (NULL == (find_in_path(volume, "/.AppleDesktop", maxlen)) )
       return NULL;

    return volume;
}

static int parse_options (char *buf, int *flags, const vol_opt_name_t *options)
{
    char *p, *q;
    const vol_opt_name_t *op;

    q = p = buf; 

    while ( *p != '\0') {
        if (*p == ' ') {
            *p = '\0';
            op = options;
            for (;op->name; op++) {
                if ( !strcmp(op->name, q )) {
                    *flags |= op->option;
                    break;
                }
            }
            q = p+1;
        }
        p++;
    }

    return 0;
} 
            


static int parseline ( char *buf, struct volinfo *vol)
{
    char *value;
    size_t len;
    int  option=-1;
    const info_option_t  *p  = &info_options[0];

    if (NULL == ( value = strchr(buf, ':')) )
	return 1;

    *value = '\0';
    value++;

    if ( 0 == (len = strlen(value)) )
        return 0;

    if (value[len-1] == '\n')
        value[len-1] = '\0';

    for (;p->name; p++) {
        if ( !strcmp(p->name, buf )) {
            option=p->type;
            break;
        }
    }

    switch (option) {
      case MAC_CHARSET:
      case CNIDBACKEND:
        if ((vol->v_cnidscheme = strdup(value)) == NULL) {
	    fprintf (stderr, "strdup: %s", strerror(errno));
            return -1;
        }
        break;
      case CNIDDBDHOST:
        if ((vol->v_dbd_host = strdup(value)) == NULL) {
	    fprintf (stderr, "strdup: %s", strerror(errno));
            return -1;
        }
        break;
      case CNIDDBDPORT:
        if ((vol->v_dbd_port = strdup(value)) == NULL) {
	    fprintf (stderr, "strdup: %s", strerror(errno));
            return -1;            
        }
        break;
      case CNID_DBPATH:
          if ((vol->v_dbpath = malloc(MAXPATHLEN+1)) == NULL)
              return -1;
          strcpy(vol->v_dbpath, value);
        break;
      case ADOUBLE_VER:
        if (strcasecmp(value, "v2") == 0) {
            vol->ad_path = ad_path;
            vol->v_adouble = AD_VERSION2;
        } else if (strcasecmp(value, "ea") == 0) {
#if HAVE_EAFD
            vol->ad_path = ad_path_ea;
#else
            vol->ad_path = ad_path_osx;
#endif
            vol->v_adouble = AD_VERSION_EA;
        } else {

	    fprintf (stderr, "unknown adouble version: %s, %s", buf, value);
	    return -1;
        }
        break;
#if 0
      case VOLUME_OPTS:
        parse_options(value, &vol->v_flags, &vol_opt_names[0]);
        break;
      case VOLCASEFOLD:
        parse_options(value, &vol->v_casefold, &vol_opt_casefold[0]);
        break;
#endif
    case EXTATTRTYPE:
        if (strcasecmp(value, "AFPVOL_EA_AD") == 0)    
            vol->v_vfs_ea = AFPVOL_EA_AD;
        else if (strcasecmp(value, "AFPVOL_EA_SYS") == 0)
            vol->v_vfs_ea = AFPVOL_EA_SYS;
        break;
      default:
	fprintf (stderr, "unknown volume information: %s, %s", buf, value);
	return (-1);
        break;
    }
        
    return 0;
}
    

int loadvolinfo (char *path)
{
    struct volinfo *vol = &volinfo;
    char   volinfofile[MAXPATHLEN];
    char   buf[MAXPATHLEN];
    struct flock lock;
    int    fd, len;
    FILE   *fp;

    if ( !path || !vol)
        return -1;

    memset(vol, 0, sizeof(struct volinfo));
    strlcpy(volinfofile, path, sizeof(volinfofile));

    /* volinfo file is in .AppleDesktop */ 
    if ( NULL == find_volumeroot(volinfofile, sizeof(volinfofile)))
        return -1;

    if ((vol->v_path = strdup(volinfofile)) == NULL ) {
        fprintf (stderr, "strdup: %s", strerror(errno));
        return (-1);
    }
    /* Remove trailing slashes */
    len = strlen(vol->v_path);
    while (len && (vol->v_path[len-1] == '/')) {
        vol->v_path[len-1] = 0;
        len--;
    }

    strlcat(volinfofile, ".AppleDesktop/", sizeof(volinfofile));
    strlcat(volinfofile, VOLINFOFILE, sizeof(volinfofile));

    /* open the file read only */
    if ( NULL == (fp = fopen( volinfofile, "r")) )  {
	fprintf (stderr, "error opening volinfo (%s): %s", volinfofile, strerror(errno));
        return (-1);
    }
    fd = fileno(fp); 

    /* try to get a read lock */ 
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;
    lock.l_type   = F_RDLCK;

    /* wait for read lock */
    if (fcntl(fd, F_SETLKW, &lock) < 0) {
        fclose(fp);
        return (-1);
    }

    /* read the file */
    while (NULL != fgets(buf, sizeof(buf), fp)) {
        parseline(buf, vol);
    }

    /* unlock file */
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    fclose(fp);
    return 0;
}

const char *ad_path_ea( const char *path, int adflags)
{
    return path;
}

const char *ad_path_osx(const char *path, int adflags)
{
    static char pathbuf[ MAXPATHLEN + 1];
    char    c, *slash, buf[MAXPATHLEN + 1];

    if (!strcmp(path,".")) {
        /* fixme */
        getcwd(buf, MAXPATHLEN);
    }
    else {
        strlcpy(buf, path, MAXPATHLEN +1);
    }
    if (NULL != ( slash = strrchr( buf, '/' )) ) {
        c = *++slash;
        *slash = '\0';
        strlcpy( pathbuf, buf, MAXPATHLEN +1);
        *slash = c;
    } else {
        pathbuf[ 0 ] = '\0';
        slash = buf;
    }
    strlcat( pathbuf, "._", MAXPATHLEN  +1);
    strlcat( pathbuf, slash, MAXPATHLEN +1);
    return pathbuf;
}

const char *ad_path( const char *path, int adflags)
{
    static char pathbuf[ MAXPATHLEN + 1];
    const char *slash;
    size_t  l ;

    if ( adflags & ADFLAGS_DIR ) {
        l = strlcpy( pathbuf, path, sizeof(pathbuf));

        if ( l && l < MAXPATHLEN) {
            pathbuf[l++] = '/';
        }
        strlcpy(pathbuf +l, ".AppleDouble/.Parent", sizeof(pathbuf) -l);
    } else {
        if (NULL != ( slash = strrchr( path, '/' )) ) {
            slash++;
            l = slash - path;
            /* XXX we must return NULL here and test in the caller */
            if (l > MAXPATHLEN)
                l = MAXPATHLEN;
            memcpy( pathbuf, path, l);
        } else {
            l = 0;
            slash = path;
        }
        l += strlcpy( pathbuf +l, ".AppleDouble/", sizeof(pathbuf) -l);
        strlcpy( pathbuf + l, slash, sizeof(pathbuf) -l);
    }

    return( pathbuf );
}
