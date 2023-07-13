echo "Testing firls speed"
set -e
. "$(dirname -- "$0")/settings.sh"

mkdir -p "${OUTPUT_FOLDER}"
cp emscripten_helpers.mjs speed_fir.mjs "${OUTPUT_FOLDER}"
cd "${OUTPUT_FOLDER}" || exit 1
node ./speed_fir.mjs
