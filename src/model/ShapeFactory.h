#ifndef VECTORFORGE_SHAPEFACTORY_H
#define VECTORFORGE_SHAPEFACTORY_H

#include "Shape.h"
#include <memory>
#include <string>
#include <vector>

namespace VectorForge {

enum class ToolType {
    Select,
    Rectangle,
    Circle,
    Polygon,
    Star,
    Bezier,
    Line,
    Generative
};

class ShapeFactory {
public:
    static std::shared_ptr<Shape> createShape(ShapeType type);
    
    static std::shared_ptr<Shape> createShapeFromTool(
        ToolType tool, 
        const QRectF& dragBounds, 
        const QColor& stroke = Qt::white, 
        const QColor& fill = QColor(100, 150, 240, 100)
    );

    static std::shared_ptr<Shape> createShapeFromToolName(
        const std::string& toolName, 
        const QRectF& dragBounds, 
        const QColor& stroke = Qt::white, 
        const QColor& fill = QColor(100, 150, 240, 100)
    );

    static std::shared_ptr<Shape> createDefaultCircle(const QPointF& center = {0,0}, double radius = 50);
    static std::shared_ptr<Shape> createDefaultRectangle(const QRectF& rect = {0,0,100,80});
    static std::shared_ptr<Shape> createRegularPolygon(const QPointF& center, double radius, int sides = 5);
    static std::shared_ptr<Shape> createStar(const QPointF& center, double outerRadius, double innerRadius, int points = 5);
    static std::shared_ptr<Shape> createDefaultBezier(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);
    
    static std::shared_ptr<Shape> fromJson(const QJsonObject& json);

    static std::vector<std::string> availableToolNames();
};

} // namespace VectorForge

#endif // VECTORFORGE_SHAPEFACTORY_H
