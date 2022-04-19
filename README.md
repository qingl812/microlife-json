# microlife-json

- 符合标准的 JSON 解析器和生成器
- 使用 GoogleTest 进行单元测试，覆盖率达到100%
- 跨平台／编译器（Windows／Linux，msvc／gcc）

## build

```bash
bash build.sh
```

## clean

```bash
bash build.sh clean
```

## View the code coverage report

> 生成代码覆盖率使用的是 lcov

```bash
# sudo npm install live-server -g

live-server --port=5500 build/lcov/html
```
