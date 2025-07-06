.PHONY: all build test run-selfplay-startpos clean run-selfplay-book

all: build

build:
	./package.sh

test: build
	./test_uci.sh

run-selfplay-startpos: build
	fastchess -engine cmd=./Agent4k name=Agent4k_1 tc=10+0.1 -engine cmd=./Agent4k name=Agent4k_2 tc=10+0.1 -pgnout file=selfplay_debug.pgn -rounds 1 -log file=fastchess_debug.log level=trace engine=true

run-selfplay-book: build
	fastchess -engine cmd=./Agent4k name=Agent4k_1 tc=10+0.1 -engine cmd=./Agent4k name=Agent4k_2 tc=10+0.1 -pgnout file=selfplay_book.pgn -rounds 20 -openings file=8moves_v3.pgn format=pgn order=random -log file=fastchess_book_debug.log level=trace engine=true

clean:
	rm -f Agent4k fastchess_debug.log selfplay_debug.pgn selfplay_book.pgn fastchess_book_debug.log config.json engine.c.xz
