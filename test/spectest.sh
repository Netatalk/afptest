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
# *** volume with adouble:v2
# VOLUME=
# LOCALVOLPATH=
# *** volume with adouble:ea
# EA_VOLUME=
# EA_LOCALVOLPATH=

# AFPVERSION: 3 = AFP 3.0, 4 = AFP 3.1, 5 = AFP 3.2
# AFPVERSION=5
EOF
    echo 'A template configuration file "spectest.conf" has been generated.'
    echo Adjust it to match your setup.
    echo =====================================
    exit 0
fi

. ./spectest.conf
rm -f spectest.log

# cleanup
if test ! -z "$LOCALVOLPATH" ; then
    rm -rf "$LOCALVOLPATH"/t*
fi
if test ! -z "$EA_LOCALVOLPATH" ; then
    rm -rf "$EA_LOCALVOLPATH"/t*
fi

if [ ! -z "$VOLUME" -a ! -z "$LOCALVOLPATH" ] ; then

    echo "Running tests with adouble:v2"
    echo "============================="

##
    printf "Running spectest with one user ..."
    ./spectest -a -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -w "$PASSWD" -s "$VOLUME" -c "$LOCALVOLPATH" >> spectest.log 2>&1
    check_return

##
    printf "Running spectest with two user ..."
    ./spectest -a -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" -c "$LOCALVOLPATH" >> spectest.log 2>&1
    check_return

##
    printf "Running spectest with local filesystem modifications..."
    ./T2_spectest -a -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$VOLUME" -c "$LOCALVOLPATH" >> spectest.log 2>&1
    check_return

    if [ $ret -ne 0 ] ; then
        echo The following individual tests failed
        echo =====================================
        grep "summary.*FAIL" spectest.log | sed s/test//g | sort -n | uniq
        echo =====================================
    fi

    echo
    echo The following tests were skipped
    echo =====================================
    grep "NOT TESTED" spectest.log | sed s/test//g | sort -n | uniq
    echo =====================================
fi

if [ ! -z "$EA_VOLUME" -a ! -z "$EA_LOCALVOLPATH" ] ; then

    echo "Running tests with adouble:ea"
    echo "============================="

    rm -f spectest-ea.log

##
    printf "Running spectest with one user ..."
    ./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -w "$PASSWD" -s "$EA_VOLUME" -c "$EA_LOCALVOLPATH" >> spectest-ea.log 2>&1
    check_return

##
    printf "Running spectest with two user ..."
    ./spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$EA_VOLUME" -c "$EA_LOCALVOLPATH" >> spectest-ea.log 2>&1
    check_return

##
    printf "Running spectest with local filesystem modifications..."
    ./T2_spectest -"$AFPVERSION" -h "$AFPSERVER" -p "$AFPPORT" -u "$USER1" -d "$USER2" -w "$PASSWD" -s "$EA_VOLUME" -c "$EA_LOCALVOLPATH" >> spectest-ea.log 2>&1
    check_return

    if [ $ret -ne 0 ] ; then
        echo The following individual tests failed
        echo =====================================
        grep "summary.*FAIL" spectest-ea.log | sed s/test//g | sort -n | uniq
        echo =====================================
    fi

    echo
    echo The following tests were skipped
    echo =====================================
    grep "NOT TESTED" spectest-ea.log | sed s/test//g | sort -n | uniq
    echo =====================================

fi

if test -f spectest.log ; then
    echo
    echo List of test results
    echo ====================
    grep summary spectest.log | grep -v PASSED
fi

# cleanup
if test ! -z "$LOCALVOLPATH" ; then
    rm -rf "$LOCALVOLPATH"/t*
fi
if test ! -z "$EA_LOCALVOLPATH" ; then
    rm -rf "$EA_LOCALVOLPATH"/t*
fi

exit $ret
