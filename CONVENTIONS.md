### CONVENTIONS.md for Agent4k Development

This document outlines the core principles and priorities for developing the `Agent4k` chess engine. Adhere to these conventions strictly.

1.  **Primary Goal:** Win the TCEC 4k tournament. All decisions must align with maximizing engine strength within the given constraints.

2.  **Absolute Constraint: 4KiB Size Limit:**
    *   The final submitted file (wrapper script + compressed engine) **MUST NOT EXCEED 4096 bytes**.
    *   **During development and debugging, exceeding this limit is permissible.** However, the limit **must be met before any feature is considered complete** and certainly before final submission.
    *   **This limit applies *only* to the size of the submitted package.**
    *   It does *not* imply any runtime resource constraints (CPU, RAM, RAM, disk space) on the machine executing the compiled engine. The compiled engine can use available system resources freely.
    *   There are no size limits on development-time files, testing scripts, or other non-submitted assets.
    *   Every byte counts. Aggressively minify all source code (C/C++ and shell script). Remove all unnecessary whitespace, comments, and use the shortest possible variable/function names.
    *   Leverage `xz` compression and on-the-fly compilation (C/C++) via the `run.sh` wrapper script to bypass the 4KiB limit for the *decompressed/compiled* engine.

3.  **No Human Readability/Writeability Concern:**
    *   Code is for the agent, not humans. Prioritize performance and size over clarity.
    *   This applies to the *submitted* engine source. Development-time code (e.g., for debugging) can be more readable if stripped before packaging.
    *   **Name Mapping:** To aid future understanding and modification of minified code, maintain `name_map.txt` to document the mapping of original, descriptive names to their minified counterparts. This file must be kept up-to-date with any minification changes.
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

8.  **Strength Development Workflow (SPRT Testing):**
    To validate strength improvements, SPRT (Sequential Probability Ratio Test) will be used. This requires managing engine versions.

    *   **Version Tagging:** Each time a stable, functional version is achieved that might serve as a new strength baseline, tag the current Git commit using `git tag vX.Y` (e.g., `git tag v0.1`).
    *   **Baseline Definition:** The `Makefile` contains a `BASELINE_TAG` variable. This variable should point to the Git tag of the engine version currently considered the strength baseline. Update this tag when a new version is proven stronger.
    *   **Development Cycle for Strength Improvements:**
        1.  Implement a new feature or optimization in `engine.c` aimed at increasing playing strength.
        2.  Run `make test` to ensure basic UCI compliance and no regressions.
        3.  Run `make run-selfplay-book` (or `make run-selfplay-startpos`) to perform initial functional testing across varied positions.
        4.  If basic functionality is confirmed, commit the changes (`git commit -m "Implemented [feature]"`).
        5.  Run `make run-sprt-test` to compare the current `HEAD` against the `BASELINE_TAG`. This will run a statistically significant number of games.
        6.  Analyze the `sprt_test.log` output.
        7.  If the SPRT test indicates a statistically significant strength gain (e.g., current engine is proven stronger than baseline), then:
            *   Tag the current commit (e.g., `git tag v0.2`).
            *   Update the `BASELINE_TAG` variable in the `Makefile` to this new tag (e.g., `v0.2`).
            *   Commit the `Makefile` change (`git commit -m "Updated baseline to v0.2 after SPRT win"`).
        8.  If the SPRT test is inconclusive or shows a loss, revert the changes or iterate on the feature.

    This process ensures that only statistically validated strength improvements are integrated into the baseline.
