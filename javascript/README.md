# JavaScript build fir-cpp
## build instructions
Emscripten em++ must be in the path. The library Eigen must be registered in `pkg-config`. Change `settings.sh` to change the output folder.

```
# Debug build
./build_debug.sh
# Release build
./build_release.sh
# Test in node
./speed_fir.sh
./test_fir.sh
# Test in the browser
./web.sh
```