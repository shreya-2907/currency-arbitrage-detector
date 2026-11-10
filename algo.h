#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include "graph.h"
using namespace std;

struct Result {
    bool found;
    vector<int> cycle;
    double profit;
    long long ops;
    double timeMs;
};

Result bellmanFord(const Graph& g);
Result spfa(const Graph& g);
Result floydWarshall(const Graph& g);
double cycleProfit(const Graph& g, const vector<int>& cycle);
void printResult(const Graph& g, const Result& r);
double breakEvenFee(Graph& g);

#endif