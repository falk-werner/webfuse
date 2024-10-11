# webfuse build instructions

## Build using Docker bake

Use Docker bake to build without installing dependencies (aside of docker).

```
docker buildx bake
```

Find the build results in the `out` directory.

## Build using CMake

Please install the dependencies listed below before building using CMake.

````
cmake -B build
cmake --build build
sudo cmake --install build
````

## Build options

| Options            | Default  | Description |
| ------------------ | -------- | ----------- |
| WITHOUT_PROVIDER   | OFF      | Disables build of webfuse provider |
| WITHOUT_TEST       | OFF      | Disables build of unit and integration tests |
| WITHOUT_CLANG_TIDY | OFF      | Disables running clang tidy on build |

## Dependencies

- [libfuse](https://github.com/libfuse/libfuse)
- [libwebsockets](https://libwebsockets.org/)

### Optional dependencies

- [googletest](https://github.com/google/googletest)
- [valgrind](https://valgrind.org/)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

## Additional cmake targets

| Target   | Description |
| -------- | ----------- |
| test     | runs unit and integration tests |
| memcheck | runs unit test with valgrind/memcheck |
