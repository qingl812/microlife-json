#!/bin/bash
source scripts/functions.sh

# need 3 argu
if [ $# -ne 3 ]; then
    exit_with_error "Usage: cmake.sh {log_file} {build_dir} {cxx_compiler}"
else
    log_file=$1
    build_dir=$2
    cxx_compiler=$3
fi

# compiler_options
if [ "${cxx_compiler}" == "gcc" ]; then
    # gcc
    system=`get_system_name`
    if [ $system == "windows" ]; then
        compiler_options=(-G "MinGW Makefiles")
    fi
else
    # msvc
    compiler_options=()
fi

# run cmake
print_info Running cmake... 1>>${log_file}
run_no_error cmake -S . -B ${build_dir} "${compiler_options[@]}" 1>>${log_file}
run_no_error cmake --build ${build_dir} 1>>${log_file}
