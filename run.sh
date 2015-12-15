#!/bin/bash


until src/run; do
    echo "src/run crashed with exit code $?. Respawning..." >&2
    sleep 1
done
