#include "GenerativeEngine.h"
#include "LSystemGenerator.h"
#include "SpirographGenerator.h"
#include "FractalTreeGenerator.h"
#include "RoseCurveGenerator.h"
#include "FlowFieldGenerator.h"
#include "MandalaGenerator.h"

namespace VectorForge {

GenerativeEngine::GenerativeEngine() {
    m_algorithms.push_back(std::make_shared<LSystemGenerator>());
    m_algorithms.push_back(std::make_shared<SpirographGenerator>());
    m_algorithms.push_back(std::make_shared<FractalTreeGenerator>());
    m_algorithms.push_back(std::make_shared<RoseCurveGenerator>());
    m_algorithms.push_back(std::make_shared<FlowFieldGenerator>());
    m_algorithms.push_back(std::make_shared<MandalaGenerator>());
}

std::shared_ptr<IGenerativeAlgorithm> GenerativeEngine::findAlgorithm(const std::string& name) const {
    for (const auto& algo : m_algorithms) {
        if (algo && algo->name() == name) {
            return algo;
        }
    }
    return nullptr;
}

std::vector<std::string> GenerativeEngine::algorithmNames() const {
    std::vector<std::string> names;
    for (const auto& algo : m_algorithms) {
        if (algo) {
            names.push_back(algo->name());
        }
    }
    return names;
}

std::vector<std::shared_ptr<Shape>> GenerativeEngine::generate(const std::string& algorithmName, const ParameterMap& params, const QPointF& origin) {
    auto algo = findAlgorithm(algorithmName);
    if (algo) {
        return algo->generate(params, origin);
    }
    return {};
}

} // namespace VectorForge
