#include "graph.h"
#include <cmath>
#include <fstream>

Graph::Graph() {
    fee = 0.0;
}

// ---------- private helpers ----------

int Graph::findCurrency(const string& name) const {
    return currencyIndex.get(name);     // apna HashMap: nahi mila to -1
}

double Graph::computeWeight(double rate) const {
    return -log(rate * (1.0 - fee));
}

// ---------- building the graph ----------

int Graph::addCurrency(const string& name) {
    int idx = findCurrency(name);
    if (idx != -1) return idx;          // pehle se hai

    idx = currencies.size();
    currencies.push_back(name);
    adjacencyList.push_back(vector<Edge>());
    currencyIndex.put(name, idx);
    return idx;
}

void Graph::addExchangeRate(const string& from, const string& to, double rate) {
    int u = addCurrency(from);          // naya ho to bana dega, warna purana index
    int v = addCurrency(to);
    addExchangeRateIdx(u, v, rate);
}

void Graph::addExchangeRateIdx(int u, int v, double rate) {
    if (u < 0 || v < 0 || u >= (int)adjacencyList.size() || v >= (int)adjacencyList.size()) return;
    if (rate <= 0) return;              // log(<=0) valid nahi hota

    Edge e;
    e.to = v;
    e.rate = rate;
    e.weight = computeWeight(rate);
    e.active = true;

    adjacencyList[u].push_back(e);
}

// u -> v ka rate badlo (weight bhi dobara nikalo). Edge pehle se na ho to naya jodo.
bool Graph::updateRate(int u, int v, double rate) {
    if (u < 0 || v < 0 || u >= (int)adjacencyList.size() || v >= (int)adjacencyList.size()) return false;
    if (rate <= 0) return false;

    for (size_t i = 0; i < adjacencyList[u].size(); i++) {
        if (adjacencyList[u][i].to == v) {
            adjacencyList[u][i].rate = rate;
            adjacencyList[u][i].weight = computeWeight(rate);
            return true;
        }
    }
    addExchangeRateIdx(u, v, rate);
    return true;
}

bool Graph::updateRateByName(const string& from, const string& to, double rate) {
    int u = findCurrency(from);
    int v = findCurrency(to);
    if (u == -1 || v == -1 || u == v) return false;
    return updateRate(u, v, rate);
}

bool Graph::loadRatesFromFile(const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    string from, to;
    double rate;
    while (file >> from >> to >> rate) {    // har line: USD EUR 0.92
        if (rate > 0) {                     // log(<=0) valid nahi hota
            addExchangeRate(from, to, rate);
        }
    }

    file.close();
    return true;
}

// ---------- display ----------

void Graph::displayGraph() const {
    int totalEdges = 0;

    for (int u = 0; u < (int)adjacencyList.size(); u++) {
        for (int i = 0; i < (int)adjacencyList[u].size(); i++) {
            const Edge& e = adjacencyList[u][i];
            cout << currencies[u] << " -> " << currencies[e.to]
                 << " : " << e.rate << endl;
            totalEdges++;
        }
    }

    cout << "Currencies: " << currencies.size() << endl;
    cout << "Edges: " << totalEdges << endl;
}

// ---------- fee / edge control ----------

void Graph::setFee(double f) {
    if (f < 0.0 || f >= 1.0) return;        // galat fee ignore

    fee = f;
    for (int u = 0; u < (int)adjacencyList.size(); u++) {
        for (int i = 0; i < (int)adjacencyList[u].size(); i++) {
            adjacencyList[u][i].weight = computeWeight(adjacencyList[u][i].rate);
        }
    }
}

double Graph::getFee() const {
    return fee;
}

void Graph::setEdgeActive(int u, int edgeIdx, bool a) {
    if (u < 0 || u >= (int)adjacencyList.size()) return;
    if (edgeIdx < 0 || edgeIdx >= (int)adjacencyList[u].size()) return;
    adjacencyList[u][edgeIdx].active = a;
}

// ---------- getters (algorithms inhi se graph padhenge) ----------

int Graph::size() const {
    return currencies.size();
}

string Graph::nameOf(int i) const {
    return currencies[i];
}

const vector<Edge>& Graph::edgesOf(int u) const {
    return adjacencyList[u];
}

double Graph::rateBetween(int u, int v) const {
    for (int i = 0; i < (int)adjacencyList[u].size(); i++) {
        if (adjacencyList[u][i].to == v) return adjacencyList[u][i].rate;
    }
    return 0.0;
}