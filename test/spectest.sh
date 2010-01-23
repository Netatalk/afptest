#!/bin/sh

ret=0
. spectest.conf
rm -f spectest.log

##
echo -n "Running spectest with one user ..."
./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -w "$PASSWD" -s "$VOLUME" > spectest.log 2>&1

if [ "$?" -ne 0 ] ; then
    echo "[error]"
    ret=1
else
    echo "[OK]"
fi

##
echo -n "Running spectest with two user ..."
./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" >> spectest.log 2>&1

if [ "$?" -ne 0 ] ; then
    echo "[error]"
    ret=1
else
    echo "[OK]"
fi

##
echo -n "Running spectest with local filesystem modifications..."
./T2_spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" >> spectest.log 2>&1

if [ "$?" -ne 0 ] ; then
    echo "[error]"
    ret=1
else
    echo "[OK]"
fi

if [ $ret -ne 0 ] ; then
    echo The following individual tests failed
    echo =====================================
    grep "summary.*FAIL" spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================
fi

exit $ret