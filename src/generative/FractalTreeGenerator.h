#ifndef VECTORFORGE_FRACTALTREEGENERATOR_H
#define VECTORFORGE_FRACTALTREEGENERATOR_H

#include "GenerativeAlgorithm.h"

namespace VectorForge {

class FractalTreeGenerator : public IGenerativeAlgorithm {
public:
    std::string name() const override { return "Fractal Branching Tree"; }
    std::string description() const override { return "Recursive geometric tree with customizable angle, decay, and depth."; }
    std::vector<ParameterInfo> parameters() const override;
    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;

private:
    void generateBranch(std::vector<std::shared_ptr<Shape>>& result, const QPointF& start, double length, double angleRad, int depth, double angleSpread, double lengthDecay, const QColor& stroke, double strokeWidth);
};

} // namespace VectorForge

#endif // VECTORFORGE_FRACTALTREEGENERATOR_H
