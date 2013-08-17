#!/bin/sh

ret=0

check_return() {
    ERRNO=$?
    if test $ERRNO -eq 0 -o $ERRNO -eq 3 ; then
        echo "[OK]"
    else
        echo "[error: $ERRNO]"
        ret=1
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
#
# *** volume with adouble:ea
# VOLUME=
# LOCALVOLPATH=

# AFPVERSION: 3 = AFP 3.0, 4 = AFP 3.1, 5 = AFP 3.2
# AFPVERSION=5
EOF
    echo 'A template configuration file "spectest.conf" has been generated.'
    echo Adjust it to match your setup.
    echo =====================================
    exit 0
fi

. ./spectest.conf

# cleanup
if test ! -z "$LOCALVOLPATH" ; then
    rm -rf "$LOCALVOLPATH"/t*
fi

if [ ! -z "$VOLUME" -a ! -z "$LOCALVOLPATH" ] ; then

    rm -f spectest.log

##
    printf "Running spectest with two users..."
    ./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME"  >> spectest.log 2>&1
    check_return

##
    printf "Running spectest with local filesystem modifications..."
    ./T2_spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" -c "$LOCALVOLPATH" >> spectest.log 2>&1
    check_return

    echo
    echo Failed tests
    grep "summary.*FAIL" spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================

    echo
    echo Skipped tests
    egrep 'summary.*NOT TESTED|summary.*SKIPPED' spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================

    echo
    echo Successfull tests
    grep "summary.*PASSED" spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================

fi

# cleanup
if test ! -z "$LOCALVOLPATH" ; then
    rm -rf "$LOCALVOLPATH"/t*
fi

exit $ret
