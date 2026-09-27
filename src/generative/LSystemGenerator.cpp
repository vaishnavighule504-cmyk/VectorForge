#include "LSystemGenerator.h"
#include "../model/GroupShape.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorForge {

std::vector<ParameterInfo> LSystemGenerator::parameters() const {
    return {
        {"preset", "Preset (0:Dragon, 1:Koch, 2:Sierpinski, 3:Fern, 4:Penrose)", ParamType::Int, 0, 4, 0, 1, Qt::white},
        {"depth", "Recursion Depth", ParamType::Int, 1, 12, 5, 1, Qt::white},
        {"stepLength", "Step Length", ParamType::Double, 2.0, 100.0, 12.0, 1.0, Qt::white},
        {"angle", "Turn Angle (Degrees)", ParamType::Double, 1.0, 180.0, 90.0, 0.5, Qt::white},
        {"symmetry", "Symmetry Count", ParamType::Int, 1, 24, 1, 1, Qt::white},
        {"strokeWidth", "Stroke Width", ParamType::Double, 0.5, 10.0, 1.5, 0.5, Qt::white},
        {"startColor", "Start Gradient Color", ParamType::Color, 0, 0, 0, 0, QColor(0, 230, 255)},
        {"endColor", "End Gradient Color", ParamType::Color, 0, 0, 0, 0, QColor(255, 0, 180)}
    };
}

std::string LSystemGenerator::expandLSystem(const std::string& axiom, const std::map<char, std::string>& rules, int iterations) {
    std::string current = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next = "";
        for (char c : current) {
            auto it = rules.find(c);
            if (it != rules.end()) {
                next += it->second;
            } else {
                next += c;
            }
        }
        current = next;
    }
    return current;
}

std::vector<std::shared_ptr<Shape>> LSystemGenerator::renderTurtle(
    const std::string& lsystemString,
    const QPointF& origin,
    double stepLength,
    double angleStepDegrees,
    int symmetryCount,
    const QColor& startColor,
    const QColor& endColor,
    double strokeWidth
) {
    std::vector<std::shared_ptr<Shape>> generatedShapes;

    if (symmetryCount < 1) symmetryCount = 1;
    double symmetryAngleStep = (360.0 / symmetryCount) * (M_PI / 180.0);

    size_t totalCommands = lsystemString.length();
    size_t processedCommands = 0;

    for (int sym = 0; sym < symmetryCount; ++sym) {
        double baseRotationAngle = sym * symmetryAngleStep;

        std::stack<TurtleState> stateStack;
        TurtleState current;
        current.position = origin;
        current.angleDegrees = (sym * (360.0 / symmetryCount)) - 90.0;
        current.color = startColor;
        current.strokeWidth = strokeWidth;

        QPolygonF currentPolyline;
        currentPolyline.append(current.position);

        for (char cmd : lsystemString) {
            processedCommands++;
            double progress = static_cast<double>(processedCommands) / (totalCommands * symmetryCount);

            // Interpolate color gradient
            int r = static_cast<int>(startColor.red() * (1.0 - progress) + endColor.red() * progress);
            int g = static_cast<int>(startColor.green() * (1.0 - progress) + endColor.green() * progress);
            int b = static_cast<int>(startColor.blue() * (1.0 - progress) + endColor.blue() * progress);
            QColor iterColor(r, g, b);

            switch (cmd) {
                case 'F':
                case 'G': {
                    double rad = current.angleDegrees * (M_PI / 180.0);
                    QPointF nextPos(
                        current.position.x() + stepLength * std::cos(rad),
                        current.position.y() + stepLength * std::sin(rad)
                    );
                    
                    currentPolyline.append(nextPos);
                    current.position = nextPos;
                    break;
                }
                case 'f': {
                    // Move forward without drawing line
                    if (currentPolyline.size() > 1) {
                        auto poly = std::make_shared<Polygon>(currentPolyline, false);
                        poly->setStrokeColor(iterColor);
                        poly->setFillColor(Qt::transparent);
                        poly->setStrokeWidth(strokeWidth);
                        generatedShapes.push_back(poly);
                    }
                    currentPolyline.clear();

                    double rad = current.angleDegrees * (M_PI / 180.0);
                    current.position = QPointF(
                        current.position.x() + stepLength * std::cos(rad),
                        current.position.y() + stepLength * std::sin(rad)
                    );
                    currentPolyline.append(current.position);
                    break;
                }
                case '+': {
                    current.angleDegrees += angleStepDegrees;
                    break;
                }
                case '-': {
                    current.angleDegrees -= angleStepDegrees;
                    break;
                }
                case '[': {
                    stateStack.push(current);
                    break;
                }
                case ']': {
                    if (currentPolyline.size() > 1) {
                        auto poly = std::make_shared<Polygon>(currentPolyline, false);
                        poly->setStrokeColor(iterColor);
                        poly->setFillColor(Qt::transparent);
                        poly->setStrokeWidth(strokeWidth);
                        generatedShapes.push_back(poly);
                    }
                    currentPolyline.clear();

                    if (!stateStack.empty()) {
                        current = stateStack.top();
                        stateStack.pop();
                        currentPolyline.append(current.position);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        if (currentPolyline.size() > 1) {
            auto poly = std::make_shared<Polygon>(currentPolyline, false);
            poly->setStrokeColor(endColor);
            poly->setFillColor(Qt::transparent);
            poly->setStrokeWidth(strokeWidth);
            generatedShapes.push_back(poly);
        }
    }

    return generatedShapes;
}

std::vector<std::shared_ptr<Shape>> LSystemGenerator::generate(const ParameterMap& params, const QPointF& origin) {
    int presetIdx = std::get<int>(params.at("preset"));
    int depth = std::get<int>(params.at("depth"));
    double stepLength = std::get<double>(params.at("stepLength"));
    double angle = std::get<double>(params.at("angle"));
    int symmetry = std::get<int>(params.at("symmetry"));
    double strokeWidth = std::get<double>(params.at("strokeWidth"));
    QColor startColor = std::get<QColor>(params.at("startColor"));
    QColor endColor = std::get<QColor>(params.at("endColor"));

    std::string axiom;
    std::map<char, std::string> rules;
    double defaultAngle = angle;

    switch (presetIdx) {
        case 0: { // Dragon Curve
            axiom = "FX";
            rules['X'] = "X+YF+";
            rules['Y'] = "-FX-Y";
            defaultAngle = 90.0;
            break;
        }
        case 1: { // Koch Snowflake
            axiom = "F--F--F";
            rules['F'] = "F+F--F+F";
            defaultAngle = 60.0;
            break;
        }
        case 2: { // Sierpinski Arrowhead
            axiom = "YF";
            rules['X'] = "YF+XF+Y";
            rules['Y'] = "XF-YF-X";
            defaultAngle = 60.0;
            break;
        }
        case 3: { // Plant / Fern Tree
            axiom = "X";
            rules['X'] = "F[+X][-X]FX";
            rules['F'] = "FF";
            defaultAngle = 25.0;
            break;
        }
        case 4: { // Penrose Mandala
            axiom = "[F]++[F]++[F]++[F]++[F]";
            rules['F'] = "F++F++F+++++F-F++F";
            defaultAngle = 36.0;
            break;
        }
        default:
            axiom = "F";
            rules['F'] = "F+F-F-F+F";
            break;
    }

    std::string expandedString = expandLSystem(axiom, rules, depth);
    return renderTurtle(expandedString, origin, stepLength, defaultAngle, symmetry, startColor, endColor, strokeWidth);
}

} // namespace VectorForge
