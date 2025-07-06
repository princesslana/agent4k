.PHONY: all build run clean test selfplay

all: build

build:
	@echo "Packaging Agent4k..."
	./package.sh

run: build
	@echo "Running Agent4k..."
	./Agent4k

selfplay: build
	@echo "Running self-play game with Agent4k (debug enabled)..."
	fastchess -engine cmd=./Agent4k name=Agent4k_1 tc=10+0.1 -engine cmd=./Agent4k name=Agent4k_2 tc=10+0.1 -pgnout file=selfplay_debug.pgn -rounds 1 -log file=fastchess_debug.log level=trace engine=true

test: build
	@echo "Running UCI compliance tests..."
	./test_uci.sh

clean:
	@echo "Cleaning build artifacts..."
	rm -f Agent4k *.o *.tmp uci_test_output.log engine_input.tmp selfplay_debug.pgn fastchess_debug.log
