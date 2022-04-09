#!/bin/bash
source scripts/functions.sh

# clean
print_info "Cleaning gcov files..." >>${log_file}
# if exist ${build_dir}/CMakeFiles
if [ -d ${build_dir}/CMakeFiles ]; then
    old_files=$(find ${build_dir}/CMakeFiles -name '*.gcda') >/dev/null
    run_no_error "rm -f ${old_files}"
fi

# run cmake
print_info "Running cmake..." >>${log_file}
run_no_error "cmake -S . -B ${build_dir}"
run_no_error "cmake --build ${build_dir}"
