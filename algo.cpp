#include "algo.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <iomanip>

// Cycle ke edges ke weights jodo: profit = e^(-sum) - 1
// (weight me fee already included hai, isliye profit fee ke baad ka aata hai)
double cycleProfit(const Graph& g, const vector<int>& cycle) {
    double sum = 0.0;
    for (size_t i = 0; i < cycle.size(); i++) {
        int u = cycle[i];
        int v = cycle[(i + 1) % cycle.size()];   // aakhri node se wapas pehle pe

        const vector<Edge>& edges = g.edgesOf(u);
        for (size_t j = 0; j < edges.size(); j++) {
            if (edges[j].to == v && edges[j].active) {
                sum += edges[j].weight;
                break;
            }
        }
    }
    return exp(-sum) - 1.0;
}

Result bellmanFord(const Graph& g) {
    Result res;
    res.found = false;
    res.profit = 0.0;
    res.ops = 0;
    res.timeMs = 0.0;

    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

    int n = g.size();
    const double EPS = 1e-12;          // floating-point error se bachne ke liye

    vector<double> dist(n, 0.0);       // sab 0 se shuru (virtual source)
    vector<int> parent(n, -1);
    int last = -1;                     // pass me jis node ka dist improve hua

    for (int pass = 0; pass < n; pass++) {
        last = -1;
        for (int u = 0; u < n; u++) {
            const vector<Edge>& edges = g.edgesOf(u);
            for (size_t i = 0; i < edges.size(); i++) {
                if (!edges[i].active) continue;   // band edge skip
                res.ops++;

                int v = edges[i].to;
                if (dist[u] + edges[i].weight < dist[v] - EPS) {
                    dist[v] = dist[u] + edges[i].weight;
                    parent[v] = u;
                    last = v;
                }
            }
        }
        if (last == -1) break;         // koi improvement nahi: negative cycle nahi hai
    }

    if (last != -1) {                  // n passes ke baad bhi improve hua: cycle hai
        int x = last;
        for (int i = 0; i < n && x != -1; i++) {
            x = parent[x];             // n step peeche: pakka cycle ke andar
        }

        if (x != -1) {
            vector<int> cyc;
            int cur = x;
            do {
                cyc.push_back(cur);
                cur = parent[cur];
            } while (cur != x && cur != -1);

            reverse(cyc.begin(), cyc.end());   // parent peeche jaata hai, isliye ulta karo
            res.cycle = cyc;
            res.found = true;
            res.profit = cycleProfit(g, cyc);
        }
    }

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(end - start).count();
    return res;
}

void printResult(const Graph& g, const Result& r) {
    if (!r.found) {
        cout << "No arbitrage found";
    } else {
        cout << "Arbitrage: ";
        for (size_t i = 0; i < r.cycle.size(); i++) {
            cout << g.nameOf(r.cycle[i]) << " -> ";
        }
        cout << g.nameOf(r.cycle[0]);
        cout << " | Profit: " << fixed << setprecision(2) << r.profit * 100 << "%";
    }
    cout << " | Edge checks: " << r.ops
         << " | Time: " << fixed << setprecision(4) << r.timeMs << " ms" << endl;
}