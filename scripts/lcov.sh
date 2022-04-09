#!/bin/bash

source scripts/functions.sh

# init
mkdir_if_not_exist ${lcov_dir}
mkdir_if_not_exist ${lcov_dir}/info

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

# run lcov
print_info "Generate code coverage report..." >>${log_file}
lcov_run_no_error "lcov -c -d ${gcov_dir} -o ${coverage_info}"
lcov_run_no_error "lcov -r ${coverage_info} */googletest/* test/* */c++/* -o ${coverageFiltered_info}"
lcov_run_no_error "genhtml -o ${coverage_html_dir} ${coverageFiltered_info}"

# print success
print_success "Code coverage report generated successfully."
