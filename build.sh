#!/bin/bash
source scripts/functions.sh

# exit if error
set -e

# clean
if [ "$1" == "clean" ]; then
    clean
    exit 0
fi

# build all
run_no_error_no_log "bash scripts/build_all.sh"
