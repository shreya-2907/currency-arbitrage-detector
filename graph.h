#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "hashmap.h"       // custom hash map (chaining)
using namespace std;

struct Edge {
    int to;         // destination node index
    double rate;    // original exchange rate
    double weight;  // -ln(rate * (1 - fee))
    bool active;    // top-k cycles ke liye: false = edge temporarily band
};

class Graph {
private:
    vector<string> currencies;
    vector<vector<Edge> > adjacencyList;
    StringIntMap currencyIndex;                   // currency naam -> index 
    double fee;                                   // per-trade fee, 0.0 = no fee

    int findCurrency(const string& name) const;  
    double computeWeight(double rate) const;

public:
    Graph();

    int addCurrency(const string& name);
    void addExchangeRate(const string& from, const string& to, double rate);
    void addExchangeRateIdx(int u, int v, double rate);      
    bool updateRate(int u, int v, double rate);              
    bool updateRateByName(const string& from, const string& to, double rate);
    bool loadRatesFromFile(const string& filename);
    void displayGraph() const;

    void setFee(double f);                       
    double getFee() const;
    void setEdgeActive(int u, int edgeIdx, bool a);

    int size() const;
    string nameOf(int i) const;
    const vector<Edge>& edgesOf(int u) const;
    double rateBetween(int u, int v) const;     
};

#endif