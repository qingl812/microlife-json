#!/bin/bash
source scripts/functions.sh

# need 3 argu
if [ $# -ne 3 ]; then
    exit_with_error "Usage: lcov.sh {build_dir} {lcov_dir} {log_file}"
else
    build_dir=$1
    lcov_dir=$2
    log_file=$3
fi

# init
mkdir_if_not_exist ${lcov_dir}
mkdir_if_not_exist ${lcov_dir}/info

gcov_dir="${build_dir}/test/CMakeFiles"
coverage_info=${lcov_dir}/info/coverage.info
coverageFiltered_info=${lcov_dir}/info/coverageFiltered.info
coverage_html_dir=${lcov_dir}/html

function lcov_run_no_error() {
    $1 >>${log_file}
    # exit with error
    if [ $? -ne 0 ]; then
        print_error "Generate code coverage report failed. For more information see ${log_file}"
        exit 1
    fi
}

# has lcov
lcov_version=$(lcov --version 2>>${log_file})
if [ $? -ne 0 ]; then
    print_error "lcov not found. Please install lcov."
    exit 1
fi

# run lcov
print_info "Generate code coverage report..." >>${log_file}
lcov_run_no_error "lcov -c -d ${gcov_dir} -o ${coverage_info}"
lcov_run_no_error "lcov -r ${coverage_info} /*3rdparty/* */googletest/* test/* */c++/* -o ${coverageFiltered_info}"
lcov_run_no_error "genhtml -o ${coverage_html_dir} ${coverageFiltered_info}"

# print success
print_success "Code coverage report generated successfully."
