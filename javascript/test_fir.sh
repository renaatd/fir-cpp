#!/bin/sh
set -e
. "$(dirname -- "$0")/settings.sh"

mkdir -p "${OUTPUT_FOLDER}"
cp emscripten_helpers.mjs test_fir.mjs "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
node ./test_fir.mjs
