#ifndef GENERATOR_H
#define GENERATOR_H

#include "graph.h"

/// Builds a test market.
//   n -number of currencies
//   density-probability that an edge (i -> j) exists (1.0 = dense, 0.1 = sparse)
//   spread-small loss on every trade (0.005 = 0.5%), so the base market has NO arbitrage
//   plantLen-0 = no arbitrage; 3 = hide a profitable cycle through 3 currencies
//   bonus-extra gain on every edge of the planted cycle (0.02 = 2%)
//   seed-the same seed always gives the same market (reproducible results)
void buildMarket(Graph& g, int n, double density, double spread,
                 int plantLen, double bonus, unsigned seed);

#endif