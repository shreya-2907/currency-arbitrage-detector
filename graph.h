#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "hashmap.h"       // apna HashMap (chaining)
using namespace std;

struct Edge {
    int to;         // destination node index
    double rate;    // original exchange rate
    double weight;  // -ln(rate * (1 - fee)), algorithms yahi use karte hain
    bool active;    // top-k cycles ke liye: false = edge temporarily band
};

class Graph {
private:
    vector<string> currencies;
    vector<vector<Edge> > adjacencyList;
    StringIntMap currencyIndex;                   // currency naam -> index (apna HashMap)
    double fee;                                   // per-trade fee, 0.0 = no fee

    int findCurrency(const string& name) const;   // -1 agar na mile
    double computeWeight(double rate) const;

public:
    Graph();

    int addCurrency(const string& name);
    void addExchangeRate(const string& from, const string& to, double rate);
    void addExchangeRateIdx(int u, int v, double rate);      // index se seedha (generator ke liye tez)
    bool updateRate(int u, int v, double rate);              // rate badlo, edge na ho to jodo
    bool updateRateByName(const string& from, const string& to, double rate);
    bool loadRatesFromFile(const string& filename);
    void displayGraph() const;

    void setFee(double f);                        // saare weights dobara compute
    double getFee() const;
    void setEdgeActive(int u, int edgeIdx, bool a);

    int size() const;
    string nameOf(int i) const;
    const vector<Edge>& edgesOf(int u) const;
    double rateBetween(int u, int v) const;       // 0 agar edge nahi hai
};

#endif