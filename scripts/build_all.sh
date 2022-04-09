#!/bin/bash
source scripts/functions.sh

# init
init

# cmake
run_no_error_no_log "bash scripts/cmake.sh"

# Execute the generated program
${build_dir}/${project_name}
if [ $? -ne 0 ]; then
    print_error "${project_name} return code is $?"
    exit 1
fi
printf "\n"

# lcov.sh
run_no_error_no_log "bash scripts/lcov.sh"

