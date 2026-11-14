#include "features.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <set>
#include <utility>


// 1) Hop-limited arbitrage


struct HopSearch {
    const Graph* g;
    int k;
    int start;
    vector<int> path;
    vector<bool> onPath;
    double bestSum;
    vector<int> bestCycle;
    long long ops;
};

static void hopDfs(HopSearch& hs, int u, double sum) {
    const vector<Edge>& edges = hs.g->edgesOf(u);
    int edgesUsed = (int)hs.path.size() - 1;          

    for (size_t i = 0; i < edges.size(); i++) {
        if (!edges[i].active) continue;
        hs.ops++;
        int v = edges[i].to;
        double nsum = sum + edges[i].weight;

        if (v == hs.start) {
            if (edgesUsed + 1 >= 2 && nsum < hs.bestSum - 1e-12) {  
                hs.bestSum = nsum;
                hs.bestCycle = hs.path;
            }
        } else if (v > hs.start && !hs.onPath[v] && edgesUsed + 2 <= hs.k) {
            hs.onPath[v] = true;
            hs.path.push_back(v);
            hopDfs(hs, v, nsum);
            hs.path.pop_back();
            hs.onPath[v] = false;
        }
    }
}

Result hopLimited(const Graph& g, int k) {
    Result res;
    res.found = false;
    res.profit = 0.0;
    res.ops = 0;
    res.timeMs = 0.0;

    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

    int n = g.size();
    HopSearch hs;
    hs.g = &g;
    hs.k = k;
    hs.bestSum = -1e-12;                             
    hs.ops = 0;
    hs.onPath.assign(n, false);

    for (int s = 0; s < n; s++) {
        hs.start = s;
        hs.path.clear();
        hs.path.push_back(s);
        hs.onPath[s] = true;
        hopDfs(hs, s, 0.0);
        hs.onPath[s] = false;
    }

    res.ops = hs.ops;
    if (!hs.bestCycle.empty()) {
        res.found = true;
        res.cycle = hs.bestCycle;
        res.profit = cycleProfit(g, hs.bestCycle);
    }

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(end - start).count();
    return res;
}


// 2) Top-k cycles


struct DisabledEdge {
    int u;
    int idx;      // position of the edge inside adjacencyList[u]
};


static vector<int> canonical(const vector<int>& cyc) {
    size_t minPos = 0;
    for (size_t i = 1; i < cyc.size(); i++) {
        if (cyc[i] < cyc[minPos]) minPos = i;
    }
    vector<int> out;
    for (size_t i = 0; i < cyc.size(); i++) out.push_back(cyc[(minPos + i) % cyc.size()]);
    return out;
}


static int findEdgeIndex(const Graph& g, int u, int v) {
    const vector<Edge>& edges = g.edgesOf(u);
    int best = -1;
    for (size_t i = 0; i < edges.size(); i++) {
        if (edges[i].to == v && edges[i].active) {
            if (best == -1 || edges[i].weight < edges[best].weight) best = i;
        }
    }
    return best;
}

static bool profitHigher(const Result& a, const Result& b) {
    return a.profit > b.profit;
}

vector<Result> topKCycles(Graph& g, int k, Result (*algo)(const Graph&)) {
    vector<Result> found;
    vector<vector<int> > seen;                       
    vector<vector<DisabledEdge> > pending;           
    set<vector<pair<int, int> > > visited;           

    pending.push_back(vector<DisabledEdge>());       
    visited.insert(vector<pair<int, int> >());
    size_t next = 0;
    int runs = 0;
    const int MAX_RUNS = 2000;                       

    while (next < pending.size() && runs < MAX_RUNS) {
        vector<DisabledEdge> state = pending[next];
        next++;

        for (size_t i = 0; i < state.size(); i++) g.setEdgeActive(state[i].u, state[i].idx, false);
        Result r = algo(g);
        runs++;
        for (size_t i = 0; i < state.size(); i++) g.setEdgeActive(state[i].u, state[i].idx, true);

        if (!r.found) continue;

        vector<int> canon = canonical(r.cycle);
        bool isNew = true;
        for (size_t i = 0; i < seen.size(); i++) {
            if (seen[i] == canon) { isNew = false; break; }
        }
        if (isNew) {
            seen.push_back(canon);
            r.cycle = canon;
            found.push_back(r);
        }

       
        for (size_t i = 0; i < canon.size(); i++) {
            int u = canon[i];
            int v = canon[(i + 1) % canon.size()];

           
            for (size_t j = 0; j < state.size(); j++) g.setEdgeActive(state[j].u, state[j].idx, false);
            int idx = findEdgeIndex(g, u, v);
            for (size_t j = 0; j < state.size(); j++) g.setEdgeActive(state[j].u, state[j].idx, true);

            if (idx == -1) continue;
            DisabledEdge d;
            d.u = u;
            d.idx = idx;
            vector<DisabledEdge> ns = state;
            ns.push_back(d);

           
            vector<pair<int, int> > key;
            for (size_t j = 0; j < ns.size(); j++) key.push_back(make_pair(ns[j].u, ns[j].idx));
            sort(key.begin(), key.end());
            if (visited.count(key) == 0 && pending.size() < 50000) {
                visited.insert(key);
                pending.push_back(ns);
            }
        }
    }

    sort(found.begin(), found.end(), profitHigher);
    if ((int)found.size() > k) found.resize(k);
    return found;
}
