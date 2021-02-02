#!/bin/bash

#
# This script setup develop environment by installing required libraries
#

# THIS_DIR=$(dirname "$0")
# $THIS_DIR/dependencies.sh
#
echo "Setup submodules."
git submodule update --init

pre-commit install
