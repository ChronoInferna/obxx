# obxx [![Actions Status](https://github.com/chronoinferna/obxx/workflows/MacOS/badge.svg)](https://github.com/chronoinferna/obxx/actions) [![Actions Status](https://github.com/chronoinferna/obxx/workflows/Windows/badge.svg)](https://github.com/chronoinferna/obxx/actions) [![Actions Status](https://github.com/chronoinferna/obxx/workflows/Ubuntu/badge.svg)](https://github.com/chronoinferna/obxx/actions) [![codecov](https://codecov.io/github/ChronoInferna/obxx/graph/badge.svg?token=YY72MZYE4C)](https://codecov.io/github/ChronoInferna/obxx) [![GitHub release (latest by date)](https://img.shields.io/github/v/release/chronoinferna/obxx)](https://github.com/chronoinferna/obxx/releases)

## Features

- TODO

## Getting Started

These instructions will get you a copy of the project up and running on your local
machine for development and testing purposes.

### Prerequisites

This project is meant to be only a template, thus versions of the software used
can be change to better suit the needs of the developer(s). If you wish to use the
template _as-is_, meaning using the versions recommended here, then you will need:

- **CMake v3.28+**
- **C++23**
- **uv**

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

To build the project, all you need to do is run:

```bash
./scripts/setup.sh
cmake --preset conan-release
cmake --build --preset conan-release
```

More options that you can set for the project can be found in the
[`cmake/StandardSettings.cmake` file](cmake/StandardSettings.cmake). For certain
options additional configuration may be needed in their respective `*.cmake` files

> **_Note:_** \*Windows might break everything... sorry!\*

## Running the tests

```bash
cd build/Release          # if not in the build directory already
ctest
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
