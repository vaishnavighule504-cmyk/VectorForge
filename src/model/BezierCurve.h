#ifndef VECTORFORGE_BEZIERCURVE_H
#define VECTORFORGE_BEZIERCURVE_H

#include "Shape.h"

namespace VectorForge {

class BezierCurve : public Shape {
public:
    BezierCurve();
    BezierCurve(const QPointF& start, const QPointF& ctrl1, const QPointF& ctrl2, const QPointF& end);
    ~BezierCurve() override = default;

    // Polymorphic Overrides
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    QPainterPath shapePath() const override;

    void translate(double dx, double dy) override;
    void scale(double sx, double sy, const QPointF& origin) override;
    void rotate(double angleDegrees, const QPointF& origin) override;

    std::shared_ptr<Shape> clone() const override;
    ShapeType type() const override { return ShapeType::BezierCurve; }
    std::string typeName() const override { return "BezierCurve"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    // Bezier control points accessors
    QPointF startPoint() const { return m_p0; }
    void setStartPoint(const QPointF& pt) { m_p0 = pt; }

    QPointF controlPoint1() const { return m_p1; }
    void setControlPoint1(const QPointF& pt) { m_p1 = pt; }

    QPointF controlPoint2() const { return m_p2; }
    void setControlPoint2(const QPointF& pt) { m_p2 = pt; }

    QPointF endPoint() const { return m_p3; }
    void setEndPoint(const QPointF& pt) { m_p3 = pt; }

    // De Casteljau's algorithm evaluation at t in [0, 1]
    QPointF evaluateDeCasteljau(double t) const;

private:
    QPointF m_p0{0.0, 0.0};     // Start
    QPointF m_p1{30.0, -50.0};  // Control 1
    QPointF m_p2{70.0, 50.0};   // Control 2
    QPointF m_p3{100.0, 0.0};   // End
};

} // namespace VectorForge

#endif // VECTORFORGE_BEZIERCURVE_H
