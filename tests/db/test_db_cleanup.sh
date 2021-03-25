#!/bin/bash

#
# This script clean up DB for tests
#

set -ex

THIS_DIR=$(dirname "$0")

source $THIS_DIR/test_db_defines.sh

if psql --command "SELECT 1;" &> /dev/null; then
    CMD_PREFIX=""
else
    CMD_PREFIX="sudo -u postgres"
fi

$CMD_PREFIX dropdb --if-exists $SHARED_DB_NAME

$CMD_PREFIX dropuser --if-exists $READ_USER_NAME

$CMD_PREFIX dropuser --if-exists $ADMIN_USER_NAME
