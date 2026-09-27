#include "SpirographGenerator.h"
#include "../model/Polygon.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> SpirographGenerator::parameters() const {
    return {
        {"R", "Outer Radius (R)", ParamType::Double, 10.0, 300.0, 150.0, 5.0},
        {"r", "Inner Radius (r)", ParamType::Double, 5.0, 200.0, 63.0, 1.0},
        {"d", "Pen Distance (d)", ParamType::Double, 5.0, 200.0, 80.0, 1.0},
        {"revolutions", "Revolutions", ParamType::Int, 1.0, 50.0, 10.0, 1.0},
        {"strokeColor", "Stroke Color", ParamType::Color, 0, 0, 0, 0, QColor(0, 230, 255)},
        {"strokeWidth", "Stroke Width", ParamType::Double, 0.5, 10.0, 2.0, 0.5}
    };
}

std::vector<std::shared_ptr<Shape>> SpirographGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    double R = std::get<double>(params.at("R"));
    double r = std::get<double>(params.at("r"));
    double d = std::get<double>(params.at("d"));
    int revs = std::get<int>(params.at("revolutions"));
    QColor stroke = std::get<QColor>(params.at("strokeColor"));
    double width = std::get<double>(params.at("strokeWidth"));

    QPolygonF pts;
    double maxTheta = revs * 2.0 * M_PI;
    double step = 0.02;

    for (double t = 0; t <= maxTheta; t += step) {
        double x = (R - r) * std::cos(t) + d * std::cos((R - r) * t / r);
        double y = (R - r) * std::sin(t) - d * std::sin((R - r) * t / r);
        pts.append(QPointF(origin.x() + x, origin.y() + y));
    }

    auto poly = std::make_shared<Polygon>(pts, true);
    poly->setName("Spirograph Curve");
    poly->setStrokeColor(stroke);
    poly->setFillColor(Qt::transparent);
    poly->setStrokeWidth(width);

    return {poly};
}

} // namespace VectorForge
