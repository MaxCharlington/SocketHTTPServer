# C++ Socket HTTP Server

This project is HTTP server implementation in C++ using latest standart (some early C++23 features are in use) and latest compiler.
The repo started as a fork of C socket server implementation which you can find [Pico HTTP Server in C](https://gist.github.com/laobubu/d6d0e9beb934b60b2e552c2d03e1409e) but it was completely rewritten to make use of latest C++.

## Building

To build the project in current version you need **g++-11** compiler and **CMake** with 3.0 version or higher. Also you are required to have **Make** on your machine. The build process is validated on **Ubuntu20.04 LTS** but it is expected to be easily built on any **Debian** based distribution.

Steps to build the project:
1. Go to the root repo folder.
2. Create and go to a build directory with `mkdir ./build && cd ./build`.
3. To build type `cmake .. && cmake --build .`.
4. If you need to rebuild just use `cmake --build .` part.

## Samples

There are some samples you can tinker with. They are located at **./samples** folder and after building you can run them from **./build/samples** folder.

## Contribution

You are welcome to contribute to this project by taking an issue and resolving the problem.
To begin with:
* Contact repo owner for an access to the repository.
* Take an issue of interest (if there is no issues you easily can create one).
* After creating a solution make a pull request and wait for comments of community or repo owner.
* Make fixes according to review or get help of the community.
* After you have repo owner approval the request will be merged.
