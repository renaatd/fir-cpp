# JavaScript build fir-cpp
## Interface C++/JavaScript
The C++ code is compiled unmodified. All interface logic to pass parameters from JavaScript to C++ and back is implemented at the JavaScript side.

`emscripten_helpers.mjs` provides helper functions to put a pure JavaScript array on the Emscripten heap, and to copy an array of doubles from the Emscripten heap back to pure JavaScript.
See the function `freqz` in the file `test_fir.mjs` for an example on how to pass parameters.

The C++ function `firfreqz` makes heavily use of the stack. It needs approx 32 bytes/element in the output array. The default Emscripten data stack is only 64 kB.
To avoid a stack overflow, the code is compiled with a larger stack size.

## Build instructions
Emscripten `em++` must be in the path. The library Eigen3 must be registered in `pkg-config`. Change `settings.sh` to change the output folder.
```
# Debug build
./build_debug.sh
# Release build
./build_release.sh
```

## Test in node
```
./speed_fir.sh
./test_fir.sh
```

## Test in the browser
```
./web.sh
```
