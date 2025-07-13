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

## Commands to run
- Testing: `g++ engine.cpp -o engine && ./engine`
- Size check: `wc -c submission.gz`
- Compression: `gzip -9 engine.cpp`