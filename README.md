# WPILib Project

![CI](https://github.com/wpilibsuite/allwpilib/workflows/CI/badge.svg)
[![C++ Documentation](https://img.shields.io/badge/documentation-c%2B%2B-blue)](https://first.wpi.edu/wpilib/allwpilib/docs/development/cpp/)
[![Java Documentation](https://img.shields.io/badge/documentation-java-orange)](https://first.wpi.edu/wpilib/allwpilib/docs/development/java/)

Welcome to the WPILib project. This repository contains the HAL, WPILibJ, and WPILibC projects. These are the core libraries for creating robot programs for the roboRIO.

- [WPILib Project](#wpilib-project)
  - [WPILib Mission](#wpilib-mission)
- [Building WPILib](#building-wpilib)
  - [Requirements](#requirements)
  - [Setup](#setup)
  - [Building](#building)
    - [Faster builds](#faster-builds)
    - [Using Development Builds](#using-development-builds)
    - [Custom toolchain location](#custom-toolchain-location)
    - [Gazebo simulation](#gazebo-simulation)
    - [Formatting/linting with wpiformat](#formattinglinting-with-wpiformat)
    - [CMake](#cmake)
  - [Publishing](#publishing)
  - [Structure and Organization](#structure-and-organization)
- [Contributing to WPILib](#contributing-to-wpilib)

## WPILib Mission

The WPILib Mission is to enable FIRST Robotics teams to focus on writing game-specific software rather than focusing on hardware details - "raise the floor, don't lower the ceiling". We work to enable teams with limited programming knowledge and/or mentor experience to be as successful as possible, while not hampering the abilities of teams with more advanced programming capabilities. We support Kit of Parts control system components directly in the library. We also strive to keep parity between major features of each language (Java, C++, and NI's LabVIEW), so that teams aren't at a disadvantage for choosing a specific programming language. WPILib is an open source project, licensed under the BSD 3-clause license. You can find a copy of the license [here](LICENSE.md).

# Quick Start

Below is a list of instructions that guide you through cloning, building, publishing and using local allwpilib binaries in a robot project. This quick start is not intended as a replacement for the information further listed in this document.

1. Clone the repository with `git clone https://github.com/wpilibsuite/allwpilib.git`
2. Build the repository with `./gradlew build` or `./gradlew build --build-cache` if you have an internet connection
3. Publish the artifacts locally by running `./gradlew publish`
4. [Update your](OtherVersions.md) `build.gradle` [to use the artifacts](OtherVersions.md)

# Building WPILib

Using Gradle makes building WPILib very straightforward. It only has a few dependencies on outside tools, such as the ARM cross compiler for creating roboRIO binaries.

## Requirements

- [JDK 11](https://adoptopenjdk.net/)
    - Note that the JRE is insufficient; the full JDK is required
    - On Ubuntu, run `sudo apt install openjdk-11-jdk`
    - On Windows, install the JDK 11 .msi from the link above
    - On macOS, install the JDK 11 .pkg from the link above
- C++ compiler
    - On Linux, install GCC 8 or greater
    - On Windows, install [Visual Studio Community 2022 or 2019](https://visualstudio.microsoft.com/vs/community/) and select the C++ programming language during installation (Gradle can't use the build tools for Visual Studio)
    - On macOS, install the Xcode command-line build tools via `xcode-select --install`
- ARM compiler toolchain
    - Run `./gradlew installRoboRioToolchain` after cloning this repository
    - If the WPILib installer was used, this toolchain is already installed
- Raspberry Pi toolchain (optional)
    - Run `./gradlew installArm32Toolchain` after cloning this repository

## Setup

Clone the WPILib repository and follow the instructions above for installing any required tooling.

See the [styleguide README](https://github.com/wpilibsuite/styleguide/blob/main/README.md) for wpiformat setup instructions.

## Building

All build steps are executed using the Gradle wrapper, `gradlew`. Each target that Gradle can build is referred to as a task. The most common Gradle task to use is `build`. This will build all the outputs created by WPILib. To run, open a console and cd into the cloned WPILib directory. Then:

```bash
./gradlew build
```

To build a specific subproject, such as WPILibC, you must access the subproject and run the build task only on that project. Accessing a subproject in Gradle is quite easy. Simply use `:subproject_name:task_name` with the Gradle wrapper. For example, building just WPILibC:

```bash
./gradlew :wpilibc:build
```

The gradlew wrapper only exists in the root of the main project, so be sure to run all commands from there. All of the subprojects have build tasks that can be run. Gradle automatically determines and rebuilds dependencies, so if you make a change in the HAL and then run `./gradlew :wpilibc:build`, the HAL will be rebuilt, then WPILibC.

There are a few tasks other than `build` available. To see them, run the meta-task `tasks`. This will print a list of all available tasks, with a description of each task.

If opening from a fresh clone, generated java dependencies will not exist. Most IDEs will not run the generation tasks, which will cause lots of IDE errors. Manually run `./gradlew compileJava` from a terminal to run all the compile tasks, and then refresh your IDE's configuration (In VS Code open settings.gradle and save).

### Faster builds

`./gradlew build` builds _everything_, which includes debug and release builds for desktop and all installed cross compilers. Many developers don't need or want to build all of this. Therefore, common tasks have shortcuts to only build necessary components for common development and testing tasks.

`./gradlew testDesktopCpp` and `./gradlew testDesktopJava` will build and run the tests for `wpilibc` and `wpilibj` respectively. They will only build the minimum components required to run the tests.

`testDesktopCpp` and `testDesktopJava` tasks also exist for the projects `wpiutil`, `ntcore`, `cscore`, `hal` `wpilibNewCommands` and `cameraserver`. These can be ran with `./gradlew :projectName:task`.

`./gradlew buildDesktopCpp` and `./gradlew buildDesktopJava` will compile `wpilibcExamples` and `wpilibjExamples` respectively. The results can't be ran, but they can compile.

### Build Cache

Run with `--build-cache` on the command-line to use the shared [build cache](https://docs.gradle.org/current/userguide/build_cache.html) artifacts generated by the continuous integration server. Example:

```bash
./gradlew build --build-cache
```

### Using Development Builds

Please read the documentation available [here](OtherVersions.md)

### Custom toolchain location

If you have installed the FRC Toolchain to a directory other than the default, or if the Toolchain location is not on your System PATH, you can pass the `toolChainPath` property to specify where it is located. Example:

```bash
./gradlew build -PtoolChainPath=some/path/to/frc/toolchain/bin
```

### Gazebo simulation

If you also want to force building Gazebo simulation support, add -PforceGazebo. This requires gazebo_transport. We have tested on 14.04 and 15.05, but any correct install of Gazebo should work, even on Windows if you build Gazebo from source. Correct means CMake needs to be able to find gazebo-config.cmake. See [The Gazebo website](https://gazebosim.org/) for installation instructions.

```bash
./gradlew build -PforceGazebo
```

If you prefer to use CMake directly, the you can still do so.
The common CMake tasks are wpilibcSim, frc_gazebo_plugins, and gz_msgs

```bash
mkdir build #run this in the root of allwpilib
cd build
cmake ..
make
```


### Formatting/linting

#### wpiformat

wpiformat can be executed anywhere in the repository via `py -3 -m wpiformat` on Windows or `python3 -m wpiformat` on other platforms.

#### Java Code Quality Tools

The Java code quality tools Checkstyle, PMD, and Spotless can be run via `./gradlew javaFormat`. SpotBugs can be run via the `spotbugsMain`, `spotbugsTest`, and `spotbugsDev` tasks. These tools will all be run automatically by the `build` task. To disable this behavior, pass the `-PskipJavaFormat` flag.

If you only want to run the Java autoformatter, run `./gradlew spotlessApply`.

### CMake

CMake is also supported for building. See [README-CMAKE.md](README-CMAKE.md).

## Publishing

If you are building to test with other dependencies or just want to export the build as a Maven-style dependency, simply run the `publish` task. This task will publish all available packages to ~/releases/maven/development. If you need to publish the project to a different repo, you can specify it with `-Prepo=repo_name`. Valid options are:

- development - The default repo.
- beta - Publishes to ~/releases/maven/beta.
- stable - Publishes to ~/releases/maven/stable.
- release - Publishes to ~/releases/maven/release.

The maven artifacts are described in [MavenArtifacts.md](MavenArtifacts.md)

## Structure and Organization

The main WPILib code you're probably looking for is in WPILibJ and WPILibC. Those directories are split into shared, sim, and athena. Athena contains the WPILib code meant to run on your roboRIO. Sim is WPILib code meant to run on your computer with Gazebo, and shared is code shared between the two. Shared code must be platform-independent, since it will be compiled with both the ARM cross-compiler and whatever desktop compiler you are using (g++, msvc, etc...).

The Simulation directory contains extra simulation tools and libraries, such as gz_msgs and JavaGazebo. See sub-directories for more information.

The integration test directories for C++ and Java contain test code that runs on our test-system. When you submit code for review, it is tested by those programs. If you add new functionality you should make sure to write tests for it so we don't break it in the future.

The hal directory contains more C++ code meant to run on the roboRIO. HAL is an acronym for "Hardware Abstraction Layer", and it interfaces with the NI Libraries. The NI Libraries contain the low-level code for controlling devices on your robot. The NI Libraries are found in the [ni-libraries](https://github.com/wpilibsuite/ni-libraries) project.

The upstream_utils directory contains scripts for updating copies of thirdparty code in the repository.

The [styleguide repository](https://github.com/wpilibsuite/styleguide) contains our style guides for C++ and Java code. Anything submitted to the WPILib project needs to follow the code style guides outlined in there. For details about the style, please see the contributors document [here](CONTRIBUTING.md#coding-guidelines).

# Contributing to WPILib

See [CONTRIBUTING.md](CONTRIBUTING.md).
