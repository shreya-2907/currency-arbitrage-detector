#ifndef FEATURES_H
#define FEATURES_H

#include <vector>
#include "graph.h"
#include "algo.h"
using namespace std;

// Aisa best arbitrage jisme sabse zyada k trades (edges) hon.
Result hopLimited(const Graph& g, int k);
// Up to k different profitable cycles, best first
// 'algo' is the detection algorithm used internally (bellmanFord, spfa or floydWarshall).
vector<Result> topKCycles(Graph& g, int k, Result (*algo)(const Graph&));

#endif