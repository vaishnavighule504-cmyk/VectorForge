#ifndef VECTORFORGE_ROSECURVEGENERATOR_H
#define VECTORFORGE_ROSECURVEGENERATOR_H

#include "GenerativeAlgorithm.h"

namespace VectorForge {

class RoseCurveGenerator : public IGenerativeAlgorithm {
public:
    std::string name() const override { return "Rhodonea Rose Curve"; }
    std::string description() const override { return "Parametric mathematical rose curves (r = a * cos(k * theta))."; }
    std::vector<ParameterInfo> parameters() const override;
    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;
};

} // namespace VectorForge

#endif // VECTORFORGE_ROSECURVEGENERATOR_H
