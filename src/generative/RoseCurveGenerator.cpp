#include "RoseCurveGenerator.h"
#include "../model/Polygon.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> RoseCurveGenerator::parameters() const {
    return {
        {"n", "Numerator (n)", ParamType::Int, 1.0, 20.0, 5.0, 1.0},
        {"d", "Denominator (d)", ParamType::Int, 1.0, 20.0, 8.0, 1.0},
        {"amplitude", "Radius (a)", ParamType::Double, 20.0, 400.0, 180.0, 5.0},
        {"strokeColor", "Petal Color", ParamType::Color, 0, 0, 0, 0, QColor(255, 80, 150)},
        {"strokeWidth", "Stroke Width", ParamType::Double, 0.5, 10.0, 2.0, 0.5}
    };
}

std::vector<std::shared_ptr<Shape>> RoseCurveGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    int n = std::get<int>(params.at("n"));
    int d = std::get<int>(params.at("d"));
    double a = std::get<double>(params.at("amplitude"));
    QColor stroke = std::get<QColor>(params.at("strokeColor"));
    double width = std::get<double>(params.at("strokeWidth"));

    double k = static_cast<double>(n) / static_cast<double>(d);
    double maxTheta = (n * d % 2 == 0) ? 2.0 * M_PI * d : M_PI * d;
    double step = 0.01;

    QPolygonF pts;
    for (double theta = 0; theta <= maxTheta; theta += step) {
        double r = a * std::cos(k * theta);
        double x = r * std::cos(theta);
        double y = r * std::sin(theta);
        pts.append(QPointF(origin.x() + x, origin.y() + y));
    }

    auto poly = std::make_shared<Polygon>(pts, true);
    poly->setName("Rose Curve");
    poly->setStrokeColor(stroke);
    poly->setFillColor(Qt::transparent);
    poly->setStrokeWidth(width);

    return {poly};
}

} // namespace VectorForge
