# microlife-json

> 在 cmake 中使用 gtest，并使用 gcov+lcov 生产代码覆盖率报告。

## build

cmake 会从 github 下载 gtest 源码以编译项目，所以使用时请确保网络环境
- 使用 vscode 可以打开设置并搜索 proxy 设置为 https://127.0.0.1:port
- 终端环境可以输入 `export all_proxy="http://127.0.0.1:port"`

```shell
# 编译并执行项目
cmake -S . -B build
cmake --build build
build/my_project

# 生成代码覆盖率报告
bash lcov/code_coverage.sh

# 使用 live-server 运行一个服务器
# install -> sudo npm install live-server -g
bash lcov/server.sh
```

## 参考资料

- [nlohmann/json](https://github.com/nlohmann/json)
- [miloyip/json-tutorial](https://github.com/miloyip/json-tutorial)
- [Nomango/configor](https://github.com/Nomango/configor)
- [gtest-cmake-lcov-example](https://github.com/qingl812/gtest-cmake-lcov-example)
