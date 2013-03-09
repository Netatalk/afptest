dist_check_SCRIPTS = spectest.sh
noinst_HEADERS = adoublehelper.h afp.h afpclient.h cdefs.h dsi.h specs.h test.h ea.h compat.h
bin_PROGRAMS = afparg afp_ls lantest spectest postmark fail_spectest T2_spectest logintest rotest \
               encoding_test speedtest sleeptest
noinst_LTLIBRARIES = libafptest.la
TESTS = spectest.sh
CLEANFILES = spectest.conf

libafptest_la_SOURCES = afpcli.c afpcmd.c afphelper.c adoublehelper.c extattr.c compat.c

afparg_SOURCES = afparg.c afparg_FPfuncs.c
afparg_LDADD = libafptest.la

postmark_SOURCES = postmark-1_5.c
postmark_LDFLAGS = -export-dynamic
postmark_LDADD = libafptest.la

lantest_SOURCES = lantest.c
lantest_LDADD = libafptest.la

afp_ls_SOURCES = afp_ls.c
afp_ls_LDADD = libafptest.la

encoding_test_SOURCES = encoding_test.c
encoding_test_LDADD = libafptest.la

logintest_SOURCES = logintest.c
logintest_LDADD = libafptest.la

rotest_SOURCES = rotest.c
rotest_LDADD = libafptest.la

speedtest_SOURCES = speedtest.c
speedtest_LDADD = libafptest.la

spectest_SOURCES = spectest.c FPAddAPPL.c FPAddComment.c FPAddIcon.c \
        FPByteRangeLock.c FPByteRangeLockExt.c FPCloseFork.c FPCopyFile.c FPCreateDir.c FPCreateFile.c\
        FPCloseDir.c FPCloseDT.c FPCloseVol.c FPDelete.c FPExchangeFiles.c FPOpenDT.c FPOpenFork.c \
	FPOpenVol.c FPGetAPPL.c FPGetIcon.c FPGetIconInfo.c FPGetSrvrInfo.c FPGetSrvrMsg.c FPGetSrvrParms.c\
        FPGetComment.c FPGetForkParms.c FPGetFileDirParms.c FPGetVolParms.c FPGetUserInfo.c\
        FPEnumerate.c FPEnumerateExt.c FPEnumerateExt2.c FPFlush.c FPFlushFork.c FPMapID.c FPMapName.c \
	FPMoveAndRename.c FPOpenDir.c FPSetDirParms.c FPCatSearch.c FPCatSearchExt.c\
        FPSetFileDirParms.c FPSetFileParms.c FPDisconnectOldSession.c\
        FPSetForkParms.c FPSetVolParms.c FPRead.c FPReadExt.c FPRename.c FPRemoveAPPL.c \
	FPRemoveComment.c FPResolveID.c FPGetSessionToken.c \
        FPWrite.c FPWriteExt.c FPzzz.c Error.c Utf8.c FPGetACL.c \
		FPSync.c
spectest_LDADD = libafptest.la

sleeptest_SOURCES = sleeptest.c FPzzz.c
sleeptest_LDADD = libafptest.la

fail_spectest_SOURCES = fail_spectest.c failed_FPEnumerate.c failed_FPExchangeFiles.c \
	failed_FPMoveAndRename.c failed_Error.c
fail_spectest_CFLAGS = -DQUIRK
fail_spectest_LDADD = libafptest.la

T2_spectest_SOURCES = T2_spectest.c T2_FPByteRangeLock.c\
        T2_FPDelete.c T2_FPGetFileDirParms.c T2_FPOpenFork.c T2_FPSetDirParms.c T2_FPSetFileParms.c \
	T2_FPMoveAndRename.c T2_FPResolveID.c T2_FPGetSrvrParms.c T2_FPCreateFile.c \
	T2_FPCopyFile.c T2_Dircache_attack.c T2_FPRead.c
T2_spectest_LDADD = libafptest.la

