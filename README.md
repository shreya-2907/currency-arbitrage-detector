# Currency Arbitrage Detector

A C++ project that detects profitable currency exchange opportunities using graph algorithms.

The project represents currencies as nodes and exchange rates as edges in a weighted directed graph. It uses different algorithms to find arbitrage opportunities and compares their performance.

---

## 1. What is Currency Arbitrage?

Currency arbitrage means taking advantage of differences in exchange rates by converting money through multiple currencies and ending up with more money than you started with.

For example:

```text
USD → EUR : 0.92
EUR → GBP : 0.86
GBP → USD : 1.28
```

Starting with 100 USD:

```text
100 USD → 92 EUR → 79.12 GBP → 101.27 USD
```

This gives a profit of approximately **1.27%**.

The aim of this project is to automatically find such profitable cycles, even when the graph contains hundreds of currencies and thousands of exchange rates.

## 2. How Does It Work?

The project uses a graph to represent the currency market:

- Each currency is represented as a node.
- Each exchange rate is represented as a directed edge.
- A sequence of exchanges forms a cycle.

A cycle is profitable when the product of its exchange rates is greater than 1.
However, shortest-path algorithms work with addition rather than multiplication. To solve this, we use logarithms.
The exchange rate is converted into a weight using:

```text
weight = -ln(rate)
```

This gives us an important relationship:

```text
Product of exchange rates > 1       ⇕
Sum of weights < 0
```

So, finding a profitable arbitrage opportunity becomes a **negative-weight cycle detection problem**.

This is why we use algorithms like Bellman-Ford and Floyd-Warshall instead of Dijkstra, which does not support negative edge weights.

## 3. Features

The program provides a menu through which users can explore different features.

| Option | Feature                 | Description                                                                   |
| ------ | ----------------------- | ----------------------------------------------------------------------------- |
| 1      | Show Exchange Rates     | Displays all available exchange rates and graph details.                      |
| 2      | Set Transaction Fee     | Applies a fee to every currency exchange.                                     |
| 3      | Detect Arbitrage        | Finds profitable cycles using the selected algorithm.                         |
| 4      | Compare Algorithms      | Runs all three algorithms and compares their results.                         |
| 5      | Break-even Fee          | Finds the maximum transaction fee at which arbitrage is still possible.       |
| 6      | Hop-limited Arbitrage   | Finds profitable cycles within a given number of trades.                      |
| 7      | Top-K Profitable Cycles | Displays the best distinct profitable cycles found.                           |
| 8      | Update Exchange Rate    | Updates an exchange rate and checks the graph again.                          |
| 9      | Benchmark               | Tests algorithm performance on graphs of different sizes.                     |
| 10     | Self-test               | Checks algorithm results against an exhaustive search on small random graphs. |

## 4. Algorithms Used

The project implements three main algorithms for detecting negative cycles.

| Algorithm      | Time Complexity     | How it works                                                                                              |
| -------------- | ------------------- | --------------------------------------------------------------------------------------------------------- |
| Bellman-Ford   | O(V × E)            | Repeatedly updates shortest distances. A further improvement after V-1 passes indicates a negative cycle. |
| SPFA           | O(V × E) worst case | An improved queue-based version of Bellman-Ford that processes only nodes whose distances have changed.   |
| Floyd-Warshall | O(V³)               | Finds shortest paths between all pairs of vertices. A negative diagonal value indicates a negative cycle. |

**Additional approaches:**

- **Hop-limited Search:** Uses Depth-Limited DFS to find profitable cycles within a maximum number of trades.
- **Top-K Cycle Search:** Repeatedly detects cycles and temporarily disables edges to find other profitable cycles. This is a heuristic approach, so it may not always find every top cycle.

All three main algorithms start with distances set to zero. This allows the program to detect negative cycles anywhere in the graph, without needing a specific starting currency.

A small epsilon value (`1e-12`) is also used to reduce errors caused by floating-point calculations.

## 5. Data Structures Used

Different data structures are used to make the program efficient and organised.

| Data Structure     | Purpose                                                        |
| ------------------ | -------------------------------------------------------------- |
| Adjacency List     | Stores currencies and their outgoing exchange rates.           |
| Custom Hash Map    | Maps currency names to their corresponding graph indices.      |
| Circular Queue     | Used by SPFA to manage nodes that need processing.             |
| Parent Array       | Helps reconstruct the detected arbitrage cycle.                |
| 2D Distance Matrix | Stores shortest distances between all pairs in Floyd-Warshall. |
| Recursion Stack    | Used during depth-limited DFS searches.                        |

## 6. Project Structure

```text
arbitrage/
│
├── main.cpp
├── graph.h
├── graph.cpp
├── hashmap.h
├── myqueue.h
├── algo.h
├── algo.cpp
├── features.h
├── features.cpp
├── generator.h
├── generator.cpp
├── benchmark.h
├── benchmark.cpp
│
└── data/
    └── rates.txt
```

**File descriptions:**

