#include <sanitizer/lsan_interface.h>
#include <emscripten/stack.h>

extern "C" int stack_get_free() {
    return (int)emscripten_stack_get_free();
}

extern "C" void leak_check() {
    // https://emscripten.org/docs/debugging/Sanitizers.html
#if defined(__has_feature)
    #if __has_feature(address_sanitizer)
        // code for ASan-enabled builds
        __lsan_do_recoverable_leak_check();
    #endif
#endif
}
