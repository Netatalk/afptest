SUBDIRS =
 
bin_PROGRAMS = afpclient loadtest spectest postmark fail_spectest T2_spectest logintest rotest
 
noinst_LTLIBRARIES = libafptest.la
 
libafptest_la_SOURCES = afpcli.c afpcmd.c afphelper.c adoublehelper.c
 
afpclient_SOURCES = main.c testsrc.c
afpclient_LDFLAGS = -export-dynamic
afpclient_LDADD = libafptest.la
 
postmark_SOURCES = postmark-1_5.c
postmark_LDFLAGS = -export-dynamic
postmark_LDADD = libafptest.la
#
loadtest_SOURCES = loadtest.c
loadtest_LDADD = libafptest.la
 
logintest_SOURCES = logintest.c
logintest_LDADD = libafptest.la
 
rotest_SOURCES = rotest.c
rotest_LDADD = libafptest.la
 
#
spectest_SOURCES = spectest.c FPAddAPPL.c FPAddComment.c FPAddIcon.c \
        FPByteRangeLock.c FPByteRangeLockExt.c FPCloseFork.c FPCopyFile.c FPCreateDir.c FPCreateFile.c\
        FPCloseDir.c FPCloseDT.c FPCloseVol.c FPDelete.c FPExchangeFiles.c FPOpenDT.c FPOpenFork.c \
	FPOpenVol.c FPGetAPPL.c FPGetIcon.c FPGetIconInfo.c FPGetSrvrInfo.c FPGetSrvrMsg.c FPGetSrvrParms.c\
        FPGetComment.c FPGetForkParms.c FPGetFileDirParms.c FPGetVolParms.c FPGetUserInfo.c\
        FPEnumerate.c FPEnumerateExt.c FPEnumerateExt2.c FPFlush.c FPFlushFork.c FPMapID.c FPMapName.c \
	FPMoveAndRename.c FPOpenDir.c FPSetDirParms.c \
        FPSetFileDirParms.c FPSetFileParms.c FPDisconnectOldSession.c\
        FPSetForkParms.c FPSetVolParms.c FPRead.c FPReadExt.c FPRename.c FPRemoveAPPL.c \
	FPRemoveComment.c FPResolveID.c FPGetSessionToken.c \
        FPWrite.c FPWriteExt.c Error.c
 
 
spectest_LDADD = libafptest.la
#
fail_spectest_SOURCES = spectest.c failed_FPEnumerate.c failed_FPExchangeFiles.c \
	failed_FPMoveAndRename.c failed_Error.c

fail_spectest_CFLAGS = -DQUIRK
 
fail_spectest_LDADD = libafptest.la
 
#
T2_spectest_SOURCES = T2_spectest.c \
        T2_FPDelete.c T2_FPGetFileDirParms.c T2_FPOpenFork.c T2_FPSetDirParms.c T2_FPSetFileParms.c \
	T2_FPMoveAndRename.c

T2_spectest_LDADD = libafptest.la
 
noinst_HEADERS = adoublehelper.h afpclient.h specs.h test.h 