- `main.cpp` – Handles the menu and user interaction.
- `graph.h / graph.cpp` – Manages currencies, exchange rates and graph operations.
- `hashmap.h` – Implements the custom hash map.
- `myqueue.h` – Implements the circular queue used in SPFA.
- `algo.h / algo.cpp` – Contains the main graph algorithms and related calculations.
- `features.h / features.cpp` – Handles hop-limited arbitrage and top-K cycle detection.
- `generator.h / generator.cpp` – Generates random currency graphs for testing.
- `benchmark.h / benchmark.cpp` – Handles performance comparisons and self-tests.
- `data/rates.txt` – Contains the sample exchange rate data.

## 7. How to Run the Project

**Requirements:**

- C++ compiler (g++ or clang++)
- C++11 or later

**Step 1: Compile the project**

```bash
g++ -std=c++11 *.cpp -o arb
```

**Step 2: Run the program**

```bash
./arb
```

For Windows:

```bash
arb.exe
```

Make sure to run the program from the main project folder so that it can access `data/rates.txt`.

### Input Format

Exchange rates are stored in `data/rates.txt`.
Each line contains the source currency, destination currency and exchange rate.

Example:

```text
USD EUR 0.92
USD GBP 0.79
EUR USD 1.07
```

All exchange rates must be greater than zero. New currencies are added automatically when they appear in the input file.

## 8. Sample Output

Here are a few examples of what the program displays.

### Comparing All Algorithms

```text
Algorithm       Found   Profit    Edge Checks   Time (ms)
Bellman-Ford    Yes     1.27%     48            0.0086
SPFA            Yes     1.27%     19            0.0046
Floyd-Warshall  Yes     1.12%     11            0.0023
```

Each algorithm can find a profitable cycle, but the cycle it finds may be different.

### Finding Top-K Profitable Cycles

```text
Rank  Profit    Cycle
1     1.27%     USD>EUR>GBP>USD
2     1.12%     USD>GBP>USD
3     0.18%     USD>EUR>INR>GBP>USD
4     0.11%     USD>INR>GBP>USD
```

### Finding the Break-even Fee

```text
Break-even fee: 0.5553% per trade
```

This represents the maximum transaction fee at which the sample market still has an arbitrage opportunity.

### Hop-limited Arbitrage

```text
Max Trades    Profit    Cycle
2             1.12%     USD>GBP>USD
3             1.27%     USD>EUR>GBP>USD
```

These examples are based on the sample exchange rate data.

## 9. Performance Benchmarking

The project also includes a benchmarking feature to compare the performance of all three algorithms.

It generates graphs containing:

- 10, 50, 100, 200 and 500 currencies.
- Sparse graphs (10% edge density).
- Dense graphs (100% edge density).
- Graphs with and without an intentionally added arbitrage cycle.

This helps us understand how the algorithms behave under different conditions.

**Sample Benchmark Results**

500 currencies, dense graph (249,500 edges):

| Arbitrage Present | Bellman-Ford (ms) | SPFA (ms) | Floyd-Warshall (ms) |
| ----------------- | ----------------: | --------: | ------------------: |
| No                |               5.6 |       3.2 |               933.7 |
| Yes               |            1010.9 |     836.0 |               236.9 |

The results show that:

- SPFA performs well in many cases because it avoids processing unnecessary nodes.
- Floyd-Warshall can be expensive on large graphs because of its cubic time complexity.
- No single algorithm performs best in every situation.

Actual timings may vary depending on the system.

All benchmark results are saved in `benchmark_results.csv` and can be viewed using spreadsheet software.

## 10. Testing and Validation

To check whether the algorithms work correctly, the project includes two testing features.

**1. Self-test**

- Generates 500 random graphs containing 3 to 7 currencies.
- Compares Bellman-Ford, SPFA and Floyd-Warshall results with an exhaustive search over simple cycles.
- Checks whether the detected cycles are valid and actually profitable.

**2. Benchmark Testing**

- Generates graphs with known arbitrage conditions.
- Tests whether the algorithms correctly identify the presence or absence of arbitrage.
- Compares their execution times.

The hop-limited search was also tested against brute-force results on thousands of random graphs during development.

## 11. Limitations

Although the project can detect arbitrage opportunities in the given data, it has some limitations:

- **No live exchange rates:** The program reads rates from a file and does not fetch real-time market data.
- **Transaction limitations:** Real-world factors such as liquidity, trade size and execution delays are not considered.
- **Top-K Search:** This feature uses a heuristic approach and may occasionally miss some of the best cycles.
- **Hop-limited Search:** Its execution time increases significantly as the number of allowed trades increases.
- **Floating-point Precision:** Very small arbitrage opportunities may be affected by rounding errors.

## 12. Future Improvements

Some improvements that can be added to the project in the future include:

- Fetching live exchange rates through an API.
- Considering bid/ask spreads and different transaction fees.
- Exploring more graph algorithms, such as Karp's Minimum Mean Cycle Algorithm.
- Adding a graphical interface to visualise currency connections and profitable cycles.
