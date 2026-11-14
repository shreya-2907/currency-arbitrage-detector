#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include "graph.h"
#include "algo.h"
#include "features.h"
#include "benchmark.h"
using namespace std;


Result runAlgorithm(int choice, const Graph& g) {
    if (choice == 1) return bellmanFord(g);
    if (choice == 2) return spfa(g);
    return floydWarshall(g);
}

string algorithmName(int choice) {
    if (choice == 1) return "Bellman-Ford";
    if (choice == 2) return "SPFA";
    return "Floyd-Warshall";
}

void compareAll(const Graph& g) {
    cout << endl;
    cout << left << setw(16) << "Algorithm" << setw(8) << "Found"
         << setw(10) << "Profit" << setw(14) << "Edge checks"
         << setw(12) << "Time (ms)" << "Cycle" << endl;

    for (int a = 1; a <= 3; a++) {
        Result r = runAlgorithm(a, g);
        cout << left << setw(16) << algorithmName(a)
             << setw(8) << (r.found ? "Yes" : "No");

        if (r.found) {
            ostringstream p;                               
            p << fixed << setprecision(2) << r.profit * 100 << "%";
            cout << setw(10) << p.str();
        } else {
            cout << setw(10) << "-";
        }

        cout << setw(14) << r.ops << setw(12) << fixed << setprecision(4) << r.timeMs;

        if (r.found) {
            for (size_t i = 0; i < r.cycle.size(); i++) cout << g.nameOf(r.cycle[i]) << ">";
            cout << g.nameOf(r.cycle[0]);
        }
        cout << endl;
    }

}


void hopTable(const Graph& g, int maxK) {
    cout << endl;
    cout << left << setw(14) << "Max trades" << setw(10) << "Profit" << "Cycle" << endl;
    for (int k = 2; k <= maxK; k++) {
        Result r = hopLimited(g, k);
        cout << left << setw(14) << k;
        if (!r.found) {
            cout << "none" << endl;
            continue;
        }
        ostringstream p;
        p << fixed << setprecision(2) << r.profit * 100 << "%";
        cout << setw(10) << p.str();
        for (size_t i = 0; i < r.cycle.size(); i++) cout << g.nameOf(r.cycle[i]) << ">";
        cout << g.nameOf(r.cycle[0]) << endl;
    }
}

void printTopK(Graph& g, int k, int algo) {
    vector<Result> list = topKCycles(g, k, algo == 1 ? bellmanFord : (algo == 2 ? spfa : floydWarshall));
    if (list.empty()) {
        cout << "No arbitrage found." << endl;
        return;
    }
    cout << endl;
    cout << left << setw(6) << "Rank" << setw(10) << "Profit" << "Cycle" << endl;
    for (size_t i = 0; i < list.size(); i++) {
        ostringstream p;
        p << fixed << setprecision(2) << list[i].profit * 100 << "%";
        cout << left << setw(6) << i + 1 << setw(10) << p.str();
        for (size_t j = 0; j < list[i].cycle.size(); j++) cout << g.nameOf(list[i].cycle[j]) << ">";
        cout << g.nameOf(list[i].cycle[0]) << endl;
    }
    cout << "Note: top-k is a heuristic; finding every cycle is not guaranteed." << endl;
}

void printMenu(const Graph& g) {
    cout << endl;
    cout << "===== Currency Arbitrage Detector =====" << endl;
    cout << "Current fee: " << fixed << setprecision(3) << g.getFee() * 100 << "% per trade" << endl;
    cout << "1. Show exchange rates" << endl;
    cout << "2. Set fee per trade" << endl;
    cout << "3. Detect arbitrage (choose algorithm)" << endl;
    cout << "4. Compare all algorithms" << endl;
    cout << "5. Break-even fee" << endl;
    cout << "6. Hop-limited arbitrage (max trades)" << endl;
    cout << "7. Top-k profitable cycles" << endl;
    cout << "8. Update one exchange rate" << endl;
    cout << "9. Benchmark (planted arbitrage, saves CSV)" << endl;
    cout << "10. Self-test against brute force" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter choice: ";
}

int main() {
    Graph g;

    if (!g.loadRatesFromFile("data/rates.txt")) {
        cout << "Could not open data/rates.txt" << endl;
        return 1;
    }

    int choice = -1;
    while (choice != 0) {
        printMenu(g);

        if (!(cin >> choice)) {                      
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
            cout << "Please enter a number." << endl;
            continue;
        }

        if (choice == 1) {
            g.displayGraph();
        }
        else if (choice == 2) {
            double percent;
            cout << "Enter fee in percent (example 0.2 for 0.2%): ";
            if (cin >> percent && percent >= 0 && percent < 100) {
                g.setFee(percent / 100.0);
                cout << "Fee set to " << percent << "%" << endl;
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid fee." << endl;
            }
        }
        else if (choice == 3) {
            int a;
            cout << "1. Bellman-Ford  2. SPFA  3. Floyd-Warshall" << endl;
            cout << "Choose algorithm: ";
            if (cin >> a && a >= 1 && a <= 3) {
                cout << algorithmName(a) << ": ";
                printResult(g, runAlgorithm(a, g));
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid algorithm number." << endl;
            }
        }
        else if (choice == 4) {
            compareAll(g);
        }
        else if (choice == 5) {
            double be = breakEvenFee(g);
            if (be < 0) cout << "No arbitrage even with 0% fee." << endl;
            else cout << "Break-even fee: " << fixed << setprecision(4) << be * 100 << "% per trade" << endl;
        }
        else if (choice == 6) {
            int k;
            cout << "Maximum number of trades to show (2 to 6): ";
            if (cin >> k && k >= 2 && k <= 6) {
            
                int allowed = 6;
                if (g.size() > 60) allowed = 3;
                else if (g.size() > 25) allowed = 4;
                if (k > allowed) {
                    cout << "Large graph: maximum trades limited to " << allowed << "."<< endl;
                    k = allowed;
                }
                hopTable(g, k);
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number from 2 to 6." << endl;
            }
        }
        else if (choice == 7) {
            int k, a;
            cout << "How many cycles (1 to 10): ";
            if (!(cin >> k) || k < 1 || k > 10) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number from 1 to 10." << endl;
                continue;
            }
            cout << "1. Bellman-Ford  2. SPFA  3. Floyd-Warshall" << endl;
            cout << "Choose algorithm: ";
            if (cin >> a && a >= 1 && a <= 3) {
                printTopK(g, k, a);
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid algorithm number." << endl;
            }
        }
        else if (choice == 8) {
            string from, to;
            double rate;
            cout << "Enter: from to new_rate (example: USD EUR 0.95): ";
            if (cin >> from >> to >> rate && g.updateRateByName(from, to, rate)) {
                cout << "Updated. Re-checking with Bellman-Ford: ";
                printResult(g, bellmanFord(g));
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Could not update. Check currency names and use a rate greater than 0." << endl;
            }
        }
        else if (choice == 9) {
            runBenchmark();
        }
        else if (choice == 10) {
            runSelfTest();
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }

    cout << "Bye." << endl;
    return 0;
}