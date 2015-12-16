#!/bin/bash
echo 'creating lib!'
mkdir lib
echo 'creating bin!'
mkdir bin
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
export BASE_DIR=$(pwd)

export PATH=$QTDIR/bin:$BASE_DIR/bin:$PATH
export LD_LIBRARY_PATH=/opt/cactus/lib:$QTROOTSYSDIR/lib:$QTDIR/lib:$BASE_DIR/lib:${LD_LIBRARY_PATH}
