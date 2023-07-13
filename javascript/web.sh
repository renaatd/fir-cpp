#!/bin/sh
set -e
. "$(dirname -- "$0")/settings.sh"
echo "OUTPUT_FOLDER: ${OUTPUT_FOLDER}"
mkdir -p "${OUTPUT_FOLDER}"
cp index.html emscripten_helpers.mjs "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
echo "Open the browser and point to http://localhost:8000/"
python3 -m http.server