#include "BezierCurve.h"

namespace VectorForge {

BezierCurve::BezierCurve() {
    m_name = "Bezier Curve";
}

BezierCurve::BezierCurve(const QPointF& start, const QPointF& ctrl1, const QPointF& ctrl2, const QPointF& end)
    : m_p0(start), m_p1(ctrl1), m_p2(ctrl2), m_p3(end) {
    m_name = "Bezier Curve";
}

QPointF BezierCurve::evaluateDeCasteljau(double t) const {
    auto lerp = [](const QPointF& a, const QPointF& b, double t) {
        return QPointF((1.0 - t) * a.x() + t * b.x(), (1.0 - t) * a.y() + t * b.y());
    };

    QPointF p01 = lerp(m_p0, m_p1, t);
    QPointF p12 = lerp(m_p1, m_p2, t);
    QPointF p23 = lerp(m_p2, m_p3, t);

    QPointF p012 = lerp(p01, p12, t);
    QPointF p123 = lerp(p12, p23, t);

    return lerp(p012, p123, t);
}

void BezierCurve::draw(QPainter& painter) const {
    if (!m_isVisible) return;

    painter.save();
    painter.setTransform(m_transform, true);
    setupPainterStyle(painter);

    QPainterPath path;
    path.moveTo(m_p0);
    path.cubicTo(m_p1, m_p2, m_p3);
    painter.drawPath(path);

    painter.restore();
}

QPainterPath BezierCurve::shapePath() const {
    QPainterPath path;
    path.moveTo(m_p0);
    path.cubicTo(m_p1, m_p2, m_p3);
    return m_transform.map(path);
}

bool BezierCurve::contains(const QPointF& point) const {
    if (!m_isVisible) return false;

    QPainterPath path = shapePath();
    QPainterPathStroker stroker;
    stroker.setWidth(m_strokeWidth > 0 ? m_strokeWidth + 6.0 : 8.0);
    return stroker.createStroke(path).contains(point);
}

QRectF BezierCurve::boundingRect() const {
    return shapePath().boundingRect();
}

void BezierCurve::translate(double dx, double dy) {
    m_transform.translate(dx, dy);
}

void BezierCurve::scale(double sx, double sy, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.scale(sx, sy);
    m_transform.translate(-origin.x(), -origin.y());
}

void BezierCurve::rotate(double angleDegrees, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.rotate(angleDegrees);
    m_transform.translate(-origin.x(), -origin.y());
}

std::shared_ptr<Shape> BezierCurve::clone() const {
    return std::make_shared<BezierCurve>(*this);
}

QJsonObject BezierCurve::toJson() const {
    QJsonObject json = Shape::toJson();
    json["p0x"] = m_p0.x(); json["p0y"] = m_p0.y();
    json["p1x"] = m_p1.x(); json["p1y"] = m_p1.y();
    json["p2x"] = m_p2.x(); json["p2y"] = m_p2.y();
    json["p3x"] = m_p3.x(); json["p3y"] = m_p3.y();
    return json;
}

void BezierCurve::fromJson(const QJsonObject& json) {
    Shape::fromJson(json);
    m_p0 = QPointF(json["p0x"].toDouble(), json["p0y"].toDouble());
    m_p1 = QPointF(json["p1x"].toDouble(), json["p1y"].toDouble());
    m_p2 = QPointF(json["p2x"].toDouble(), json["p2y"].toDouble());
    m_p3 = QPointF(json["p3x"].toDouble(), json["p3y"].toDouble());
}

} // namespace VectorForge
