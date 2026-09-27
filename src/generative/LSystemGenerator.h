#ifndef VECTORFORGE_LSYSTEMGENERATOR_H
#define VECTORFORGE_LSYSTEMGENERATOR_H

#include "GenerativeAlgorithm.h"
#include "../model/Polygon.h"
#include <stack>
#include <map>

namespace VectorForge {

/**
 * @brief Turtle Graphics state for L-System evaluation.
 */
struct TurtleState {
    QPointF position;
    double angleDegrees; // Angle in degrees
    QColor color;
    double strokeWidth;
};

/**
 * @brief L-System Fractal & Mandala Generative Algorithm.
 * 
 * Supports classic presets: Dragon Curve, Koch Snowflake, Sierpinski Arrowhead,
 * Plant/Fern Tree, Penrose Tile, and Custom L-System rules.
 * Integrates n-fold rotational symmetry and smooth color gradients.
 */
class LSystemGenerator : public IGenerativeAlgorithm {
public:
    enum class Preset {
        DragonCurve,
        KochSnowflake,
        SierpinskiArrowhead,
        PlantTree,
        PenroseMandala,
        Custom
    };

    LSystemGenerator() = default;
    ~LSystemGenerator() override = default;

    std::string name() const override { return "L-System Fractal"; }
    std::string description() const override { return "Generates intricate recursive fractal curves & symmetric mandalas via L-System string grammars."; }
    std::vector<ParameterInfo> parameters() const override;

    std::vector<std::shared_ptr<Shape>> generate(const ParameterMap& params, const QPointF& origin) override;

    // Core String Grammar Expansion
    static std::string expandLSystem(const std::string& axiom, const std::map<char, std::string>& rules, int iterations);

    // Turtle Graphics Rendering
    static std::vector<std::shared_ptr<Shape>> renderTurtle(
        const std::string& lsystemString,
        const QPointF& origin,
        double stepLength,
        double angleStepDegrees,
        int symmetryCount,
        const QColor& startColor,
        const QColor& endColor,
        double strokeWidth
    );
};

} // namespace VectorForge

#endif // VECTORFORGE_LSYSTEMGENERATOR_H
