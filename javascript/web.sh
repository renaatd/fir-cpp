#!/bin/sh
set -e
. "$(dirname -- "$0")/settings.sh"

mkdir -p "${OUTPUT_FOLDER}"
cp emscripten_helpers.mjs index.html "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
echo "Open the browser and point to http://localhost:8000/"
python3 -m http.server