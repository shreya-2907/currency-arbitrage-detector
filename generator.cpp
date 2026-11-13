#include "generator.h"
#include <random>
#include <string>
#include <vector>

// Idea: har currency ka ek "asli price" hota hai. i se j ka rate = price[i] / price[j].
// Aise rates me kisi bhi cycle ka product exactly 1 hota hai (kuch fayda nahi).
// Spread se har rate thoda ghatate hain, isliye har cycle ka product < 1 (arbitrage nahi).
// Phir chahein to ek cycle ke rates jaanbujh kar badhake arbitrage "plant" karte hain.
void buildMarket(Graph& g, int n, double density, double spread,
                 int plantLen, double bonus, unsigned seed) {
    mt19937 rng(seed);
    uniform_real_distribution<double> unit(0.0, 1.0);

    vector<double> price(n);
    for (int i = 0; i < n; i++) {
        g.addCurrency("C" + to_string(i));
        price[i] = 1.0 + unit(rng) * 999.0;             // 1 se 1000 ke beech
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
        vector<int> nodes;                              // plantLen alag-alag random currencies
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
            // edge na ho (sparse graph) to ye jod bhi deta hai
            g.updateRate(u, v, price[u] / price[v] * (1.0 + bonus));
        }
    }
}