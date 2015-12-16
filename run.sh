#!/bin/bash
#CACTUS
export CACTUSBIN=/opt/cactus/bin
export CACTUSLIB=/opt/cactus/lib
export CACTUSINCLUDE=/opt/cactus/include


# BOOST
export BOOST_LIB=/opt/cactus/lib
export BOOST_INCLUDE=/opt/cactus/include

# QT if installed
#export QTDIR=/usr/local/Trolltech/Qt-4.8.5
#export QMAKESPEC=
#export QTROOTSYSDIR=/usr/local/qtRoot/root

#ROOT
#source /usr/local/bin/thisroot.sh

#FED Burnin
export BASE_DIR=/home/fectest/FED_testing/FED_burnin

export PATH=$BASE_DIR/bin:$PATH
export LD_LIBRARY_PATH=$CACTUSLIB:$QTROOTSYSDIR/lib:$QTDIR/lib:$BASE_DIR/lib:${LD_LIBRARY_PATH}

# Run and respawn run binary unless it exits gracefully
cd $BASE_DIR
export DIR=$(pwd)
echo 'Basedir: ' $BASE_DIR

until $BASE_DIR/src/run $BASE_DIR/settings/FEDburnin.xml ; do
    echo "src/run crashed with exit code $?. Respawning..." >&2
    sleep 5
done
