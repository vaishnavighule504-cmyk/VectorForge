#include "Layer.h"
#include "ShapeFactory.h"
#include <algorithm>

namespace VectorForge {

Layer::Layer(const std::string& name) : m_name(name) {}

void Layer::addShape(const std::shared_ptr<Shape>& shape) {
    if (shape) {
        m_shapes.push_back(shape);
    }
}

void Layer::insertShape(size_t index, const std::shared_ptr<Shape>& shape) {
    if (shape) {
        if (index >= m_shapes.size()) {
            m_shapes.push_back(shape);
        } else {
            m_shapes.insert(m_shapes.begin() + index, shape);
        }
    }
}

bool Layer::removeShape(const std::shared_ptr<Shape>& shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        m_shapes.erase(it);
        return true;
    }
    return false;
}

void Layer::clear() {
    m_shapes.clear();
}

int Layer::indexOf(const std::shared_ptr<Shape>& shape) const {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        return static_cast<int>(std::distance(m_shapes.begin(), it));
    }
    return -1;
}

QJsonObject Layer::toJson() const {
    QJsonObject json;
    json["name"] = QString::fromStdString(m_name);
    json["isVisible"] = m_isVisible;
    json["isLocked"] = m_isLocked;

    QJsonArray shapesArray;
    for (const auto& shape : m_shapes) {
        if (shape) {
            shapesArray.append(shape->toJson());
        }
    }
    json["shapes"] = shapesArray;
    return json;
}

void Layer::fromJson(const QJsonObject& json) {
    if (json.contains("name")) m_name = json["name"].toString().toStdString();
    if (json.contains("isVisible")) m_isVisible = json["isVisible"].toBool(true);
    if (json.contains("isLocked")) m_isLocked = json["isLocked"].toBool(false);

    m_shapes.clear();
    if (json.contains("shapes")) {
        QJsonArray arr = json["shapes"].toArray();
        for (const auto& val : arr) {
            auto shape = ShapeFactory::fromJson(val.toObject());
            if (shape) {
                m_shapes.push_back(shape);
            }
        }
    }
}

} // namespace VectorForge
