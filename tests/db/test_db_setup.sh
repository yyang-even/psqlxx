#!/bin/bash

#
# This script setup DB for tests
#

set -ex

THIS_DIR=$(dirname "$0")

source $THIS_DIR/test_db_defines.sh

if psql --command "SELECT 1;" &> /dev/null; then
    CMD_PREFIX=""
else
    CMD_PREFIX="sudo -u postgres"
fi

$CMD_PREFIX psql --command "CREATE USER $ADMIN_USER_NAME WITH CREATEDB NOCREATEROLE LOGIN PASSWORD '$ADMIN_USER_PASSWORD';"

$CMD_PREFIX psql --command "CREATE USER $READ_USER_NAME WITH NOCREATEDB NOCREATEROLE LOGIN PASSWORD '$READ_USER_PASSWORD';"

$CMD_PREFIX createdb --owner=$ADMIN_USER_NAME $SHARED_DB_NAME
