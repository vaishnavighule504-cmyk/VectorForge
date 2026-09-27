#include "Circle.h"
#include <cmath>

namespace VectorForge {

Circle::Circle() {
    m_name = "Circle";
}

Circle::Circle(const QPointF& center, double radius)
    : m_center(center), m_radius(radius) {
    m_name = "Circle";
}

void Circle::draw(QPainter& painter) const {
    if (!m_isVisible) return;

    painter.save();
    painter.setTransform(m_transform, true);
    setupPainterStyle(painter);

    QRectF rect(m_center.x() - m_radius, m_center.y() - m_radius,
                m_radius * 2.0, m_radius * 2.0);
    painter.drawEllipse(rect);

    painter.restore();
}

QPainterPath Circle::shapePath() const {
    QPainterPath path;
    path.addEllipse(m_center, m_radius, m_radius);
    return m_transform.map(path);
}

bool Circle::contains(const QPointF& point) const {
    if (!m_isVisible) return false;
    return shapePath().contains(point);
}

QRectF Circle::boundingRect() const {
    return shapePath().boundingRect();
}

void Circle::translate(double dx, double dy) {
    m_transform.translate(dx, dy);
}

void Circle::scale(double sx, double sy, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.scale(sx, sy);
    m_transform.translate(-origin.x(), -origin.y());
}

void Circle::rotate(double angleDegrees, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.rotate(angleDegrees);
    m_transform.translate(-origin.x(), -origin.y());
}

std::shared_ptr<Shape> Circle::clone() const {
    auto copy = std::make_shared<Circle>(*this);
    return copy;
}

QJsonObject Circle::toJson() const {
    QJsonObject json = Shape::toJson();
    json["centerX"] = m_center.x();
    json["centerY"] = m_center.y();
    json["radius"] = m_radius;
    return json;
}

void Circle::fromJson(const QJsonObject& json) {
    Shape::fromJson(json);
    if (json.contains("centerX") && json.contains("centerY")) {
        m_center = QPointF(json["centerX"].toDouble(), json["centerY"].toDouble());
    }
    if (json.contains("radius")) {
        m_radius = json["radius"].toDouble(50.0);
    }
}

} // namespace VectorForge
