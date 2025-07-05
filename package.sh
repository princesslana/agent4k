#!/bin/bash

ENGINE_NAME="Agent4k"
WRAPPER_SCRIPT="run.sh"
ENGINE_SOURCE="engine.c"
FINAL_EXECUTABLE="${ENGINE_NAME}"
MAX_SIZE_BYTES=4096

echo "--- Packaging ${ENGINE_NAME} ---"

# Get the exact size of the wrapper script
WRAPPER_SIZE=$(stat -c %s "${WRAPPER_SCRIPT}")
echo "DEBUG: Size of ${WRAPPER_SCRIPT} is ${WRAPPER_SIZE} bytes."
# The offset for tail -c +N should be WRAPPER_SIZE + 1

# 1. Minify C source (remove comments and excessive whitespace)
# This is a basic minification. More aggressive tools could be used later.
echo "Minifying ${ENGINE_SOURCE}..."

# 2. Compress the C source code
echo "Compressing ${ENGINE_SOURCE} with xz..."
xz -zc "${ENGINE_SOURCE}" > "${ENGINE_SOURCE}.xz"

# 3. Combine wrapper script and compressed source
echo "Combining wrapper and compressed source into ${FINAL_EXECUTABLE}..."
cat "${WRAPPER_SCRIPT}" "${ENGINE_SOURCE}.xz" > "${FINAL_EXECUTABLE}"

# 4. Make the final executable runnable
echo "Making ${FINAL_EXECUTABLE} executable..."
chmod +x "${FINAL_EXECUTABLE}"

# 5. Verify size
echo "Verifying size of ${FINAL_EXECUTABLE}..."
FILE_SIZE=$(stat -c %s "${FINAL_EXECUTABLE}")
echo "Current size: ${FILE_SIZE} bytes"

if (( FILE_SIZE <= MAX_SIZE_BYTES )); then
    echo "SUCCESS: ${FINAL_EXECUTABLE} is within the ${MAX_SIZE_BYTES} byte limit."
else
    echo "FAILURE: ${FINAL_EXECUTABLE} EXCEEDS the ${MAX_SIZE_BYTES} byte limit by $((FILE_SIZE - MAX_SIZE_BYTES)) bytes!"
    echo "You need to reduce the size of ${WRAPPER_SCRIPT} or ${ENGINE_SOURCE}."
fi

# 6. Clean up temporary compressed file
echo "Cleaning up temporary files..."
rm "${ENGINE_SOURCE}.xz"

echo "--- Packaging Complete ---"
echo "To test, run: ./${FINAL_EXECUTABLE}"
echo "Then type UCI commands like 'uci', 'isready', 'go', 'quit'."
