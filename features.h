#ifndef FEATURES_H
#define FEATURES_H

#include <vector>
#include "graph.h"
#include "algo.h"
using namespace std;

// Aisa best arbitrage jisme sabse zyada k trades (edges) hon.
Result hopLimited(const Graph& g, int k);

// Profit ke hisaab se top-k alag-alag arbitrage cycles (heuristic).
// algo = kaunsa detection algorithm andar use hoga (bellmanFord, spfa ya floydWarshall).
vector<Result> topKCycles(Graph& g, int k, Result (*algo)(const Graph&));

#endif