#ifndef GENERATOR_H
#define GENERATOR_H

#include "graph.h"

// Test market banata hai.
//   n        : kitni currencies
//   density  : har (i -> j) jodi ke liye edge hone ki probability (1.0 = dense, 0.1 = sparse)
//   spread   : har trade pe chhota nuksaan (0.005 = 0.5%), isliye base market me arbitrage NAHI banta
//   plantLen : 0 = koi arbitrage nahi; 3 = 3 currencies ka profitable cycle chhupao
//   bonus    : planted cycle ke har edge pe extra faayda (0.02 = 2%)
//   seed     : same seed = same market (results repeat ho sakte hain)
void buildMarket(Graph& g, int n, double density, double spread,
                 int plantLen, double bonus, unsigned seed);

#endif