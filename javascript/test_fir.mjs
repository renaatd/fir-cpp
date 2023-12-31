import factory from './fir.mjs';
import { createEmscriptenArrayDoubles, reserveEmscriptenArrayDoubles, getEmscriptenArrayDoubles } from './emscripten_helpers.mjs'

let cwrap_freqz;
let cwrap_firls;
let cwrap_firerror;
let cwrap_get_stack_free;

function assert(condition, message) {
  if (!condition) {
    throw message || "Assertion failed";
  }
}

function freqz(module, noFrequencies, taps, fs) {
  // Pass all data to Emscripten heap and reserve memory for results
  const t0 = performance.now();
  const tapsPtr = createEmscriptenArrayDoubles(module, taps);
  const frequenciesPtr = reserveEmscriptenArrayDoubles(module, noFrequencies);
  const magnitudesPtr = reserveEmscriptenArrayDoubles(module, noFrequencies);

  // Call function and get result
  const t0bis = performance.now();
  const ret = cwrap_freqz(frequenciesPtr, magnitudesPtr, noFrequencies, taps.length, tapsPtr, fs);
  const frequencies = getEmscriptenArrayDoubles(module, frequenciesPtr, noFrequencies);
  const magnitudes = getEmscriptenArrayDoubles(module, magnitudesPtr, noFrequencies);
  const t1bis = performance.now();

  // Free memory
  module._free(tapsPtr);
  module._free(frequenciesPtr);
  module._free(magnitudesPtr);
  const t1 = performance.now();

  console.log("Full call took " + (t1 - t0) + " milliseconds");
  console.log("setup took " + (t0bis - t0) + " milliseconds");
  console.log("C++ and copy took " + (t1bis - t0bis) + " milliseconds");
  console.log("free took " + (t1 - t1bis) + " milliseconds");
  return [ret, frequencies, magnitudes];
}

factory().then(instance => {
  // save all the wrappers for later reuse

  // extern "C" int freqz(FirFloat frequencies[], FirFloat magnitudes[], int n, int numTaps, const FirFloat taps[], FirFloat fs);
  cwrap_freqz = instance.cwrap(
    'firfreqz', // name
    'number', // return value
    ['number', 'number', 'number', 'number', 'number', 'number'] // arguments
  );

  // extern "C" int firls(FirFloat result[], int numTaps, int numBands, const FirFloat bands[], const FirFloat desiredBegin[], const FirFloat desiredEnd[], const FirFloat weight[], FirFloat fs);
  cwrap_firls = instance.cwrap(
    'firls', // name
    'number', // return value
    ['number', 'number', 'number', 'number', 'number', 'number', 'number', 'number'] // arguments
  );

  cwrap_firerror = instance.cwrap(
    'firerror', // name
    'string', // return value
    ['number'] // arguments
  );

  cwrap_get_stack_free = instance.cwrap(
    'stack_get_free', // name
    'number', // return value
    [] // arguments
  );

  return instance;
}).then(instance => {
  console.log('\nTesting stack_free');
  console.log("Free stack size: ", cwrap_get_stack_free(), " bytes.");

  return instance;
}).then(instance => {
  console.log('Testing firfreqz');

  // Create example taps
  const taps = [1, 1];
  const NO_FREQS = 11;

  const [ret, frequencies, magnitudes] = freqz(instance, NO_FREQS, taps, 2.0);
  console.log('Return value', ret);
  console.log('Frequencies: ', frequencies);
  console.log('Magnitudes: ', magnitudes);

  console.log("Doing leak check...");
  instance._leak_check();

  return instance;
}).then(instance => {
  /*
   * firfreqz needs 32 bytes/frequency on the stack + some margin, and the
   * default stack is only 64 KB -> code only works if compiled with large
   * stack, otherwise an out of bounds excpetion is thrown or address sanitizer
   * crashes the application
   */
  const NO_FREQS = 5001;
  console.log(`Testing firfreqz with ${NO_FREQS} frequencies`);

  // Create example taps
  const taps = [1, 1];

  const [ret, frequencies, magnitudes] = freqz(instance, NO_FREQS, taps, 2.0);

  console.log("Doing leak check...");
  instance._leak_check();

  return instance;
}).then(instance => {
  console.log('\nTesting firls');

  // compare to MATLAB output
  const NUMTAPS = 11;
  const NUMBANDS = 2;

  const bands = [0, 0.5, 0.5, 1];
  const desired = [1, 0];
  const weight = [1, 2];

  const expected_taps = [0.058545300496815, -0.014233383714318, -0.104688258464392,
    0.012403323025279, 0.317930861136062, 0.488047220029700,
    0.317930861136062, 0.012403323025279, -0.104688258464392,
    -0.014233383714318, 0.058545300496815];

  // Pass all data to Emscripten heap and reserve memory for results
  const bandsPtr = createEmscriptenArrayDoubles(instance, bands);
  const desiredPtr = createEmscriptenArrayDoubles(instance, desired);
  const weightPtr = createEmscriptenArrayDoubles(instance, weight);
  const tapsPtr = reserveEmscriptenArrayDoubles(instance, NUMTAPS);

  const ret = cwrap_firls(tapsPtr, NUMTAPS, NUMBANDS, bandsPtr, desiredPtr, desiredPtr, weightPtr, 2.0);
  const taps = getEmscriptenArrayDoubles(instance, tapsPtr, NUMTAPS);

  // Prove data is copied outside the heap by overwriting the heap
  for (let i = 0; i < NUMTAPS * Float64Array.BYTES_PER_ELEMENT; i++) {
    instance.HEAPU8[tapsPtr + i] = 0;
  }

  instance._free(tapsPtr);
  instance._free(weightPtr);
  instance._free(desiredPtr);
  instance._free(bandsPtr);

  assert(ret == 0, "firls returned error!");
  for (let i = 0; i < NUMTAPS; i++) {
    console.log('taps ', i, ': ', taps[i]);
    assert(Math.abs(taps[i] - expected_taps[i]) < 1e-8, "taps outside tolerance!");
  }

  console.log("Doing leak check...");
  instance._leak_check();

  return instance
}).then(instance => {
  console.log('\nTesting firerror');

  console.log("Index -1, expecting out of range error: ", cwrap_firerror(-1));
  console.log("Index 0, expecting OK: ", cwrap_firerror(0));
  console.log('Indexes 1..9, expecting different error messages, last ones out of range');
  for (let i = 1; i < 10; i++) {
    console.log(i, ": ", cwrap_firerror(i));
  }

  console.log("Doing leak check...");
  instance._leak_check();

  return instance;
}).then(instance => {
  console.log('\nTesting stack_free again');
  console.log("Free stack size: ", cwrap_get_stack_free(), " bytes.");

  return instance;
});
