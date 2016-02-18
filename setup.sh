#!/bin/bash
if [ ! -d "lib" ]
then
    echo 'creating lib!'
    mkdir lib
fi

if [ ! -d "bin" ]
then
    echo 'creating bin!'
    mkdir bin
fi


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


#FED Burnin
export BASE_DIR=$(pwd)

export PATH=$BASE_DIR/bin:$PATH
export LD_LIBRARY_PATH=$CACTUSLIB:$BASE_DIR/lib:${LD_LIBRARY_PATH}
export PATH=/usr/bin:/usr/local/bin:~/bin:${CACTUSBIN}:${AMC13BIN}:$PATH


#GCC compiler

. /afs/cern.ch/sw/lcg/external/gcc/4.8.4/x86_64-slc6/setup.sh

#ROOT
. /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.21/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh
