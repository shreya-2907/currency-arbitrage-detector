#include <iostream>
#include "graph.h"
using namespace std;

int main() {
    Graph g;

    if (!g.loadRatesFromFile("data/rates.txt")) {
        cout << "Could not open data/rates.txt" << endl;
        return 1;
    }

    g.displayGraph();

    // fee test: weight badalna chahiye
    cout << "weight (no fee): " << g.edgesOf(0)[0].weight << endl;
    g.setFee(0.01);
    cout << "weight (1% fee): " << g.edgesOf(0)[0].weight << endl;

    return 0;
}