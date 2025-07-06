#!/bin/bash

ENGINE="./Agent4k"
TEST_LOG="uci_test_output.log" # Log all engine interaction for debugging
ENGINE_INPUT="engine_input.tmp" # File to pipe commands to engine

echo "--- Running UCI Compliance Test ---"

# Ensure the engine is built before testing
make build > /dev/null 2>&1 # Suppress build output for cleaner test output

if [ ! -f "$ENGINE" ]; then
    echo "UCI Test: FAILED - Engine executable '$ENGINE' not found. Run 'make build' first."
    exit 1
fi

# Prepare input commands for the engine
cat << EOF > "$ENGINE_INPUT"
uci
isready
go
quit
EOF

# Run the engine, pipe input, capture STDOUT and STDERR, with a timeout
# The 'timeout' command prevents the test from hanging indefinitely
# Redirect stderr (2) to stdout (1), then redirect stdout to TEST_LOG
timeout 10s "$ENGINE" < "$ENGINE_INPUT" > "$TEST_LOG" 2>&1

TEST_EXIT_CODE=$?

# Check if the engine timed out or exited with an error
if [ $TEST_EXIT_CODE -eq 124 ]; then
    echo "UCI Test: FAILED - Engine timed out after 10 seconds."
    echo "--- Engine Output ---"
    cat "$TEST_LOG"
    rm "$ENGINE_INPUT" "$TEST_LOG"
    exit 1
elif [ $TEST_EXIT_CODE -ne 0 ]; then
    echo "UCI Test: FAILED - Engine exited with non-zero status ($TEST_EXIT_CODE)."
    echo "--- Engine Output ---"
    cat "$TEST_LOG"
    rm "$ENGINE_INPUT" "$TEST_LOG"
    exit 1
fi

# Verify engine responses from the captured log
echo "Verifying engine responses..."
ALL_PASSED=true

# Test 1: 'uci' command response
if grep -q "id name Agent4k" "$TEST_LOG" && \
   grep -q "id author Aider" "$TEST_LOG" && \
   grep -q "uciok" "$TEST_LOG"; then
    echo "  'uci' test: PASSED"
else
    echo "  'uci' test: FAILED - Expected 'id name Agent4k', 'id author Aider', 'uciok'."
    ALL_PASSED=false
fi

# Test 2: 'isready' command response
if grep -q "readyok" "$TEST_LOG"; then
    echo "  'isready' test: PASSED"
else
    echo "  'isready' test: FAILED - Expected 'readyok'."
    ALL_PASSED=false
fi

# Test 3: 'go' command response - check for any 'bestmove' output
if grep -q "bestmove " "$TEST_LOG"; then
    echo "  'go' test: PASSED"
else
    echo "  'go' test: FAILED - Expected a 'bestmove' output."
    ALL_PASSED=false
fi

# Final test result summary
if $ALL_PASSED; then
    echo "--- All UCI Compliance Tests PASSED ---"
    rm "$ENGINE_INPUT" "$TEST_LOG"
    exit 0
else
    echo "--- UCI Compliance Tests FAILED ---"
    # Only show full engine output for debugging if tests failed
    echo "--- Full Engine Output for Debugging ---"
    cat "$TEST_LOG"
    rm "$ENGINE_INPUT" "$TEST_LOG"
    exit 1
fi
