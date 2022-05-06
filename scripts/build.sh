#!/bin/bash
source scripts/functions.sh

# init
function init() {
    project_name=$1
    build_dir=$2
    log_file=$3
    cxx_compiler=$4

    # if linux and cxx_compiler==msvc, use gcc
    system=$(get_system_name)
    if [ ${system} == "linux" ] && [ "$cxx_compiler" == "msvc" ]; then
        cxx_compiler="gcc"
    fi

    # mkdir if not exist
    mkdir_if_not_exist ${build_dir}

    # remove old log file
    if [ -f ${log_file} ]; then
        rm ${log_file}
    fi

    if [ "${cxx_compiler}" == "msvc" ]; then
        # msvc
        exe_dir="${build_dir}/bin/Debug"
    else
        # gcc
        exe_dir="${build_dir}/bin"
    fi

    system=$(get_system_name)
    if [ ${system} == "windows" ]; then
        # windows
        exe_file="${project_name}.exe"
    else
        # linux
        exe_file="${project_name}"
    fi
}

# {build_dir} {log_file}
function clean() {
    # need 2 argu
    if [ $# -ne 2 ]; then
        exit_with_error "Usage: clean {build_dir} {log_file}"
    fi

    # remove old log file
    if [ -f $2 ]; then
        rm $2
    fi

    # remove old build directory
    if [ -d $1 ]; then
        rm -rf $1
    fi
}

# {project_name} {build_dir} {log_file} {cxx_compiler}
function build() {
    # need 4 argu
    if [ $# -ne 4 ]; then
        exit_with_error "Usage: build {project_name} {build_dir} {log_file} {cxx_compiler}"
    fi

    init $1 $2 $3 $4

    # cmake
    run_no_error bash scripts/cmake.sh ${log_file} ${build_dir} ${cxx_compiler}

    # Execute the generated program
    print_info "Running ${exe_dir}/${exe_file}..."
    cur_path=$(pwd)
    cd ${exe_dir}
    ./${exe_file}
    cd ${cur_path}
    if [ $? -ne 0 ]; then
        print_error "${exe_file} return code is $?"
        exit 1
    fi
}

function code_coverage() {
    # need 3 argu
    if [ $# -ne 3 ]; then
        exit_with_error "Usage: lcov.sh {build_dir} {lcov_dir} {log_file}"
    fi

    # Currently only supports linux
    system=$(get_system_name)
    if [ ${system} == "linux" ]; then
        run_no_error bash scripts/lcov.sh $1 $2 $3
    else
        print_error "Currently only supports Linux generating code coverage report."
    fi
}
