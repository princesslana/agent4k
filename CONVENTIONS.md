### CONVENTIONS.md for Agent4k Development

This document outlines the core principles and priorities for developing the `Agent4k` chess engine. Adhere to these conventions strictly.

1.  **Primary Goal:** Win the TCEC 4k tournament. All decisions must align with maximizing engine strength within the given constraints.

2.  **Absolute Constraint: 4KiB Size Limit:**
    *   The final submitted file (wrapper script + compressed engine) **MUST NOT EXCEED 4096 bytes**.
    *   **This limit applies *only* to the size of the submitted package.**
    *   It does *not* imply any runtime resource constraints (CPU, RAM, disk space) on the machine executing the compiled engine. The compiled engine can use available system resources freely.
    *   There are no size limits on development-time files, testing scripts, or other non-submitted assets.
    *   Every byte counts. Aggressively minify all source code (C/C++ and shell script). Remove all unnecessary whitespace, comments, and use the shortest possible variable/function names.
    *   Leverage `xz` compression and on-the-fly compilation (C/C++) via the `run.sh` wrapper script to bypass the 4KiB limit for the *decompressed/compiled* engine.

3.  **No Human Readability/Writeability Concern:**
    *   Code is for the agent, not humans. Prioritize performance and size over clarity.
    *   This applies to the *submitted* engine source. Development-time code (e.g., for debugging) can be more readable if stripped before packaging.
    *   Use aggressive compiler optimizations (e.g., `-O3`).
    *   Employ bitwise operations, lookup tables, and other low-level optimizations.

4.  **Core Language:** C/C++ (as previously decided).

5.  **UCI Protocol:**
    *   Strictly adhere to the 4k UCI subset: `go`, `go wtime (A) btime (B) winc (C) binc (D)`, `bestmove (move)`, `isready`, `position startpos (moves ..)`, `readyok`, `uci`, `uciok`, `quit`.
    *   Tolerate other UCI commands gracefully (do not crash).

6.  **Performance Metrics (Your Feedback Loop):**
    *   **Correctness:** Pass all provided unit and integration tests. Bugs are critical and must be fixed immediately.
    *   **Speed:** Maximize Nodes Per Second (NPS).
    *   **Strength:** Maximize Elo rating (via self-play or against reference engines).
    *   **Startup Time:** Ensure the engine starts and is ready within 60 seconds.

7.  **Development Process:**
    *   **Outcome-Driven:** Focus on achieving the desired outcome (passing tests, improving performance, staying within size) rather than specific implementation details unless explicitly instructed.
    *   **Iterative & Incremental:** Make small, testable changes. Verify correctness and performance after each significant modification.
    *   **Debugging:** If a test fails or performance degrades, identify and fix the root cause. Temporary debugging output (e.g., `printf` statements) is allowed *only* for debugging purposes and must be removed before committing or finalizing.
    *   **Self-Correction:** If a change negatively impacts any key metric (size, speed, correctness), revert or fix it.
