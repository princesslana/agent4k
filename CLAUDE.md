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
│   ├── engine.h           # Shared header with function declarations
│   ├── engine_core.cpp    # Core engine implementation  
│   ├── main.cpp           # UCI interface main function
│   ├── perft.cpp          # Perft testing executable
│   └── tests.cpp          # Unit tests executable
├── tools/
│   ├── compress.py        # Size optimization tools
│   └── create_submission.sh
├── books/
│   └── 8moves_v3.pgn     # Opening book
├── baselines/
│   └── engine_baseline    # Reference engine for testing
└── build/                 # Compiled binaries (gitignored)
```

### Development Principles
1. **Single source of truth**: Engine logic in `engine_core.cpp`, shared via `engine.h`
2. **No code duplication**: All executables link to `engine_core.cpp`, don't copy code
3. **Clear separation**: Engine logic, UCI interface, tests, tools in separate files
4. **Proper linking**: Split main() functions from core logic to enable test compilation

## Testing & Development Workflow

### Engine Testing Strategy
**SPRT (Sequential Probability Ratio Test)** for statistically confident improvement testing:

**SPRT Parameters:**
- Early development: `elo0=0 elo1=10` (wider bounds = faster completion)
- Later validation: `elo0=0 elo1=5` (narrower bounds = more precise)
- Alpha/Beta: 0.05 (95% confidence)

**Version Management:**
1. `engine_baseline` - Fixed reference (current: engine_smart copy)
2. `engine_smart` - Current best version
3. `engine_test` - Version being developed/tested

**Testing Workflow:**
1. Make improvements to new version (`engine_test`)
2. Test `engine_test` vs `engine_baseline` using SPRT
3. If SPRT passes → copy `engine_test` to `engine_smart`
4. Git commit improvement
5. Repeat

**Opening Book:** `8moves_v3.pgn` (Stockfish standard, 8MB PGN format)

### Testing Approach & Commands

**Layered Testing Strategy:**
1. **Unit Tests** - Test specific functions in isolation
2. **Perft Tests** - Verify move generation correctness via node counting
3. **SPRT Tests** - Statistical validation of engine strength improvements

**Key Testing Commands:**
- Compile engine: `g++ main.cpp engine_core.cpp -o engine`
- Run perft tests: `g++ perft.cpp engine_core.cpp -o perft && ./perft`
- Run unit tests: `g++ tests.cpp engine_core.cpp -o tests && ./tests`
- SPRT testing: `fastchess -engine cmd=./baselines/engine_baseline name=Baseline -engine cmd=./engine name=Test -openings file=books/8moves_v3.pgn format=pgn -each tc=0.1+0.01 -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 -concurrency 4 -repeat -rounds 100`
- Size check: `wc -c submission.gz`
- Compression: `gzip -9 main.cpp engine_core.cpp`

**Perft Debugging:**
- Standard perft values: perft(1)=20, perft(2)=400, perft(3)=8902
- Split perft for debugging specific moves: Use webperft tool for comparison
- Copy-make approach preferred over make/unmake for debugging clarity

### Development Workflow
1. Edit `src/engine_core.cpp` with improvements
2. Run unit tests: `./tests` (check specific functions)
3. Run perft tests: `./perft` (verify move generation correctness)
4. If tests pass, run SPRT vs baseline using fastchess
5. If SPRT passes, copy `./engine` to `baselines/engine_baseline`
6. Commit improvement to git

**Debugging Process:**
1. Unit tests reveal functional bugs (e.g., wrong moves generated)
2. Perft tests reveal move generation bugs (e.g., missing/extra moves)
3. Split perft identifies specific problematic moves
4. Copy-make approach allows clean position testing without side effects