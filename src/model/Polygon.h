#ifndef VECTORFORGE_POLYGON_H
#define VECTORFORGE_POLYGON_H

#include "Shape.h"
#include <QPolygonF>

namespace VectorForge {

class Polygon : public Shape {
public:
    Polygon();
    Polygon(const QPolygonF& points, bool isClosed = true);
    Polygon(const std::vector<QPointF>& points, bool isClosed = true);
    ~Polygon() override = default;

    // Polymorphic Overrides
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    QPainterPath shapePath() const override;

    void translate(double dx, double dy) override;
    void scale(double sx, double sy, const QPointF& origin) override;
    void rotate(double angleDegrees, const QPointF& origin) override;

    std::shared_ptr<Shape> clone() const override;
    ShapeType type() const override { return ShapeType::Polygon; }
    std::string typeName() const override { return "Polygon"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    // Polygon specific methods
    QPolygonF points() const { return m_points; }
    void setPoints(const QPolygonF& points) { m_points = points; }
    void addPoint(const QPointF& pt) { m_points.append(pt); }
    void clearPoints() { m_points.clear(); }

    bool isClosed() const { return m_isClosed; }
    void setClosed(bool closed) { m_isClosed = closed; }

    // Helper static generators
    static std::shared_ptr<Polygon> createRegularPolygon(const QPointF& center, double radius, int sides);
    static std::shared_ptr<Polygon> createStar(const QPointF& center, double outerRadius, double innerRadius, int points);

private:
    QPolygonF m_points;
    bool m_isClosed{true};
};

} // namespace VectorForge

#endif // VECTORFORGE_POLYGON_H
