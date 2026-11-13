#include "benchmark.h"
#include "graph.h"
#include "algo.h"
#include "features.h"
#include "generator.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <set>
using namespace std;

// Teeno algorithms ko ek graph pe 'reps' baar chalao, average time aur aakhri run ka result do
static void measure(const Graph& g, int reps, Result out[3], double avgMs[3]) {
    for (int a = 0; a < 3; a++) {
        double total = 0.0;
        for (int r = 0; r < reps; r++) {
            if (a == 0) out[a] = bellmanFord(g);
            else if (a == 1) out[a] = spfa(g);
            else out[a] = floydWarshall(g);
            total += out[a].timeMs;
        }
        avgMs[a] = total / reps;
    }
}

static int countEdges(const Graph& g) {
    int total = 0;
    for (int u = 0; u < g.size(); u++) total += g.edgesOf(u).size();
    return total;
}

void runBenchmark() {
    const int sizes[5] = {10, 50, 100, 200, 500};
    const double densities[2] = {0.1, 1.0};

    ofstream csv("benchmark_results.csv");
    csv << "V,density,edges,planted,bf_ms,spfa_ms,fw_ms,bf_checks,spfa_checks,fw_checks,all_correct" << endl;

    cout << endl << "Planted-arbitrage benchmark (spread 0.5%, planted cycle: 3 currencies, +2% per trade)" << endl;
    cout << "Time = average milliseconds, checks = edge checks (Floyd-Warshall: triple-loop steps)" << endl << endl;
    cout << right << setw(5) << "V" << setw(6) << "Dens" << setw(8) << "Edges" << setw(7) << "Plant"
         << setw(10) << "BF ms" << setw(10) << "SPFA ms" << setw(10) << "FW ms"
         << setw(11) << "BF chk" << setw(11) << "SPFA chk" << setw(11) << "FW chk" << setw(5) << "OK" << endl;
    cout << string(94, '-') << endl;

    bool everythingOk = true;

    for (int si = 0; si < 5; si++) {
        for (int di = 0; di < 2; di++) {
            for (int planted = 0; planted < 2; planted++) {
                int n = sizes[si];
                Graph g;
                buildMarket(g, n, densities[di], 0.005, planted ? 3 : 0, 0.02, 42);

                int reps = (n <= 10) ? 200 : (n <= 50 ? 20 : (n <= 100 ? 5 : 1));
                Result res[3];
                double ms[3];
                measure(g, reps, res, ms);

                // sahi jawab: planted ho to teeno ko "found" kehna chahiye, warna "not found"
                bool ok = true;
                for (int a = 0; a < 3; a++) {
                    if (res[a].found != (planted == 1)) ok = false;
                }
                if (!ok) everythingOk = false;

                int edges = countEdges(g);
                cout << right << setw(5) << n << setw(5) << (int)(densities[di] * 100) << "%"
                     << setw(8) << edges << setw(7) << (planted ? "Yes" : "No")
                     << fixed << setprecision(3)
                     << setw(10) << ms[0] << setw(10) << ms[1] << setw(10) << ms[2]
                     << setw(11) << res[0].ops << setw(11) << res[1].ops << setw(11) << res[2].ops
                     << setw(5) << (ok ? "yes" : "NO") << endl;

                csv << n << "," << densities[di] << "," << edges << "," << (planted ? "yes" : "no") << ","
                    << ms[0] << "," << ms[1] << "," << ms[2] << ","
                    << res[0].ops << "," << res[1].ops << "," << res[2].ops << ","
                    << (ok ? "yes" : "no") << endl;
            }
        }
    }

    csv.close();
    cout << string(94, '-') << endl;
    cout << (everythingOk ? "All algorithms gave the correct answer on every graph."
                          : "WARNING: some algorithm gave a wrong answer (see OK column).") << endl;
    cout << "Results saved to benchmark_results.csv (Excel me khol ke graph bana sakte ho)." << endl;
}

// Cycle valid hai? (koi node repeat nahi, har edge maujood, aur sach me profit hai)
static bool isValidCycle(const Graph& g, const vector<int>& c) {
    set<int> s(c.begin(), c.end());
    if (c.size() < 2 || s.size() != c.size()) return false;
    return cycleProfit(g, c) > 0;
}

void runSelfTest() {
    mt19937 rng(7);
    uniform_real_distribution<double> unit(0.0, 1.0);

    const int TESTS = 500;
    int mismatch[3] = {0, 0, 0};
    int invalid[3] = {0, 0, 0};
    int withArbitrage = 0;

    for (int t = 0; t < TESTS; t++) {
        int n = 3 + (int)(unit(rng) * 5);               // 3 se 7 currencies
        if (n > 7) n = 7;
        Graph g;
        vector<double> price(n);
        for (int i = 0; i < n; i++) {
            g.addCurrency("C" + to_string(i));
            price[i] = 1.0 + unit(rng) * 99.0;
        }
        double density = 0.5 + unit(rng) * 0.4;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j && unit(rng) < density) {
                    double noise = 0.985 + 0.03 * unit(rng);    // rates me thoda shor
                    g.addExchangeRateIdx(i, j, price[i] / price[j] * noise);
                }
            }
        }
        g.setFee(unit(rng) < 0.5 ? 0.0 : 0.001);

        // brute force: n trades tak ke saare simple cycles me se best (exact)
        Result truth = hopLimited(g, n);
        if (truth.found) withArbitrage++;

        Result r[3] = {bellmanFord(g), spfa(g), floydWarshall(g)};
        for (int a = 0; a < 3; a++) {
            if (r[a].found != truth.found) mismatch[a]++;
            if (r[a].found && !isValidCycle(g, r[a].cycle)) invalid[a]++;
        }
    }

    cout << endl << "Self-test: " << TESTS << " random graphs (3 to 7 currencies), "
         << withArbitrage << " of them contain arbitrage." << endl;
    cout << "Answer compared with brute force (all simple cycles checked):" << endl;
    const char* names[3] = {"Bellman-Ford", "SPFA", "Floyd-Warshall"};
    for (int a = 0; a < 3; a++) {
        cout << "  " << left << setw(16) << names[a] << "wrong answers: " << mismatch[a]
             << ", invalid cycles: " << invalid[a] << endl;
    }
    bool pass = true;
    for (int a = 0; a < 3; a++) {
        if (mismatch[a] != 0 || invalid[a] != 0) pass = false;
    }
    cout << (pass ? "PASS: all algorithms agree with brute force." : "FAIL: see numbers above.") << endl;
}