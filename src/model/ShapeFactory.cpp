#include "ShapeFactory.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "BezierCurve.h"
#include "GroupShape.h"
#include <QJsonArray>
#include <cmath>

namespace VectorForge {

std::shared_ptr<Shape> ShapeFactory::createShape(ShapeType type) {
    switch (type) {
        case ShapeType::Circle:
            return std::make_shared<Circle>();
        case ShapeType::Rectangle:
            return std::make_shared<Rectangle>();
        case ShapeType::Polygon:
            return std::make_shared<Polygon>();
        case ShapeType::BezierCurve:
            return std::make_shared<BezierCurve>();
        case ShapeType::Group:
            return std::make_shared<GroupShape>();
        default:
            return nullptr;
    }
}

std::shared_ptr<Shape> ShapeFactory::createShapeFromTool(ToolType tool, const QRectF& dragBounds, const QColor& stroke, const QColor& fill) {
    QRectF normRect = dragBounds.normalized();
    if (normRect.width() < 1.0) normRect.setWidth(20.0);
    if (normRect.height() < 1.0) normRect.setHeight(20.0);

    QPointF center = normRect.center();
    double radius = std::min(normRect.width(), normRect.height()) / 2.0;

    std::shared_ptr<Shape> shape = nullptr;

    switch (tool) {
        case ToolType::Rectangle: {
            shape = std::make_shared<Rectangle>(normRect);
            break;
        }
        case ToolType::Circle: {
            shape = std::make_shared<Circle>(center, radius);
            break;
        }
        case ToolType::Polygon: {
            shape = Polygon::createRegularPolygon(center, radius, 5);
            break;
        }
        case ToolType::Star: {
            shape = Polygon::createStar(center, radius, radius * 0.4, 5);
            break;
        }
        case ToolType::Line: {
            QPolygonF linePts;
            linePts.append(dragBounds.topLeft());
            linePts.append(dragBounds.bottomRight());
            auto polyline = std::make_shared<Polygon>(linePts, false);
            polyline->setName("Line");
            shape = polyline;
            break;
        }
        case ToolType::Bezier: {
            QPointF p0 = dragBounds.topLeft();
            QPointF p3 = dragBounds.bottomRight();
            QPointF p1(p0.x() + normRect.width() * 0.5, p0.y());
            QPointF p2(p0.x() + normRect.width() * 0.5, p3.y());
            shape = std::make_shared<BezierCurve>(p0, p1, p2, p3);
            break;
        }
        default:
            return nullptr;
    }

    if (shape) {
        shape->setStrokeColor(stroke);
        shape->setFillColor(fill);
    }

    return shape;
}

std::shared_ptr<Shape> ShapeFactory::createShapeFromToolName(const std::string& toolName, const QRectF& dragBounds, const QColor& stroke, const QColor& fill) {
    if (toolName == "Rectangle") return createShapeFromTool(ToolType::Rectangle, dragBounds, stroke, fill);
    if (toolName == "Circle") return createShapeFromTool(ToolType::Circle, dragBounds, stroke, fill);
    if (toolName == "Polygon") return createShapeFromTool(ToolType::Polygon, dragBounds, stroke, fill);
    if (toolName == "Star") return createShapeFromTool(ToolType::Star, dragBounds, stroke, fill);
    if (toolName == "Line") return createShapeFromTool(ToolType::Line, dragBounds, stroke, fill);
    if (toolName == "Bezier") return createShapeFromTool(ToolType::Bezier, dragBounds, stroke, fill);
    return nullptr;
}

std::vector<std::string> ShapeFactory::availableToolNames() {
    return {"Select", "Rectangle", "Circle", "Polygon", "Star", "Line", "Bezier", "Generative"};
}

std::shared_ptr<Shape> ShapeFactory::createDefaultCircle(const QPointF& center, double radius) {
    return std::make_shared<Circle>(center, radius);
}

std::shared_ptr<Shape> ShapeFactory::createDefaultRectangle(const QRectF& rect) {
    return std::make_shared<Rectangle>(rect);
}

std::shared_ptr<Shape> ShapeFactory::createRegularPolygon(const QPointF& center, double radius, int sides) {
    return Polygon::createRegularPolygon(center, radius, sides);
}

std::shared_ptr<Shape> ShapeFactory::createStar(const QPointF& center, double outerRadius, double innerRadius, int points) {
    return Polygon::createStar(center, outerRadius, innerRadius, points);
}

std::shared_ptr<Shape> ShapeFactory::createDefaultBezier(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3) {
    return std::make_shared<BezierCurve>(p0, p1, p2, p3);
}

std::shared_ptr<Shape> ShapeFactory::fromJson(const QJsonObject& json) {
    QString typeStr = json["type"].toString();
    std::shared_ptr<Shape> shape = nullptr;

    if (typeStr == "Circle") {
        shape = std::make_shared<Circle>();
    } else if (typeStr == "Rectangle") {
        shape = std::make_shared<Rectangle>();
    } else if (typeStr == "Polygon") {
        shape = std::make_shared<Polygon>();
    } else if (typeStr == "BezierCurve") {
        shape = std::make_shared<BezierCurve>();
    } else if (typeStr == "Group") {
        auto group = std::make_shared<GroupShape>();
        if (json.contains("children")) {
            QJsonArray arr = json["children"].toArray();
            for (const auto& item : arr) {
                auto child = fromJson(item.toObject());
                if (child) {
                    group->addChild(child);
                }
            }
        }
        shape = group;
    }

    if (shape) {
        shape->fromJson(json);
    }

    return shape;
}

} // namespace VectorForge
