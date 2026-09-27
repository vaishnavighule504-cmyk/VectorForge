#ifndef VECTORFORGE_MANDALAGENERATOR_H
#define VECTORFORGE_MANDALAGENERATOR_H

#include "GenerativeAlgorithm.h"

namespace VectorForge {

class MandalaGenerator : public IGenerativeAlgorithm {
public:
    std::string name() const override { return "Sacred Symmetry Mandala"; }
    std::string description() const override { return "Rotational N-fold symmetry rosette with concentric rings and geometric petals."; }
    std::vector<ParameterInfo> parameters() const override;
    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;
};

} // namespace VectorForge

#endif // VECTORFORGE_MANDALAGENERATOR_H
