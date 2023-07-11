#!/bin/sh
OUTPUT_FOLDER="../build_javascript"
mkdir -p "${OUTPUT_FOLDER}"
cp index.html "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
echo "Open the browser and point to http://localhost:8000/"
python3 -m http.server