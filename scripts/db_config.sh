#!/bin/bash

#
# This script setup DB for current user
#

sudo -u postgres createuser --superuser $USER
sudo -u postgres createdb $USER
