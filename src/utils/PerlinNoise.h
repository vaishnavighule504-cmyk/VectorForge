#ifndef VECTORFORGE_PERLINNOISE_H
#define VECTORFORGE_PERLINNOISE_H

#include <vector>
#include <numeric>
#include <random>

namespace VectorForge {

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 2026);
    double noise2D(double x, double y) const;

private:
    std::vector<int> p;
    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y);
};

} // namespace VectorForge

#endif // VECTORFORGE_PERLINNOISE_H
