#!/bin/bash

#
# This script installs required dependencies.
#

sudo apt update

sudo apt --yes install libpq-dev postgresql-server-dev-all
sudo apt --yes install libedit-dev
