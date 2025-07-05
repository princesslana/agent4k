.PHONY: all build run clean

all: build

build:
	@echo "Packaging Agent4k..."
	./package.sh

run: build
	@echo "Running Agent4k..."
	./Agent4k

clean:
	@echo "Cleaning build artifacts..."
	rm -f Agent4k *.o *.tmp
