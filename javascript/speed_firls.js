// Speed test of WASM firls on a desktop PC from 2014
// 501 taps: 8.0 ms (C++ native: 3.7 ms)
// 1001 taps: 61.5 ms (C++ native: 31 ms)
function assert(condition, message) {
    if (!condition) {
        throw message || "Assertion failed";
    }
}

/**
 * Allocate an array of values on the EmScripten heap and return a pointer
 * @param {*} module  Emscripten instance
 * @param {*} data    Array containing numbers
 * @returns pointer in Emscripten heap containing the array. This must be freed after use with module._free(ptr)
 */
function createEmscriptenArrayDoubles(module, data) {
    var data_float64 = new Float64Array(data);

    var nDataBytes = data_float64.length * data_float64.BYTES_PER_ELEMENT;
    var dataPtr = module._malloc(nDataBytes);

    // Copy data to Emscripten heap (directly accessed from Module.HEAPU8)
    var dataHeap = new Uint8Array(module.HEAPU8.buffer, dataPtr, nDataBytes);
    dataHeap.set(new Uint8Array(data_float64.buffer));

    return dataPtr;
}

/**
 * Reserve room for an array of doubles on the EmScripten heap and return a pointer
 * @param {*} module  Emscripten instance
 * @param {number} length  No of doubles
 * @returns pointer in Emscripten heap containing the array. This must be freed after use with module._free(ptr)
 */
function reserveEmscriptenArrayDoubles(module, length) {
    var nDataBytes = length * Float64Array.BYTES_PER_ELEMENT;
    var dataPtr = module._malloc(nDataBytes);
    return dataPtr;
}

/**
 * Copy an array of doubles from the Emscripten heap to a newly created JavaScript Float64Array
 * @param {*} module  Emscripten instance
 * @param {number} dataPtr  Pointer in emscripten heap to the input array
 * @param {number} length  No of elements in the input array
 * @returns  Array containing the data
 */
function getEmscriptenArrayDoubles(module, dataPtr, length) {
  // inner Float64Array is a view on the heap. Copy this to a separate Float64Array
  return new Float64Array(new Float64Array(module.HEAPU8.buffer, dataPtr, length));
  // slower variant (26 µs instead of 10 µs for 1000 elements):
  // return Array.from(new Float64Array(module.HEAPU8.buffer, dataPtr, length));
}

var factory = require('../build_javascript/fir.js');
// only works in module (.mjs file):
// import factory from '../build_javascript/fir.js';

factory().then(instance => {
    // extern "C" int firls(FirFloat result[], int numTaps, int numBands, const FirFloat bands[], const FirFloat desiredBegin[], const FirFloat desiredEnd[], const FirFloat weight[], FirFloat fs);
    firls_firls = instance.cwrap(
        'firls', // name
        'number', // return value
        ['number', 'number', 'number', 'number', 'number', 'number', 'number', 'number'] // arguments
    );

    const MAXTAPS = 1010;
    const NUMBANDS = 2;
    const a = 0.1; // width of the transition band

    const bands = [0, a, 0.5 - a, 0.5];
    const desired = [1, 1, 0, 0];
    const weight = [1, 1];

    // Pass all data to Emscripten heap
    var bandsPtr = createEmscriptenArrayDoubles(instance, bands);
    var desiredPtr = createEmscriptenArrayDoubles(instance, desired);
    var weightPtr = createEmscriptenArrayDoubles(instance, weight);

    console.log("taps, total (ms), setup (ms), C++ (ms), teardown (ms), DC gain")
    for (var i = 1; i < MAXTAPS; i += 10) {
        var t0 = performance.now();
        var tapsPtr = reserveEmscriptenArrayDoubles(instance, i);

        var t1 = performance.now();
        var ret = firls_firls(tapsPtr, i, NUMBANDS, bandsPtr, desiredPtr, desiredPtr, weightPtr, 1.0);
        var t2 = performance.now();
        var taps = getEmscriptenArrayDoubles(instance, tapsPtr, i);
        instance._free(tapsPtr);
        var t3 = performance.now();

        assert(ret == 0, "firls returned error!");
        console.log(i, (t3 - t0).toFixed(3), (t1 - t0).toFixed(3), (t2 - t1).toFixed(3), (t3 - t2).toFixed(3), taps.reduce((a, b) => a + b, 0));
    }

    // Free memory
    instance._free(weightPtr);
    instance._free(desiredPtr);
    instance._free(bandsPtr);
    console.log("Doing leak check...");
    instance._leak_check();

    return instance;
}
);
