[![Actions Status](https://github.com/chronoinferna/obxx/workflows/MacOS/badge.svg)](https://github.com/chronoinferna/obxx/actions)
[![Actions Status](https://github.com/chronoinferna/obxx/workflows/Windows/badge.svg)](https://github.com/chronoinferna/obxx/actions)
[![Actions Status](https://github.com/chronoinferna/obxx/workflows/Ubuntu/badge.svg)](https://github.com/chronoinferna/obxx/actions)
[![codecov](https://codecov.io/gh/chronoinferna/obxx/branch/master/graph/badge.svg)](https://codecov.io/gh/chronoinferna/obxx)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/chronoinferna/obxx)](https://github.com/chronoinferna/obxx/releases)

# obxx

## Features

- Modern **CMake** configuration and project, which, to the best of my
  knowledge, uses the best practices,

- An example of a **Clang-Format** config, inspired from the base _Google_ model,
  with minor tweaks. This is aimed only as a starting point, as coding style
  is a subjective matter, everyone is free to either delete it (for the _LLVM_
  default) or supply their own alternative,

- **Static analyzers** integration, with _Clang-Tidy_ and _Cppcheck_, the former
  being the default option,

- **Doxygen** support, through the `ENABLE_DOXYGEN` option, which you can enable
  if you wish to use it,

- **Unit testing** support, through _GoogleTest_ (with an option to enable
  _GoogleMock_) or _Catch2_,

- **Code coverage**, enabled by using the `ENABLE_CODE_COVERAGE` option, through
  _Codecov_ CI integration,

- **Package manager support**, with _Conan_ and _Vcpkg_, through their respective
  options

- **CI workflows for Windows, Linux and MacOS** using _GitHub Actions_, making
  use of the caching features, to ensure minimum run time,

- **.md templates** for: _README_, _Contributing Guideliness_,
  _Issues_ and _Pull Requests_,

- **Permissive license** to allow you to integrate it as easily as possible. The
  template is licensed under the [Unlicense](https://unlicense.org/),

- Options to build as a header-only library or executable, not just a static or
  shared library.

- **Ccache** integration, for speeding up rebuild times

## Getting Started

These instructions will get you a copy of the project up and running on your local
machine for development and testing purposes.

### Prerequisites

This project is meant to be only a template, thus versions of the software used
can be change to better suit the needs of the developer(s). If you wish to use the
template _as-is_, meaning using the versions recommended here, then you will need:

- **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)

- **C++ Compiler** - needs to support at least the **C++23** standard
  _GCC_, _Clang_

> **_Note:_** \*You also need to be able to provide **_CMake_** a supported
> [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).\*

### Installing

It is fairly easy to install the project, all you need to do is clone if from
[GitHub](https://github.com/chronoinferna/obxx) or
[generate a new repository from it](https://github.com/chronoinferna/obxx/generate)
(also on **GitHub**).

If you wish to clone the repository, rather than generate from it, you simply need
to run:

```bash
git clone https://github.com/chronoinferna/obxx/
```

## Building the project

To build the project, all you need to do, **_after correctly
[installing the project](README.md#Installing)_**, is run a similar **CMake** routine
to the the one below:

```bash
mkdir build/ && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/absolute/path/to/custom/install/directory
cmake --build . --target install --config <configuration>
```

> **_Note:_** _The custom `CMAKE_INSTALL_PREFIX` can be omitted if you wish to
> install in [the default install location](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)._

More options that you can set for the project can be found in the
[`cmake/StandardSettings.cmake` file](cmake/StandardSettings.cmake). For certain
options additional configuration may be needed in their respective `*.cmake` files
(i.e. Conan needs the `CONAN_REQUIRES` and might need the `CONAN_OPTIONS` to be setup
for it work correctly; the two are set in the [`cmake/Conan.cmake` file](cmake/Conan.cmake)).

## Generating the documentation

In order to generate documentation for the project, you need to configure the build
to use Doxygen. This is easily done, by modifying the workflow shown above as follows:

```bash
mkdir build/ && cd build/
cmake .. -D<project_name>_ENABLE_DOXYGEN=1 -DCMAKE_INSTALL_PREFIX=/absolute/path/to/custom/install/directory
cmake --build . --target doxygen-docs
```

> **_Note:_** _This will generate a `docs/` directory in the **project's root directory**._

## Running the tests

By default, the template uses [Google Test](https://github.com/google/googletest/)
for unit testing. Unit testing can be disabled in the options, by setting the
`ENABLE_UNIT_TESTING` (from
[cmake/StandardSettings.cmake](cmake/StandardSettings.cmake)) to be false. To run
the tests, simply use CTest, from the build directory, passing the desire
configuration for which to run tests for. An example of this procedure is:

```bash
cd build          # if not in the build directory already
ctest -C Release  # or `ctest -C Debug` or any other configuration you wish to test

# you can also run tests with the `-VV` flag for a more verbose output (i.e.
#GoogleTest output as well)
```

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our how you can
become a contributor and the process for submitting pull requests to us.

## Versioning

This project makes use of [SemVer](http://semver.org/) for versioning. A list of
existing versions can be found in the
[project's releases](https://github.com/chronoinferna/obxx/releases).

## Authors

- **Oliver Deng** - [@chronoinferna](https://github.com/chronoinferna)

## License

This project is licensed under the [Unlicense](https://unlicense.org/) - see the
[LICENSE](LICENSE) file for details
