#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include "graph.h"
using namespace std;

// Har algorithm yahi struct return karega, taaki menu aur benchmark sabko same tarah treat kar sake
struct Result {
    bool found;          // arbitrage mila ya nahi
    vector<int> cycle;   // nodes in order (last se wapas pehle pe jaata hai)
    double profit;       // 0.0127 = 1.27%
    long long ops;       // kitne edge checks hue
    double timeMs;       // kitna time laga (milliseconds)
};

Result bellmanFord(const Graph& g);
double cycleProfit(const Graph& g, const vector<int>& cycle);
void printResult(const Graph& g, const Result& r);
double breakEvenFee(Graph& g);

#endif