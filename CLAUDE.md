# Agent4k Chess Engine - TCEC 4k Competition

## Project Overview
Building a chess engine to compete in TCEC 4k competition using agentic coding. Focus on AI-friendly code structure over human readability.

## TCEC 4k Rules & Requirements

### Technical Constraints
- **Size limit**: 4KiB (4096 bytes) total
- **Single file**: Must be one executable or script
- **Startup time**: Must complete within 60 seconds
- **No persistent files**: Cannot create lingering files (except system caches)

### UCI Protocol (Required)
Must support minimal UCI commands:
- `uci` - identify engine
- `isready` - ready check
- `position` - set board position
- `go` - start searching
- `bestmove` - return best move
- Must tolerate unsupported UCI commands gracefully

### Allowed Resources
- System commands: bash, python, gcc, cargo
- Libraries: libc, CUDA, OpenCL, NumPy
- Python-chess library (must fit in size limit)
- Compression allowed (UPX, self-extracting)

### Competition Format
- Initial Pool: 4x Double Round Robin (120 games)
- Time control: 30 mins + 3 sec increment per move
- Top 2 advance to MiniFinal (25x Double Round Robin, 50 games)
- Crashes = losses

## Development Strategy
- Code for AI maintainability, not human readability
- Aggressive size optimization
- Focus on core chess logic and basic search
- Consider compression for final submission

## Submission Format Key Insight
**Final submission size is what matters** - can be compressed source that gets compiled on-site. Binary size is irrelevant for the 4k limit.

This enables:
- Readable source code during development
- Aggressive compression for submission (UPX, shell scripts)
- Focus on algorithm density in source, not binary optimization
- Possible use of self-extracting compressed archives

## Revised Strategy: C++ with Compression Focus
**Language**: C++ (for performance to win)
**Submission**: Compressed source or self-extracting script
**Development**: Readable code → aggressive compression

### Size Optimization Strategy
1. **Develop readable C++** with full variable names
2. **Compress variable names** (board→b, position→p, etc.)
3. **Remove whitespace** and unnecessary characters
4. **Use compression tools** (gzip, UPX, custom)
5. **Self-extracting script** if beneficial

### Compression Potential
- Raw C++ source: ~8-12KB readable code
- Compressed: ~4KB through variable compression + gzip
- This allows much more sophisticated algorithms

## Project Structure & Development Workflow

### File Organization
```
agent4k/
├── src/
│   ├── chess.h            # Chess logic header
│   ├── chess.cpp          # Chess logic implementation
│   ├── chess_test.cpp     # Comprehensive tests (includes perft)
│   ├── main.cpp           # UCI interface main function
│   └── perftsuite.epd     # Perft test positions (126 positions)
├── books/
│   └── 8moves_v3.pgn     # Opening book
├── baselines/
│   └── engine_baseline    # Reference engine for testing
└── build/                 # Compiled binaries (gitignored)
```

### Development Principles
1. **Single source of truth**: Engine logic in `chess.cpp`, shared via `chess.h`
2. **No code duplication**: All executables link to `chess.cpp`, don't copy code
3. **Clear separation**: Engine logic, UCI interface, tests in separate files
4. **Bulletproof move generation**: Validated against 126 perft positions
5. **Minimal UCI compliance**: Only implement required commands for 4k size

## Testing & Development Workflow

### Engine Testing Strategy
**SPRT (Sequential Probability Ratio Test)** for statistically confident improvement testing:

**SPRT Parameters:**
- Early development: `elo0=0 elo1=10` (wider bounds = faster completion)
- Later validation: `elo0=0 elo1=5` (narrower bounds = more precise)
- Alpha/Beta: 0.05 (95% confidence)

**Version Management:**
1. `baseline-latest` (git tag) - Current best engine for SPRT testing
2. `main` branch - Current development version
3. Local builds - `./build/agent4k` for testing

**Testing Workflow:**
1. Make improvements to source code
2. Test new version vs baseline using SPRT
3. If SPRT passes → commit improvement and update baseline tag
4. Continue development cycle

