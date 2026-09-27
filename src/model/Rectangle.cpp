#include "Rectangle.h"

namespace VectorForge {

Rectangle::Rectangle() {
    m_name = "Rectangle";
}

Rectangle::Rectangle(const QRectF& rect, double cornerRadius)
    : m_rect(rect), m_cornerRadius(cornerRadius) {
    m_name = "Rectangle";
}

Rectangle::Rectangle(double x, double y, double width, double height, double cornerRadius)
    : m_rect(x, y, width, height), m_cornerRadius(cornerRadius) {
    m_name = "Rectangle";
}

void Rectangle::draw(QPainter& painter) const {
    if (!m_isVisible) return;

    painter.save();
    painter.setTransform(m_transform, true);
    setupPainterStyle(painter);

    if (m_cornerRadius > 0.0) {
        painter.drawRoundedRect(m_rect, m_cornerRadius, m_cornerRadius);
    } else {
        painter.drawRect(m_rect);
    }

    painter.restore();
}

QPainterPath Rectangle::shapePath() const {
    QPainterPath path;
    if (m_cornerRadius > 0.0) {
        path.addRoundedRect(m_rect, m_cornerRadius, m_cornerRadius);
    } else {
        path.addRect(m_rect);
    }
    return m_transform.map(path);
}

bool Rectangle::contains(const QPointF& point) const {
    if (!m_isVisible) return false;
    return shapePath().contains(point);
}

QRectF Rectangle::boundingRect() const {
    return shapePath().boundingRect();
}

void Rectangle::translate(double dx, double dy) {
    m_transform.translate(dx, dy);
}

void Rectangle::scale(double sx, double sy, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.scale(sx, sy);
    m_transform.translate(-origin.x(), -origin.y());
}

void Rectangle::rotate(double angleDegrees, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.rotate(angleDegrees);
    m_transform.translate(-origin.x(), -origin.y());
}

std::shared_ptr<Shape> Rectangle::clone() const {
    return std::make_shared<Rectangle>(*this);
}

QJsonObject Rectangle::toJson() const {
    QJsonObject json = Shape::toJson();
    json["x"] = m_rect.x();
    json["y"] = m_rect.y();
    json["width"] = m_rect.width();
    json["height"] = m_rect.height();
    json["cornerRadius"] = m_cornerRadius;
    return json;
}

void Rectangle::fromJson(const QJsonObject& json) {
    Shape::fromJson(json);
    double x = json["x"].toDouble(0.0);
    double y = json["y"].toDouble(0.0);
    double w = json["width"].toDouble(100.0);
    double h = json["height"].toDouble(80.0);
    m_rect = QRectF(x, y, w, h);
    m_cornerRadius = json["cornerRadius"].toDouble(0.0);
}

} // namespace VectorForge
