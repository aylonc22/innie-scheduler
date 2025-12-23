# Lumon Industries - Innie Scheduler Test Suite

This document outlines the 11 test scenarios used to validate the stability, concurrency, and safety of the severed floor's task execution engine.

---

## Test Case Registry

### 1. Fundamental Arithmetic & Dependency Resolution
* **Test 1:** Validates basic `ADD` operations across multiple Innies (`HELLY`, `MARK`, `IRVING`).
* **Test 2:** **Multi-Waffle Persistence:** Ensures that `BURT` correctly captures sequential updates from `DYLAN` as they are finalized.
* **Test 8:** **State Reset:** Confirms `WELLNESS_CHECK` successfully resets `currentValue` to 0 when conditions are met.
* **Test 9:** **Dependency Chaining:** Verifies a "Relay Race" scenario where 5 Innies execute in a perfect linear sequence.
* **Test 11:** **Math Precision:** Validates `MODULO` operations combined with conditional checks.

### 2. Control Flow & Loops (The SHIFT Command)
* **Test 5:** **Nested Loops:** Tests the "Shift Stack" by running a loop within a loop ($0 + (1 + (10 \times 3)) \times 2 = 62$).
* **Test 7:** **Loop commitment:** Ensures `WAFFLE` inside a loop correctly broadcasts the "latest product" to waiting colleagues.

### 3. Logical Intelligence (Short-Circuiting)
* **Test 3:** **Complex Logic:** Evaluates `ANY OF` and `ALL OF` within shifting schedules.
* **Test 6:** **Race Conditions:** Checks if a fast Innie (`FAST_HELLY`) can satisfy a condition for another Innie before a slow Innie finishes.
* **Test 10:** **Logical Short-Circuit:** Proves that `MARK` will skip a dependency if a condition is false, avoiding unnecessary stalls (or deadlocks).

### 4. Safety & Deadlock Protocols (The -1 Rule)
* **Test 4:** **Standard Deadlock:** Detects a circular wait between `BURT` and `DYLAN`, triggering a safe termination with `-1`.
* **Test 10 (Redux):** Confirms that even if a colleague is deadlocked, the rest of the office can continue working if their logic allows for it.



---

## How to Validate
Each test is stored as a separate JSON object. To execute the suite, run:
```
node index.js tests/test_<number>.json
```