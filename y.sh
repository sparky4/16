#!/bin/bash

# run this from the top dir of the project
topdir=`pwd`

# force rebuild of the project
wmake clean
# force rebuild of doslib, make sure the latest is checked out from master
cd "$topdir/src/lib/doslib" || exit 1
git pull
git checkout master
git pull
./buildall.sh clean
# back to top dir, build project again
cd "$topdir" || exit 1
./wbuild.sh
