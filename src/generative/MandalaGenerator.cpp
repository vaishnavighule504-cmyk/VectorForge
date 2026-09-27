#include "MandalaGenerator.h"
#include "../model/Circle.h"
#include "../model/Polygon.h"
#include "../model/GroupShape.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> MandalaGenerator::parameters() const {
    return {
        {"symmetry", "Symmetry Folds (N)", ParamType::Int, 3.0, 24.0, 12.0, 1.0},
        {"rings", "Concentric Rings", ParamType::Int, 1.0, 10.0, 4.0, 1.0},
        {"maxRadius", "Outer Radius", ParamType::Double, 50.0, 400.0, 220.0, 10.0},
        {"strokeColor", "Mandala Line Color", ParamType::Color, 0, 0, 0, 0, QColor(180, 100, 255)},
        {"strokeWidth", "Stroke Width", ParamType::Double, 0.5, 10.0, 2.0, 0.5}
    };
}

std::vector<std::shared_ptr<Shape>> MandalaGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    int symmetry = std::get<int>(params.at("symmetry"));
    int rings = std::get<int>(params.at("rings"));
    double maxRadius = std::get<double>(params.at("maxRadius"));
    QColor stroke = std::get<QColor>(params.at("strokeColor"));
    double width = std::get<double>(params.at("strokeWidth"));

    std::vector<std::shared_ptr<Shape>> shapes;
    double ringStep = maxRadius / rings;

    // Concentric guide circles
    for (int r = 1; r <= rings; ++r) {
        double radius = r * ringStep;
        auto circle = std::make_shared<Circle>(origin, radius);
        circle->setName("Ring Guide");
        circle->setStrokeColor(stroke);
        circle->setFillColor(Qt::transparent);
        circle->setStrokeWidth(width * 0.75);
        circle->setOpacity(0.6);
        shapes.push_back(circle);

        // Petal diamonds in each ring
        double angleStep = 2.0 * M_PI / symmetry;
        for (int i = 0; i < symmetry; ++i) {
            double a1 = i * angleStep;
            double aMid = a1 + angleStep * 0.5;
            double a2 = (i + 1) * angleStep;

            QPointF pInner(origin.x() + (radius - ringStep * 0.5) * std::cos(aMid),
                           origin.y() + (radius - ringStep * 0.5) * std::sin(aMid));
            QPointF pOuter(origin.x() + (radius + ringStep * 0.4) * std::cos(aMid),
                           origin.y() + (radius + ringStep * 0.4) * std::sin(aMid));
            QPointF pLeft(origin.x() + radius * std::cos(a1),
                          origin.y() + radius * std::sin(a1));
            QPointF pRight(origin.x() + radius * std::cos(a2),
                           origin.y() + radius * std::sin(a2));

            QPolygonF diamond;
            diamond.append(pInner);
            diamond.append(pLeft);
            diamond.append(pOuter);
            diamond.append(pRight);

            auto petal = std::make_shared<Polygon>(diamond, true);
            petal->setName("Mandala Petal");
            petal->setStrokeColor(stroke);
            petal->setFillColor(QColor(stroke.red(), stroke.green(), stroke.blue(), 25));
            petal->setStrokeWidth(width);
            shapes.push_back(petal);
        }
    }

    return shapes;
}

} // namespace VectorForge
