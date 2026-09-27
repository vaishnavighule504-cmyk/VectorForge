#include "FlowFieldGenerator.h"
#include "../utils/PerlinNoise.h"
#include "../model/Polygon.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> FlowFieldGenerator::parameters() const {
    return {
        {"lineCount", "Line Count", ParamType::Int, 10.0, 300.0, 80.0, 5.0},
        {"lineSteps", "Line Length (Steps)", ParamType::Int, 10.0, 200.0, 50.0, 5.0},
        {"stepSize", "Step Distance", ParamType::Double, 1.0, 20.0, 5.0, 0.5},
        {"noiseScale", "Noise Frequency", ParamType::Double, 0.001, 0.05, 0.008, 0.001},
        {"strokeColor", "Streamline Color", ParamType::Color, 0, 0, 0, 0, QColor(255, 170, 0)},
        {"strokeWidth", "Stroke Width", ParamType::Double, 0.5, 8.0, 1.5, 0.5}
    };
}

std::vector<std::shared_ptr<Shape>> FlowFieldGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    int lineCount = std::get<int>(params.at("lineCount"));
    int steps = std::get<int>(params.at("lineSteps"));
    double stepDist = std::get<double>(params.at("stepSize"));
    double noiseScale = std::get<double>(params.at("noiseScale"));
    QColor stroke = std::get<QColor>(params.at("strokeColor"));
    double width = std::get<double>(params.at("strokeWidth"));

    PerlinNoise perlin(2026);
    std::vector<std::shared_ptr<Shape>> result;

    double gridRadius = 250.0;
    for (int i = 0; i < lineCount; ++i) {
        double angle = (2.0 * M_PI * i) / lineCount;
        double r = (static_cast<double>(i % 10) / 10.0) * gridRadius;
        QPointF curr(origin.x() + r * std::cos(angle), origin.y() + r * std::sin(angle));

        QPolygonF linePts;
        linePts.append(curr);

        for (int s = 0; s < steps; ++s) {
            double n = perlin.noise2D(curr.x() * noiseScale, curr.y() * noiseScale);
            double flowAngle = n * 4.0 * M_PI; // Vector direction

            curr.setX(curr.x() + stepDist * std::cos(flowAngle));
            curr.setY(curr.y() + stepDist * std::sin(flowAngle));
            linePts.append(curr);
        }

        auto stream = std::make_shared<Polygon>(linePts, false);
        stream->setName("Flow Streamline");
        stream->setStrokeColor(stroke);
        stream->setStrokeWidth(width);
        stream->setOpacity(0.8);
        result.push_back(stream);
    }

    return result;
}

} // namespace VectorForge
