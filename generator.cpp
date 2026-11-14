#include "generator.h"
#include <random>
#include <string>
#include <vector>


void buildMarket(Graph& g, int n, double density, double spread,
                 int plantLen, double bonus, unsigned seed) {
    mt19937 rng(seed);
    uniform_real_distribution<double> unit(0.0, 1.0);

    vector<double> price(n);
    for (int i = 0; i < n; i++) {
        g.addCurrency("C" + to_string(i));
        price[i] = 1.0 + unit(rng) * 999.0;             // between 1 and 1000
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (unit(rng) < density) {
                g.addExchangeRateIdx(i, j, price[i] / price[j] * (1.0 - spread));
            }
        }
    }

    if (plantLen >= 2 && plantLen <= n) {
        vector<int> nodes;                              
        while ((int)nodes.size() < plantLen) {
            int c = (int)(unit(rng) * n);
            if (c >= n) c = n - 1;
            bool used = false;
            for (size_t k = 0; k < nodes.size(); k++) {
                if (nodes[k] == c) used = true;
            }
            if (!used) nodes.push_back(c);
        }

        for (int i = 0; i < plantLen; i++) {
            int u = nodes[i];
            int v = nodes[(i + 1) % plantLen];
            // also adds the edge if it does not exist
            g.updateRate(u, v, price[u] / price[v] * (1.0 + bonus));
        }
    }
}