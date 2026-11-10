#include <iostream>
#include <iomanip>
#include "graph.h"
#include "algo.h"
using namespace std;

int main() {
    Graph g;

    if (!g.loadRatesFromFile("data/rates.txt")) {
        cout << "Could not open data/rates.txt" << endl;
        return 1;
    }

    g.displayGraph();
    cout << endl;

    cout << "Fee 0%: ";
    printResult(g, bellmanFord(g));

    g.setFee(0.002);
    cout << "Fee 0.2%: ";
    printResult(g, bellmanFord(g));

    g.setFee(0.01);
    cout << "Fee 1%: ";
    printResult(g, bellmanFord(g));

    g.setFee(0.0);
    double be = breakEvenFee(g);
    cout << endl << "Break-even fee: " << fixed << setprecision(4) << be * 100 << "% per trade" << endl;

    return 0;
}