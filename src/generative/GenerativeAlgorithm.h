#ifndef VECTORFORGE_GENERATIVEALGORITHM_H
#define VECTORFORGE_GENERATIVEALGORITHM_H

#include "../model/Shape.h"
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>

namespace VectorForge {

enum class ParamType {
    Int,
    Double,
    Bool,
    Color
};

struct ParameterInfo {
    std::string name;
    std::string label;
    ParamType type;
    double minValue{0.0};
    double maxValue{100.0};
    double defaultValue{10.0};
    double step{1.0};
    QColor defaultColor{Qt::white};
};

using ParameterMap = std::map<std::string, std::variant<int, double, bool, QColor>>;

class IGenerativeAlgorithm {
public:
    virtual ~IGenerativeAlgorithm() = default;

    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual std::vector<ParameterInfo> parameters() const = 0;
    virtual std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) = 0;
};

} // namespace VectorForge

#endif // VECTORFORGE_GENERATIVEALGORITHM_H
