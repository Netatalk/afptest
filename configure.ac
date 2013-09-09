AC_INIT([afptest], [1.0.6])
AC_CONFIG_SRCDIR([test/spectest.c])
AM_INIT_AUTOMAKE(foreign)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE([enable])
AC_CANONICAL_HOST
AC_PROG_CC
AM_PROG_LIBTOOL

case "$host_os" in
	*aix*)				this_os=aix ;;
	*freebsd*) 			this_os=freebsd ;;
	*hpux11*)			this_os=hpux11 ;;
	*irix*)				this_os=irix ;;
	*linux*)   			this_os=linux ;;
	*osx*)				this_os=macosx ;;
	*darwin*)			this_os=macosx ;;
	*netbsd*) 			this_os=netbsd ;;
	*openbsd*) 			this_os=openbsd ;;
	*osf*) 				this_os=tru64 ;;
	*solaris*) 			this_os=solaris ;;
esac

AC_SYS_LARGEFILE
AC_CHECK_HEADERS([byteswap.h fcntl.h strings.h sys/fcntl.h unistd.h sys/cdefs.h])
AC_CHECK_LIB(dl, dlopen)
AC_CHECK_LIB(nsl, main)
AC_CHECK_LIB(socket, socket)
AC_CHECK_FUNC(attropen, AC_DEFINE(HAVE_EAFD, 1, [Platform has seekable extended attributes fds]))
AC_CHECK_FUNCS(strlcpy strlcat)

AC_MSG_CHECKING(for bswap_64)
AC_TRY_LINK([#if HAVE_BYTESWAP_H
		 #include <byteswap.h>
		 #endif]
           ,[bswap_64(37);]
           ,neta_cv_bswap_64=yes,neta_cv_bswap_64=no)
AC_MSG_RESULT($neta_cv_bswap_64)
if test x"$neta_cv_bswap_64" = xyes; then
  AC_DEFINE(HAVE_BSWAP_64,1,[Define to 1 if bswap_64 is available via byteswap.h])
fi

dnl Check for Extended Attributes support
neta_cv_eas="ad"
neta_cv_eas_sys_found=no
neta_cv_eas_sys_not_found=no

AC_CHECK_HEADERS(sys/attributes.h attr/xattr.h sys/xattr.h sys/extattr.h sys/uio.h sys/ea.h)

case "$this_os" in

  *osf*)
	AC_SEARCH_LIBS(getproplist, [proplist])
	AC_CHECK_FUNCS([getproplist fgetproplist setproplist fsetproplist],
                   [neta_cv_eas_sys_found=yes],
                   [neta_cv_eas_sys_not_found=yes])
	AC_CHECK_FUNCS([delproplist fdelproplist add_proplist_entry get_proplist_entry],,
                   [neta_cv_eas_sys_not_found=yes])
	AC_CHECK_FUNCS([sizeof_proplist_entry],,
                   [neta_cv_eas_sys_not_found=yes])
  ;;

  *solaris*)
	AC_CHECK_FUNCS([attropen],
                   [neta_cv_eas_sys_found=yes],
                   [neta_cv_eas_sys_not_found=yes])
  ;;

  'freebsd')
    AC_CHECK_FUNCS([extattr_delete_fd extattr_delete_file extattr_delete_link],
                   [neta_cv_eas_sys_found=yes],
                   [neta_cv_eas_sys_not_found=yes])
    AC_CHECK_FUNCS([extattr_get_fd extattr_get_file extattr_get_link],,
                   [neta_cv_eas_sys_not_found=yes])
    AC_CHECK_FUNCS([extattr_list_fd extattr_list_file extattr_list_link],,
                   [neta_cv_eas_sys_not_found=yes])
    AC_CHECK_FUNCS([extattr_set_fd extattr_set_file extattr_set_link],,
                   [neta_cv_eas_sys_not_found=yes])
  ;;

  *freebsd4* | *dragonfly* )
    AC_DEFINE(BROKEN_EXTATTR, 1, [Does extattr API work])
  ;;

  *)
	AC_SEARCH_LIBS(getxattr, [attr])

    if test "x$neta_cv_eas_sys_found" != "xyes" ; then
       AC_CHECK_FUNCS([getxattr lgetxattr fgetxattr listxattr llistxattr],
                      [neta_cv_eas_sys_found=yes],
                      [neta_cv_eas_sys_not_found=yes])
	   AC_CHECK_FUNCS([flistxattr removexattr lremovexattr fremovexattr],,
                      [neta_cv_eas_sys_not_found=yes])
	   AC_CHECK_FUNCS([setxattr lsetxattr fsetxattr],,
                      [neta_cv_eas_sys_not_found=yes])
    fi

    if test "x$neta_cv_eas_sys_found" != "xyes" ; then
	   AC_CHECK_FUNCS([getea fgetea lgetea listea flistea llistea],
                      [neta_cv_eas_sys_found=yes],
                      [neta_cv_eas_sys_not_found=yes])
	   AC_CHECK_FUNCS([removeea fremoveea lremoveea setea fsetea lsetea],,
                      [neta_cv_eas_sys_not_found=yes])
    fi

    if test "x$neta_cv_eas_sys_found" != "xyes" ; then
	   AC_CHECK_FUNCS([attr_get attr_list attr_set attr_remove],,
                      [neta_cv_eas_sys_not_found=yes])
       AC_CHECK_FUNCS([attr_getf attr_listf attr_setf attr_removef],,
                      [neta_cv_eas_sys_not_found=yes])
    fi
  ;;
esac

# Do xattr functions take additional options like on Darwin?
if test x"$ac_cv_func_getxattr" = x"yes" ; then
	AC_CACHE_CHECK([whether xattr interface takes additional options], smb_attr_cv_xattr_add_opt, [
		old_LIBS=$LIBS
		LIBS="$LIBS $ACL_LIBS"
		AC_TRY_COMPILE([
			#include <sys/types.h>
			#if HAVE_ATTR_XATTR_H
			#include <attr/xattr.h>
			#elif HAVE_SYS_XATTR_H
			#include <sys/xattr.h>
			#endif
		],[
			getxattr(0, 0, 0, 0, 0, 0);
		],
	        [smb_attr_cv_xattr_add_opt=yes],
		[smb_attr_cv_xattr_add_opt=no;LIBS=$old_LIBS])
	])
	if test x"$smb_attr_cv_xattr_add_opt" = x"yes"; then
		AC_DEFINE(XATTR_ADD_OPT, 1, [xattr functions have additional options])
	fi
fi

if test "x$neta_cv_eas_sys_found" = "xyes" ; then
   if test "x$neta_cv_eas_sys_not_found" != "xyes" ; then
      neta_cv_eas="$neta_cv_eas | sys"
   fi
fi

AC_OUTPUT([Makefile
	macros/Makefile
	test/Makefile])

