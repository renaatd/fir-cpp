#!/bin/sh
set -e
. "$(dirname -- "$0")/settings.sh"

mkdir -p "${OUTPUT_FOLDER}"
cp emscripten_helpers.mjs speed_fir.mjs "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
echo "Testing firls speed"
node ./speed_fir.mjs
