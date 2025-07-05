.PHONY: all build run clean test

all: build

build:
	@echo "Packaging Agent4k..."
	./package.sh

run: build
	@echo "Running Agent4k..."
	./Agent4k

test: build
	@echo "Running UCI compliance tests..."
	./test_uci.sh

clean:
	@echo "Cleaning build artifacts..."
	rm -f Agent4k *.o *.tmp uci_test_output.log engine_input.tmp
