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
export BASE_DIR=/home/fectest/FED_testing/FED_burnin
#export BASE_DIR=~/PixelDAQ/FED_burnin

export PATH=$BASE_DIR/bin:$PATH
export LD_LIBRARY_PATH=$CACTUSLIB:$QTROOTSYSDIR/lib:$QTDIR/lib:$BASE_DIR/lib:${LD_LIBRARY_PATH}

# Run and respawn run binary unless it exits gracefully
cd $BASE_DIR
export DIR=$(pwd)
echo 'Basedir: ' $BASE_DIR

echo "Running burnin SW in a tmux session - please disconnect via <Ctrl-b> <d> and check the consoledump.txt file for the current running session"

#until $BASE_DIR/src/run $BASE_DIR/settings/FEDburnin.xml > $BASE_DIR/consoledump.txt ; do
#echo "src/run crashed with exit code $?. Respawning..." >&2
#echo "Restarting burnin code"
#sleep 60
#done

while true; do
    $BASE_DIR/src/run $BASE_DIR/settings/FEDburnin.xml > $BASE_DIR/consoledump.txt
    echo "src/run finished with exit code $?. Respawning..." >&2
    echo "Restarting burnin code"
    sleep 60
done
