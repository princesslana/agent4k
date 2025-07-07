.PHONY: all build test run-selfplay-startpos clean run-selfplay-book run-sprt-test

all: build

build:
	./package.sh

test: build
	./test_uci.sh

run-selfplay-startpos: build
	mkdir -p logs pgns
	fastchess -engine cmd=./Agent4k name=Agent4k_1 tc=10+0.1 -engine cmd=./Agent4k name=Agent4k_2 tc=10+0.1 -pgnout file=pgns/selfplay_debug.pgn -rounds 1 -log file=logs/fastchess_debug.log level=trace engine=true

run-selfplay-book: build
	mkdir -p logs pgns
	fastchess -engine cmd=./Agent4k name=Agent4k_1 tc=10+0.1 -engine cmd=./Agent4k name=Agent4k_2 tc=10+0.1 -pgnout file=pgns/selfplay_book.pgn -rounds 20 -openings file=8moves_v3.pgn format=pgn order=random -log file=logs/fastchess_book_debug.log level=trace engine=true

# SPRT Testing
# Define the baseline tag to compare against
BASELINE_TAG = v0.2

run-sprt-test: build
	@echo "--- Running SPRT Test: Current Agent4k vs $(BASELINE_TAG) ---"
	# Create directories for logs and pgns
	mkdir -p logs pgns
	# Create a temporary directory for the baseline engine
	mkdir -p ./.baseline_engine
	# Clone/checkout the baseline version into the temporary directory
	git clone . ./.baseline_engine/repo
	cd ./.baseline_engine/repo && git checkout $(BASELINE_TAG)
	# Build the baseline engine
	cd ./.baseline_engine/repo && make build > /dev/null
	# Run fastchess with SPRT parameters
	fastchess -engine cmd=./Agent4k name=Agent4k_Current tc=10+0.1 -engine cmd=./.baseline_engine/repo/Agent4k name=Agent4k_Baseline tc=10+0.1 -pgnout file=pgns/sprt_test.pgn -rounds 100 -openings file=8moves_v3.pgn format=pgn order=random -log file=logs/sprt_test.log level=info -sprt elo0=0 elo1=5 alpha=0.05 beta=0.05
	# Clean up the temporary directory
	rm -rf ./.baseline_engine
	@echo "--- SPRT Test Complete ---"
	@echo "Check logs/sprt_test.log and pgns/sprt_test.pgn for results."

clean:
	rm -f Agent4k config.json engine.c.xz
	rm -rf logs/ pgns/
