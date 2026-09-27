#ifndef VECTORFORGE_SPIROGRAPHGENERATOR_H
#define VECTORFORGE_SPIROGRAPHGENERATOR_H

#include "GenerativeAlgorithm.h"

namespace VectorForge {

class SpirographGenerator : public IGenerativeAlgorithm {
public:
    std::string name() const override { return "Spirograph (Hypotrochoid)"; }
    std::string description() const override { return "Generates mathematical hypotrochoid and epitrochoid spirograph curves."; }
    std::vector<ParameterInfo> parameters() const override;
    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;
};

} // namespace VectorForge

#endif // VECTORFORGE_SPIROGRAPHGENERATOR_H
