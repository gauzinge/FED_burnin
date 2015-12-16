#!/bin/bash

# Run and respawn run binary unless it exits gracefully
export DIR=$(pwd)
echo 'Basedir: ' $DIR

until $DIR/src/run $DIR/settings/FEDburnin.xml ; do
    echo "src/run crashed with exit code $?. Respawning..." >&2
    sleep 5
done
