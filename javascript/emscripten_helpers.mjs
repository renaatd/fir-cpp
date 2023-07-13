/**
 * Allocate an array of values on the EmScripten heap and return a pointer
 * @param {*} module  Emscripten instance
 * @param {*} data    Array containing numbers
 * @returns pointer in Emscripten heap containing the array. This must be freed after use with module._free(ptr)
 */
export function createEmscriptenArrayDoubles(module, data) {
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
export function reserveEmscriptenArrayDoubles(module, length) {
    var nDataBytes = length * Float64Array.BYTES_PER_ELEMENT;
    var dataPtr = module._malloc(nDataBytes);
    return dataPtr;
}

/**
 * Copy an array of doubles from the Emscripten heap to a newly created JavaScript Float64Array allocated outside the Emscripten heap
 * @param {*} module  Emscripten instance
 * @param {number} dataPtr  Pointer in emscripten heap to the input array
 * @param {number} length  No of elements in the input array
 * @returns Float64Array containing the data
 */
export function getEmscriptenArrayDoubles(module, dataPtr, length) {
    // inner Float64Array is a view on the heap. Copy this to a separate Float64Array
    return new Float64Array(new Float64Array(module.HEAPU8.buffer, dataPtr, length));
    // slower variant (26 µs instead of 10 µs for 1000 elements):
    // return Array.from(new Float64Array(module.HEAPU8.buffer, dataPtr, length));
}
