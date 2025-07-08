# 构建蜃楼引擎

蜃楼引擎使用 C++ 20 作为主要编程语言，请注意你使用的编译器是否支持 C++ 20 标准。

- 在 MacOS 中支持使用 XCode 提供的 clang 作为编译器。
- 在 Windows 中支持使用 MSVC 和 clang 作为编译器。

对于上面未提到的操作系统和编译器，蜃楼引擎不保证编译构建能够正常执行，但可以尝试使用其他兼容的编译器进行构建。

蜃楼引擎官方支持两种构建脚本：[`xmake`](https://xmake.io/) 和 [`cmake`](https://cmake.org/)。请确保已经安装了至少一种构建工具。

- xmake 最低版本：2.9.9
- cmake 最低版本：3.28

## 使用 VSCode

我们推荐在 VSCode 中使用 `xmake` 作为构建工具，这是因为在 Windows 操作系统中使用 MSVC 编译器执行构建时，cmake 无法正常的产生 compile_commands.json 文件，而 VSCode 使用的 clangd 插件需要这个文件来提供代码补全和跳转功能。

### .vscode 设置

使用下面的指令从 ./tools 文件夹中获取 .vscode 文件夹：

```sh
cp -r ./tools/.vscode ./
```

extension.json 文件包含了推荐的 VSCode 扩展列表，建议安装这些扩展以获得更好的开发体验。

settings.json 文件包含了对推荐插件的一些配置和一些格式化要求。

### xmake 设置

检查 xmake 版本，要求大于 2.9.9：

```sh
xmake --version
```

通过下面的配置指令可以将蜃楼引擎的各个部分拆分开，分别编译成动态库，这可以帮助你在开发过程中快速编译和调试，不再需要修改一个部分后重新编译整个引擎来执行对应的单元测试。

> 更多的配置项参考 [编译选项](#编译选项) 章节。

```sh
xmake config --kind=shared --mirage_split=true
```

如果你想查看当前的所有编译选项，使用下面的指令：

```sh
xmake config --help
xmake config --verbose
```

如果你配置正确，下载了推荐的 VSCode 扩展，并且安装了 xmake，那么此时你应该已经可以在底部边栏中看到 xmake 的图标，并且可以使用它来执行构建和调试。

在构建完成后，你可以使用下面的指令来执行所有的单元测试，用于检查项目是否正常：

```sh
xmake run -g tests
```

## 使用 CLion

我们推荐在 CLion 中使用 `cmake` 作为构建工具，这是因为 CLion 内置了对 `cmake` 的支持，可以直接读取 `CMakeLists.txt` 文件进行构建和调试。

只需要打开项目目录，CLion 会自动识别并加载 CMake 配置。

### cmake 设置

在 **构建，执行，部署 > CMake** 增加下面的 cmake 配置项可以将蜃楼引擎的各个部分拆分开，分别编译成动态库，这可以帮助你在开发过程中快速编译和调试，不再需要修改一个部分后重新编译整个引擎来执行对应的单元测试。

> 更多的配置项参考 [编译选项](#编译选项) 章节。

```
-DMIRAGE_BUILD_SHARED -DMIRAGE_BUILD_SPLIT
```

## 编译选项

| cmake 配置项            | xmake 配置项          | 描述                                 |
| ----------------------- | --------------------- | ------------------------------------ |
| `-DMIRAGE_BUILD_SHARED` | `--kind=shared`       | 将各个部分编译为动态库               |
| `-DMIRAGE_BUILD_SPLIT`  | `--mirage_split=true` | 拆分引擎的各个部分以便快速编译和调试 |
