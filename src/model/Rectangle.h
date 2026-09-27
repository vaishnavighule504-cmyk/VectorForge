#ifndef VECTORFORGE_RECTANGLE_H
#define VECTORFORGE_RECTANGLE_H

#include "Shape.h"

namespace VectorForge {

class Rectangle : public Shape {
public:
    Rectangle();
    Rectangle(const QRectF& rect, double cornerRadius = 0.0);
    Rectangle(double x, double y, double width, double height, double cornerRadius = 0.0);
    ~Rectangle() override = default;

    // Polymorphic Overrides
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    QPainterPath shapePath() const override;

    void translate(double dx, double dy) override;
    void scale(double sx, double sy, const QPointF& origin) override;
    void rotate(double angleDegrees, const QPointF& origin) override;

    std::shared_ptr<Shape> clone() const override;
    ShapeType type() const override { return ShapeType::Rectangle; }
    std::string typeName() const override { return "Rectangle"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    // Rectangle specific properties
    QRectF rect() const { return m_rect; }
    void setRect(const QRectF& rect) { m_rect = rect; }

    double cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(double radius) { m_cornerRadius = radius; }

private:
    QRectF m_rect{0.0, 0.0, 100.0, 80.0};
    double m_cornerRadius{0.0};
};

} // namespace VectorForge

#endif // VECTORFORGE_RECTANGLE_H
