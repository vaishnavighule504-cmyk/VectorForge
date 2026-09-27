#ifndef VECTORFORGE_GENERATIVEENGINE_H
#define VECTORFORGE_GENERATIVEENGINE_H

#include "GenerativeAlgorithm.h"
#include <vector>
#include <memory>
#include <string>

namespace VectorForge {

class GenerativeEngine {
public:
    GenerativeEngine();
    ~GenerativeEngine() = default;

    const std::vector<std::shared_ptr<IGenerativeAlgorithm>>& algorithms() const { return m_algorithms; }
    
    std::shared_ptr<IGenerativeAlgorithm> findAlgorithm(const std::string& name) const;
    std::vector<std::string> algorithmNames() const;

    std::vector<std::shared_ptr<Shape>> generate(
        const std::string& algorithmName, 
        const ParameterMap& params, 
        const QPointF& origin
    );

private:
    std::vector<std::shared_ptr<IGenerativeAlgorithm>> m_algorithms;
};

} // namespace VectorForge

#endif // VECTORFORGE_GENERATIVEENGINE_H
