#include "features.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <set>
#include <utility>

// ---------------------------------------------------------------
// 1) Hop-limited arbitrage
//
// Sawal: "sabse zyada k trades me best arbitrage kaunsa hai?"
// Tarika: har start node se depth-limited DFS. Raaste me koi node dobara nahi aata
// (simple cycle), aur sirf wahi nodes jinka index start se bada ho (taaki ek cycle
// ki rotations dobara na ginen). Weight ka sum sabse chhota (negative) = sabse zyada profit.
//
// Note: pehle Bellman-Ford jaisa dp[t][v] try kiya tha, par wo "do cycles ka mix"
// (node repeat hone wala walk) de deta hai, simple cycle nahi. Isliye ye DFS exact hai.
// Complexity: O(V * d^k) (d = ek node se nikalne wale edges), isliye k chhota rakho.
// ---------------------------------------------------------------

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
    int edgesUsed = (int)hs.path.size() - 1;          // abhi tak kitne trades hue

    for (size_t i = 0; i < edges.size(); i++) {
        if (!edges[i].active) continue;
        hs.ops++;
        int v = edges[i].to;
        double nsum = sum + edges[i].weight;

        if (v == hs.start) {
            if (edgesUsed + 1 >= 2 && nsum < hs.bestSum - 1e-12) {   // cycle band hua
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
    hs.bestSum = -1e-12;                              // sirf negative cycle chahiye
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

// ---------------------------------------------------------------
// 2) Top-k cycles
//
// Idea: ek cycle mila to uske ek edge ko band (active = false) karke dobara
// detection chalao. Cycle ke har edge ke liye aisa karo, naye cycles ko list me
// jodo aur unpe bhi yahi dohrao. Last me profit ke hisaab se sort.
// Kyun kaam karta hai: koi bhi doosra cycle C' purane cycle C ke saare edges nahi
// rakh sakta (warna wo C hi hota), yaani C' C ka kam se kam ek edge nahi rakhta, aur
// wo "us edge ko band karne wale" state me zinda rehta hai. Isliye time kaafi ho
// to saare cycles milte hain. Par time/runs limited hain (MAX_RUNS), isliye
// ye BEST-EFFORT (heuristic) hai: top-1 hamesha sabse achha ho, ye guarantee nahi.
// ---------------------------------------------------------------

struct DisabledEdge {
    int u;
    int idx;      // adjacencyList[u] me edge ka number
};

// Cycle ko ek standard roop do (sabse chhota node pehle), taaki rotations ek hi ginein
static vector<int> canonical(const vector<int>& cyc) {
    size_t minPos = 0;
    for (size_t i = 1; i < cyc.size(); i++) {
        if (cyc[i] < cyc[minPos]) minPos = i;
    }
    vector<int> out;
    for (size_t i = 0; i < cyc.size(); i++) out.push_back(cyc[(minPos + i) % cyc.size()]);
    return out;
}

// u -> v ka wo active edge dhoondo jise algorithm use karega (sabse kam weight)
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
    vector<vector<int> > seen;                       // mil chuke cycles (canonical roop me)
    vector<vector<DisabledEdge> > pending;           // jin edge-sets ko try karna baaki hai
    set<vector<pair<int, int> > > visited;           // jo band-edge sets pehle try ho chuke

    pending.push_back(vector<DisabledEdge>());       // pehle kuch band nahi
    visited.insert(vector<pair<int, int> >());
    size_t next = 0;
    int runs = 0;
    const int MAX_RUNS = 2000;                        // infinite/bahut lamba chalne se bachao

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

        // is cycle ke har edge ko (purani band list ke upar) band karke naya state banao
        for (size_t i = 0; i < canon.size(); i++) {
            int u = canon[i];
            int v = canon[(i + 1) % canon.size()];

            // edge ka number us waqt dekho jab state ke edges band hon
            for (size_t j = 0; j < state.size(); j++) g.setEdgeActive(state[j].u, state[j].idx, false);
            int idx = findEdgeIndex(g, u, v);
            for (size_t j = 0; j < state.size(); j++) g.setEdgeActive(state[j].u, state[j].idx, true);

            if (idx == -1) continue;
            DisabledEdge d;
            d.u = u;
            d.idx = idx;
            vector<DisabledEdge> ns = state;
            ns.push_back(d);

            // same edges ka set kisi aur order me pehle aa chuka ho to dobara mat jodo
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