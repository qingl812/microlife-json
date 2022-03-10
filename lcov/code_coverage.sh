#!/bin/bash

set -e

lcov="lcov"
log="lcov.log"
build="build"
html="html"

cd ${build}
# TODO:
# 如果项目下存在多个 *.dir 如何处理
file=$(find CMakeFiles -type d -name *.dir)
cd ${file}

if [ ! -d ${lcov} ]; then
  mkdir ${lcov}
fi
cd ${lcov}

echo ${build}/${file} > ${log}
# -d 项目路径，即.gcda .gcno所在的路径
# -a 合并（归并）多个lcov生成的info文件
# -c 捕获，也即收集代码运行后所产生的统计计数信息
# --external 捕获其它目录产生的统计计数文件
# -i/--initial 初始化所有的覆盖率信息，作为基准数据
# -o 生成处理后的文件
# -r/--remove 移除不需要关注的覆盖率信息文件
# -z 重置所有执行程序所产生的统计信息为0
lcov -d ../ -c -o coverage.info >> ${log}
lcov -r coverage.info */googletest/* test/* */c++/* -o coverageFiltered.info >> ${log}
genhtml -o ${html} --num-spaces 4 -s --legend coverageFiltered.info >> ${log}

cd ../../../../
rm -rf lcov/html
mv ${build}/${file}/${lcov}/${html} lcov/html
