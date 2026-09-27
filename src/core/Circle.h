#ifndef VECTORFORGE_CIRCLE_H
#define VECTORFORGE_CIRCLE_H

#include "Shape.h"

namespace VectorForge {

class Circle : public Shape {
public:
    Circle();
    Circle(const QPointF& center, double radius);
    ~Circle() override = default;

    // Polymorphic Overrides
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    QPainterPath shapePath() const override;

    void translate(double dx, double dy) override;
    void scale(double sx, double sy, const QPointF& origin) override;
    void rotate(double angleDegrees, const QPointF& origin) override;

    std::shared_ptr<Shape> clone() const override;
    ShapeType type() const override { return ShapeType::Circle; }
    std::string typeName() const override { return "Circle"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    // Circle specific properties
    QPointF center() const { return m_center; }
    void setCenter(const QPointF& center) { m_center = center; }

    double radius() const { return m_radius; }
    void setRadius(double radius) { m_radius = radius; }

private:
    QPointF m_center{0.0, 0.0};
    double m_radius{50.0};
};

} // namespace VectorForge

#endif // VECTORFORGE_CIRCLE_H
