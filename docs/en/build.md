# Building the Mirage Engine

The Mirage Engine uses C++ 20 as its primary programming language. Please ensure that your compiler supports the C++ 20 standard.

- On macOS, the clang compiler provided by XCode is supported.
- On Windows, both MSVC and clang compilers are supported.

For operating systems and compilers not mentioned above, the Mirage Engine does not guarantee successful compilation and build, but you can try using other compatible compilers.

The Mirage Engine officially supports two build scripts: [`xmake`](https://xmake.io/) and [`cmake`](https://cmake.org/). Please ensure that at least one of these build tools is installed.

- Minimum version for xmake: 2.9.9
- Minimum version for cmake: 3.28

## Using VSCode

We recommend using `xmake` as the build tool in VSCode. This is because, on Windows, when using the MSVC compiler, cmake cannot properly generate the `compile_commands.json` file, which is required by the clangd plugin used by VSCode for code completion and navigation.

### .vscode Settings

Use the following command to copy the `.vscode` folder from the `./tools` directory:

```sh
cp -r ./tools/.vscode ./
```

The `extension.json` file contains a list of recommended VSCode extensions. It is recommended to install these extensions for a better development experience.

The `settings.json` file contains configurations for the recommended plugins and some formatting requirements.

### xmake Settings

Check the xmake version, which must be greater than 2.9.9:

```sh
xmake --version
```

Use the following configuration command to split the various parts of the Mirage Engine into separate dynamic libraries. This can help you quickly compile and debug during development without needing to recompile the entire engine after modifying one part to run the corresponding unit tests.

> For more configuration options, refer to the [Compilation Options](#Compilation-Options) section.

```sh
xmake config --kind=shared --mirage_split=true
```

If you want to view all current compilation options, use the following commands:

```sh
xmake config --help
xmake config --verbose
```

If you have configured everything correctly, downloaded the recommended VSCode extensions, and installed xmake, you should now see the xmake icon in the bottom sidebar and be able to use it for building and debugging.

After the build is complete, you can use the following command to run all unit tests to check if the project is functioning correctly:

```sh
xmake run -g tests
```

## Using CLion

We recommend using `cmake` as the build tool in CLion. This is because CLion has built-in support for `cmake` and can directly read `CMakeLists.txt` files for building and debugging.

Simply open the project directory, and CLion will automatically recognize and load the CMake configuration.

### cmake Settings

In **Build, Execution, Deployment > CMake**, add the following cmake configuration options to split the various parts of the Mirage Engine into separate dynamic libraries. This can help you quickly compile and debug during development without needing to recompile the entire engine after modifying one part to run the corresponding unit tests.

> For more configuration options, refer to the [Compilation Options](#Compilation-Options) section.

```
-DMIRAGE_BUILD_SHARED -DMIRAGE_BUILD_SPLIT
```

## Compilation Options

| cmake Option            | xmake Option          | Description                                                      |
| ----------------------- | --------------------- | ---------------------------------------------------------------- |
| `-DMIRAGE_BUILD_SHARED` | `--kind=shared`       | Compile each part as a dynamic library                           |
| `-DMIRAGE_BUILD_SPLIT`  | `--mirage_split=true` | Split the engine into parts for faster compilation and debugging |
