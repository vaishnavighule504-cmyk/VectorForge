#ifndef VECTORFORGE_GROUPSHAPE_H
#define VECTORFORGE_GROUPSHAPE_H

#include "Shape.h"

namespace VectorForge {

class GroupShape : public Shape {
public:
    GroupShape();
    explicit GroupShape(const std::vector<std::shared_ptr<Shape>>& children);
    ~GroupShape() override = default;

    // Polymorphic Overrides
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    QPainterPath shapePath() const override;

    void translate(double dx, double dy) override;
    void scale(double sx, double sy, const QPointF& origin) override;
    void rotate(double angleDegrees, const QPointF& origin) override;

    std::shared_ptr<Shape> clone() const override;
    ShapeType type() const override { return ShapeType::Group; }
    std::string typeName() const override { return "Group"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    // Composite Child Operations
    void addChild(const std::shared_ptr<Shape>& child);
    void removeChild(const std::shared_ptr<Shape>& child);
    const std::vector<std::shared_ptr<Shape>>& children() const { return m_children; }
    void clear();

private:
    std::vector<std::shared_ptr<Shape>> m_children;
};

} // namespace VectorForge

#endif // VECTORFORGE_GROUPSHAPE_H
