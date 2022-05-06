#!/bin/bash

# print info
function print_info() {
    printf "[\033[33mINFO\033[0m] $*\n"
}

# print success
function print_success() {
    printf "[\033[32mSUCCESS\033[0m] $*\n"
}

# print warn
function print_warn() {
    printf "[\033[33mWARN\033[0m] $*\n"
}

# print error
function print_error() {
    printf "[\033[31mERROR\033[0m] $*\n" >&2
}

# exit with error
function exit_with_error() {
    print_error "$*"
    exit 1
}

# {command} {commmand} ...
function run_no_error() {
    "$@"

    # error
    if [ $? -ne 0 ]; then
        exit_with_error "$@"
    fi
}

# mkdir if not exist
function mkdir_if_not_exist() {
    if [ ! -d $1 ]; then
        mkdir $1
    fi
}

# get system name
function get_system_name() {
    if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        echo "linux"
    elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
        echo "windows"
    else
        exit_with_error "Unknown operating system"
    fi
}
