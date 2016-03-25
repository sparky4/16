wmake clean
cd src/lib/doslib
git pull
git checkout master
git pull
. ./buildall.sh clean
. ./wbuild.sh
