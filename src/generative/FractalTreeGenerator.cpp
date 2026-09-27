#include "FractalTreeGenerator.h"
#include "../model/Polygon.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> FractalTreeGenerator::parameters() const {
    return {
        {"depth", "Recursion Depth", ParamType::Int, 1.0, 12.0, 8.0, 1.0},
        {"initialLength", "Trunk Length", ParamType::Double, 20.0, 300.0, 120.0, 5.0},
        {"branchAngle", "Branch Angle (deg)", ParamType::Double, 5.0, 90.0, 25.0, 1.0},
        {"lengthDecay", "Decay Ratio", ParamType::Double, 0.4, 0.9, 0.72, 0.02},
        {"strokeColor", "Branch Color", ParamType::Color, 0, 0, 0, 0, QColor(40, 220, 120)},
        {"strokeWidth", "Trunk Width", ParamType::Double, 1.0, 15.0, 4.0, 0.5}
    };
}

void FractalTreeGenerator::generateBranch(std::vector<std::shared_ptr<Shape>>& result, const QPointF& start, double length, double angleRad, int depth, double angleSpreadRad, double lengthDecay, const QColor& stroke, double strokeWidth) {
    if (depth <= 0 || length < 2.0) return;

    QPointF end(start.x() + length * std::cos(angleRad),
                start.y() + length * std::sin(angleRad));

    QPolygonF pts;
    pts.append(start);
    pts.append(end);

    auto branch = std::make_shared<Polygon>(pts, false);
    branch->setName("Branch");
    branch->setStrokeColor(stroke);
    branch->setStrokeWidth(std::max(1.0, strokeWidth * (depth / 8.0)));
    result.push_back(branch);

    generateBranch(result, end, length * lengthDecay, angleRad - angleSpreadRad, depth - 1, angleSpreadRad, lengthDecay, stroke, strokeWidth);
    generateBranch(result, end, length * lengthDecay, angleRad + angleSpreadRad, depth - 1, angleSpreadRad, lengthDecay, stroke, strokeWidth);
}

std::vector<std::shared_ptr<Shape>> FractalTreeGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    int depth = std::get<int>(params.at("depth"));
    double length = std::get<double>(params.at("initialLength"));
    double angleDeg = std::get<double>(params.at("branchAngle"));
    double decay = std::get<double>(params.at("lengthDecay"));
    QColor stroke = std::get<QColor>(params.at("strokeColor"));
    double width = std::get<double>(params.at("strokeWidth"));

    std::vector<std::shared_ptr<Shape>> result;
    double angleSpreadRad = angleDeg * M_PI / 180.0;
    double initialAngleRad = -M_PI / 2.0; // Pointing upwards

    generateBranch(result, origin, length, initialAngleRad, depth, angleSpreadRad, decay, stroke, width);
    return result;
}

} // namespace VectorForge
