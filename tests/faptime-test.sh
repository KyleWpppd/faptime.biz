#! /usr/bin/env sh
BEFORE=`set -o posix; set`

CGI_VARS=""
REMOTE_ADDR="127.0.0.1"
REMOTE_PORT="80"
SERVER_ADDR="127.0.0.1"
SERVER_ADMIN="foo@example.com"
SERVER_NAME="127.0.0.1"
SERVER_PORT="80"
SERVER_PROTOCOL="HTTP/1.1"
SERVER_SIGNATURE=""
SERVER_SOFTWARE=""
SYSTEMROOT="/"

# Defaults

SCRIPT_VARS="`grep -vFe "$BEFORE" <<<"$(set -o posix ; set)" | grep -v ^VARS=`"; unset VARS;


FAPTIME_EXECUTABLE="$PWD"/"$2"
GET_PATH="$3"
plain_get () {
    SCRIPT_VARS+=' REQUEST_METHOD=GET '
    URI="REQUEST_URI=$GET_PATH"
    SCRIPT_VARS+=$URI
    ALL_VARS="${SCRIPT_VARS}"
    echo $SCRIPT_VARS
    echo "============"
    RES=`env $SCRIPT_VARS "$FAPTIME_EXECUTABLE"`
    echo "$RES"
}

eval "$1"
