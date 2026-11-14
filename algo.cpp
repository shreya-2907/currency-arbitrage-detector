#include "algo.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include "Myqueue.h"

double cycleProfit(const Graph& g, const vector<int>& cycle) {
    double sum = 0.0;
    for (size_t i = 0; i < cycle.size(); i++) {
        int u = cycle[i];
        int v = cycle[(i + 1) % cycle.size()];

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
    const double EPS = 1e-12;

    vector<double> dist(n, 0.0);
    vector<int> parent(n, -1);
    int last = -1;

    for (int pass = 0; pass < n; pass++) {
        last = -1;
        for (int u = 0; u < n; u++) {
            const vector<Edge>& edges = g.edgesOf(u);
            for (size_t i = 0; i < edges.size(); i++) {
                if (!edges[i].active) continue;
                res.ops++;

                int v = edges[i].to;
                if (dist[u] + edges[i].weight < dist[v] - EPS) {
                    dist[v] = dist[u] + edges[i].weight;
                    parent[v] = u;
                    last = v;
                }
            }
        }
        if (last == -1) break;
    }

    if (last != -1) {
        int x = last;
        for (int i = 0; i < n && x != -1; i++) {
            x = parent[x];
        }

        if (x != -1) {
            vector<int> cyc;
            int cur = x;
            do {
                cyc.push_back(cur);
                cur = parent[cur];
            } while (cur != x && cur != -1);

            reverse(cyc.begin(), cyc.end());
            res.cycle = cyc;
            res.found = true;
            res.profit = cycleProfit(g, cyc);
        }
    }

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(end - start).count();
    return res;
}


static vector<int> cycleFromParent(const vector<int>& parent, int start, int n) {
    vector<int> cyc;
    int x = start;
    for (int i = 0; i < n && x != -1; i++) x = parent[x];
    if (x == -1) return cyc;

    int cur = x;
    do {
        cyc.push_back(cur);
        cur = parent[cur];
    } while (cur != x && cur != -1);

    if (cur == -1) { cyc.clear(); return cyc; }
    reverse(cyc.begin(), cyc.end());
    return cyc;
}


Result spfa(const Graph& g) {
    Result res;
    res.found = false;
    res.profit = 0.0;
    res.ops = 0;
    res.timeMs = 0.0;

    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

    int n = g.size();
    const double EPS = 1e-12;

    vector<double> dist(n, 0.0);
    vector<int> parent(n, -1);
    vector<int> len(n, 0);
    vector<bool> inQueue(n, true);

    IntQueue q(n + 1);
    for (int i = 0; i < n; i++) q.push(i);      
    while (!q.empty() && !res.found) {
        int u = q.pop();
        inQueue[u] = false;

        const vector<Edge>& edges = g.edgesOf(u);
        for (size_t i = 0; i < edges.size(); i++) {
            if (!edges[i].active) continue;
            res.ops++;

            int v = edges[i].to;
            if (dist[u] + edges[i].weight < dist[v] - EPS) {
                dist[v] = dist[u] + edges[i].weight;
                parent[v] = u;
                len[v] = len[u] + 1;

                if (len[v] >= n) {              
                    vector<int> cyc = cycleFromParent(parent, v, n);
                    if (!cyc.empty()) {
                        res.cycle = cyc;
                        res.found = true;
                        res.profit = cycleProfit(g, cyc);
                        break;
                    }
                }

                if (!inQueue[v]) {
                    inQueue[v] = true;
                    q.push(v);
                }
            }
        }
    }

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(end - start).count();
    return res;
}

// Floyd-Warshall

Result floydWarshall(const Graph& g) {
    Result res;
    res.found = false;
    res.profit = 0.0;
    res.ops = 0;
    res.timeMs = 0.0;

    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

    int n = g.size();
    const double EPS = 1e-12;
    const double INF = 1e18;

    vector<double> dist((size_t)n * n, INF);     
    vector<int> pred((size_t)n * n, -1);

    for (int i = 0; i < n; i++) dist[(size_t)i * n + i] = 0.0;

    for (int u = 0; u < n; u++) {
        const vector<Edge>& edges = g.edgesOf(u);
        for (size_t k = 0; k < edges.size(); k++) {
            if (!edges[k].active) continue;
            int v = edges[k].to;
            if (v == u) continue;
            if (edges[k].weight < dist[(size_t)u * n + v]) {
                dist[(size_t)u * n + v] = edges[k].weight;
                pred[(size_t)u * n + v] = u;
            }
        }
    }

    int bad = -1;                                
    for (int k = 0; k < n && bad == -1; k++) {
        for (int i = 0; i < n && bad == -1; i++) {
            if (dist[(size_t)i * n + k] >= INF) continue;
            for (int j = 0; j < n; j++) {
                if (dist[(size_t)k * n + j] >= INF) continue;
                res.ops++;
                double nd = dist[(size_t)i * n + k] + dist[(size_t)k * n + j];
                if (nd < dist[(size_t)i * n + j] - EPS) {
                    dist[(size_t)i * n + j] = nd;
                    pred[(size_t)i * n + j] = pred[(size_t)k * n + j];
                    if (i == j) { bad = i; break; }   // negative cycle found
                }
            }
        }
    }

    if (bad != -1) {
        
        vector<int> walk;
        int cur = bad;
        for (int steps = 0; steps <= n + 1; steps++) {
            walk.push_back(cur);
            cur = pred[(size_t)bad * n + cur];
            if (cur == -1 || cur == bad) break;
        }

        
        vector<int> cyc;
        for (size_t i = 0; i < walk.size() && cyc.empty(); i++) {
            for (size_t j = i + 1; j < walk.size(); j++) {
                if (walk[j] == walk[i]) {
                    cyc.assign(walk.begin() + i, walk.begin() + j);
                    break;
                }
            }
        }
        if (cyc.empty()) cyc = walk;
        reverse(cyc.begin(), cyc.end());         

        double p = cycleProfit(g, cyc);
        if (p > 0) {
            res.found = true;
            res.cycle = cyc;
            res.profit = p;
        } else {
            
            Result bf = bellmanFord(g);
            res.found = bf.found;
            res.cycle = bf.cycle;
            res.profit = bf.profit;
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


double breakEvenFee(Graph& g) {
    double oldFee = g.getFee();

    g.setFee(0.0);
    if (!bellmanFord(g).found) {
        g.setFee(oldFee);
        return -1.0;
    }

    double lo = 0.0;      // arbitrage exists at this fee
    double hi = 0.5;      // no arbitrage at this fee (50%)
    for (int i = 0; i < 50; i++) {
        double mid = (lo + hi) / 2.0;
        g.setFee(mid);
        if (bellmanFord(g).found) lo = mid;
        else hi = mid;
    }

    g.setFee(oldFee);     
    return lo;
}