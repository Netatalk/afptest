#!/bin/sh

ret=0

check_return() {
    ERRNO=$?
    if test $ERRNO -eq 1 -o $ERRNO -gt 3 ; then
        echo "[error: $ERRNO]"
        ret=1
    else
        echo "[OK]"
    fi    
}

echo =====================================

if [ ! -f spectest.conf ] ; then
    cat > spectest.conf <<EOF
# AFPSERVER=127.0.0.1
# AFPPORT=548
# USER1=
# USER2=
# PASSWD=
# VOLUME=
# LOCALVOLPATH=

# AFPVERSION: 3 = AFP 3.0, 4 = AFP 3.1, 5 = AFP 3.2
# AFPVERSION=5
EOF
    echo 'A template configuration file "spectest.conf" has been generated.'
    echo Adjust it to match your setup.
    echo =====================================
    exit 1
fi

. spectest.conf
rm -f spectest.log

##
echo -n "Running spectest with one user ..."
./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -w "$PASSWD" -s "$VOLUME" > spectest.log 2>&1
check_return

##
echo -n "Running spectest with two user ..."
./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" >> spectest.log 2>&1
check_return

##
echo -n "Running spectest with local filesystem modifications..."
./T2_spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" -c "$LOCALVOLPATH" >> spectest.log 2>&1
check_return

if [ $ret -ne 0 ] ; then
    echo The following individual tests failed
    echo =====================================
    grep "summary.*FAIL" spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================
fi

exit $ret