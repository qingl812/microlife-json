#!/bin/bash

project_name="microlife-json"
build_dir="build"
log_file=${build_dir}/log.txt

gcov_dir=${build_dir}/CMakeFiles/${project_name}.dir
lcov_dir=${build_dir}/lcov

# init
function init() {
    # mkdir if not exist
    if [ ! -d ${build_dir} ]; then
        mkdir ${build_dir}
    fi

    # remove old log file
    if [ -f ${log_file} ]; then
        rm ${log_file}
    fi
}

# clean
function clean() {
    # remove old log file
    if [ -f ${log_file} ]; then
        rm ${log_file}
    fi

    # remove old build directory
    if [ -d ${build_dir} ]; then
        rm -rf ${build_dir}
    fi
}
