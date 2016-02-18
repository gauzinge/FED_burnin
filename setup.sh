#!/bin/bash
#CACTUS
export CACTUSBIN=/opt/cactus/bin
export CACTUSLIB=/opt/cactus/lib
export CACTUSINCLUDE=/opt/cactus/include


# BOOST
export BOOST_LIB=/opt/cactus/lib
export BOOST_INCLUDE=/opt/cactus/include

#ROOT
#source /usr/local/bin/thisroot.sh

#FED Burnin
export BASE_DIR=$(pwd)

#DEPENDENCIES
export FEC_SW_DIR=/home/fectest/FEC_SW/FecSoftwareV3_0/
export AMC13DIR=/opt/cactus/include/amc13/

#PATH & LD_LIBRARY_PATH
export PATH=$BASE_DIR/bin:$PATH
export LD_LIBRARY_PATH=$CACTUSLIB:$BASE_DIR/lib:${LD_LIBRARY_PATH}
export PATH=/usr/local/bin:/usr/bin:~/bin:${CACTUSBIN}:${AMC13BIN}:$PATH
