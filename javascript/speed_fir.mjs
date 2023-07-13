// Speed test of WASM firls on a desktop PC from 2014
// 501 taps: 8.0 ms (C++ native: 3.7 ms)
// 1001 taps: 61.5 ms (C++ native: 31 ms)
// var factory = require('../build_javascript/fir.js');
// only works in module (.mjs file):
import factory from './fir.mjs';
import { createEmscriptenArrayDoubles, reserveEmscriptenArrayDoubles, getEmscriptenArrayDoubles } from './emscripten_helpers.mjs'

function assert(condition, message) {
    if (!condition) {
        throw message || "Assertion failed";
    }
}

factory().then(instance => {
    // extern "C" int firls(FirFloat result[], int numTaps, int numBands, const FirFloat bands[], const FirFloat desiredBegin[], const FirFloat desiredEnd[], const FirFloat weight[], FirFloat fs);
    const firls_firls = instance.cwrap(
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

    console.log("taps, total (ms), setup (ms), C++ firls (ms), teardown (ms), DC gain")
    for (var i = 1; i < MAXTAPS; i += 100) {
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
