#ifndef VECTORFORGE_FLOWFIELDGENERATOR_H
#define VECTORFORGE_FLOWFIELDGENERATOR_H

#include "GenerativeAlgorithm.h"

namespace VectorForge {

class FlowFieldGenerator : public IGenerativeAlgorithm {
public:
    std::string name() const override { return "Perlin Noise Flow Field"; }
    std::string description() const override { return "Streamlines integrated through a 2D Perlin noise vector field."; }
    std::vector<ParameterInfo> parameters() const override;
    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;
};

} // namespace VectorForge

#endif // VECTORFORGE_FLOWFIELDGENERATOR_H
