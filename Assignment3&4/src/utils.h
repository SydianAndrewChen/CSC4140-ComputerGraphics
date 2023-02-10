#ifndef UTILS_H
#define UTILS_H

#define EPSILON 1e-3
#define VALID_BARY(b0, b1, b2, thresh) ((b0 > -thresh) && (b0 < 1. + thresh) && (b1 > -thresh) && (b1 < 1. + thresh) && (b2 > -thresh) && (b2 < 1. + thresh))

#endif