**Opening Book:** `8moves_v3.pgn` (Stockfish standard, 8MB PGN format)

### Testing Approach & Commands

**Layered Testing Strategy:**
1. **Comprehensive Tests** - Unit tests + perft validation (126 positions) in one suite
2. **Game Tests** - Quick game play testing with fastchess
3. **SPRT Tests** - Statistical validation of engine strength improvements

**Testing Philosophy:**
- **Correctness Features** (move generation, UCI protocol, legal moves): Use TDD with comprehensive unit tests
- **Strength Features** (evaluation, search algorithms): Unit tests are optional; primary validation is ELO gain via SPRT
- **Rationale**: Strength improvements are best measured through actual game performance rather than unit test expectations

**Key Testing Commands:**

**Build and Test:**
```bash
# Build UCI engine
g++ src/main.cpp src/chess.cpp -o build/agent4k

# Build and run comprehensive tests (includes perft validation)
g++ src/chess_test.cpp src/chess.cpp -o build/test_chess && ./build/test_chess

# Quick UCI test
echo -e "uci\nisready\nposition startpos\ngo\nquit" | ./build/agent4k
```

**Game Testing with fastchess:**
```bash
# Basic 2-game self-play test
fastchess -engine cmd=./build/agent4k name=Agent4k -engine cmd=./build/agent4k name=Agent4k2 -each tc=8+0.08 -rounds 1 -games 2

# Test with opening book (4 games)
fastchess -engine cmd=./build/agent4k name=Agent4k -engine cmd=./build/agent4k name=Agent4k2 -each tc=8+0.08 -rounds 2 -games 2 -openings file=books/8moves_v3.pgn format=pgn order=random

# Longer test (10 games with book)
fastchess -engine cmd=./build/agent4k name=Agent4k -engine cmd=./build/agent4k name=Agent4k2 -each tc=8+0.08 -rounds 5 -games 2 -openings file=books/8moves_v3.pgn format=pgn order=random

# SPRT test vs baseline (requires baseline/engine_baseline)
# NOTE: Engine order matters! Expected stronger engine should be FIRST for proper SPRT interpretation
fastchess -engine cmd=./build/agent4k name=Test -engine cmd=baselines/engine_baseline name=Baseline -openings file=books/8moves_v3.pgn format=pgn -each tc=8+0.08 -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 -concurrency 4 -repeat -rounds 100

# Update baseline to current version
git tag -d baseline-latest 2>/dev/null || true  # Remove old baseline tag
git tag baseline-latest  # Tag current commit as latest baseline

# Build baseline engine for SPRT testing
mkdir -p baselines
git checkout baseline-latest
g++ src/main.cpp src/chess.cpp -o baselines/engine_baseline
git checkout -
```

**Move Generation Validation:**
- Comprehensive perft suite with 126 positions validates all edge cases
- Standard starting position: perft(1)=20, perft(2)=400, perft(3)=8902
- All perft tests run automatically in the comprehensive test suite
- Copy-make approach used for clean legal move validation

### Development Workflow
1. Edit `src/chess.cpp` or `src/main.cpp` with improvements
2. Run comprehensive tests: `g++ src/chess_test.cpp src/chess.cpp -o build/test_chess && ./build/test_chess`
3. Build engine: `g++ src/main.cpp src/chess.cpp -o build/agent4k`
4. Quick game test: `fastchess -engine cmd=./build/agent4k name=Agent4k -engine cmd=./build/agent4k name=Agent4k2 -each tc=8+0.08 -rounds 1 -games 2`
5. If tests pass, run SPRT vs baseline (if baseline exists)
6. If SPRT passes, commit improvement and update baseline-latest tag
7. Continue development cycle

**Standard Time Control:** 8+0.08 (8 seconds + 0.08 second increment) for all testing

**Debugging Process:**
1. Comprehensive tests reveal both functional and move generation bugs
2. 126 perft positions catch edge cases and missing/extra moves
3. Game tests reveal UCI protocol and time management issues
4. Copy-make approach allows clean position testing without side effects