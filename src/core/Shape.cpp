#include "Shape.h"
#include <QUuid>

namespace VectorForge {

Shape::Shape() {
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
}

void Shape::setupPainterStyle(QPainter& painter) const {
    painter.setOpacity(m_opacity);
    
    QPen pen(m_strokeColor, m_strokeWidth, m_strokeStyle);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(m_strokeWidth > 0 ? pen : Qt::NoPen);

    if (m_fillColor.alpha() > 0) {
        painter.setBrush(QBrush(m_fillColor));
    } else {
        painter.setBrush(Qt::NoBrush);
    }
}

QJsonObject Shape::toJson() const {
    QJsonObject json;
    json["id"] = QString::fromStdString(m_id);
    json["name"] = QString::fromStdString(m_name);
    json["type"] = QString::fromStdString(typeName());
    json["fillColor"] = m_fillColor.name(QColor::HexArgb);
    json["strokeColor"] = m_strokeColor.name(QColor::HexArgb);
    json["strokeWidth"] = m_strokeWidth;
    json["strokeStyle"] = static_cast<int>(m_strokeStyle);
    json["opacity"] = m_opacity;
    json["isVisible"] = m_isVisible;
    json["isLocked"] = m_isLocked;
    
    QJsonObject matrixJson;
    matrixJson["m11"] = m_transform.m11();
    matrixJson["m12"] = m_transform.m12();
    matrixJson["m21"] = m_transform.m21();
    matrixJson["m22"] = m_transform.m22();
    matrixJson["dx"] = m_transform.dx();
    matrixJson["dy"] = m_transform.dy();
    json["transform"] = matrixJson;

    return json;
}

void Shape::fromJson(const QJsonObject& json) {
    if (json.contains("id")) m_id = json["id"].toString().toStdString();
    if (json.contains("name")) m_name = json["name"].toString().toStdString();
    if (json.contains("fillColor")) m_fillColor = QColor(json["fillColor"].toString());
    if (json.contains("strokeColor")) m_strokeColor = QColor(json["strokeColor"].toString());
    if (json.contains("strokeWidth")) m_strokeWidth = json["strokeWidth"].toDouble(2.0);
    if (json.contains("strokeStyle")) m_strokeStyle = static_cast<Qt::PenStyle>(json["strokeStyle"].toInt(1));
    if (json.contains("opacity")) m_opacity = json["opacity"].toDouble(1.0);
    if (json.contains("isVisible")) m_isVisible = json["isVisible"].toBool(true);
    if (json.contains("isLocked")) m_isLocked = json["isLocked"].toBool(false);

    if (json.contains("transform")) {
        QJsonObject t = json["transform"].toObject();
        m_transform = QTransform(
            t["m11"].toDouble(1.0), t["m12"].toDouble(0.0),
            t["m21"].toDouble(0.0), t["m22"].toDouble(1.0),
            t["dx"].toDouble(0.0), t["dy"].toDouble(0.0)
        );
    }
}

} // namespace VectorForge
