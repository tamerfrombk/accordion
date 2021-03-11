# accordion

`accordion` is a URL shortener that works in a similar fashion to `TinyURL`.
### Dependencies

* [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/)
* [CMake](https://cmake.org/)

### Building

Once the dependencies above are installed on your system, run the following in the root of the source tree:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=release ..
make
```

This will build the `accordion` binary in the `build` directory.

### Running

TODO