#include "Layer.h"
#include "ShapeFactory.h"
#include <algorithm>
#include <QUuid>

namespace VectorForge {

Layer::Layer(const std::string& name) : m_name(name) {
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
}

bool Layer::isEffectiveVisible() const {
    if (!m_isVisible) return false;
    auto p = parent();
    if (p) return p->isEffectiveVisible();
    return true;
}

bool Layer::isEffectiveLocked() const {
    if (m_isLocked) return true;
    auto p = parent();
    if (p) return p->isEffectiveLocked();
    return false;
}

std::shared_ptr<Layer> Layer::createSubLayer(const std::string& name) {
    std::string subName = name;
    if (subName.empty()) {
        subName = m_name + "." + std::to_string(m_children.size() + 1);
    }
    auto child = std::make_shared<Layer>(subName);
    addChildLayer(child);
    return child;
}

void Layer::addChildLayer(const std::shared_ptr<Layer>& childLayer) {
    if (childLayer && childLayer.get() != this) {
        childLayer->setParent(shared_from_this());
        m_children.push_back(childLayer);
    }
}

bool Layer::removeChildLayer(const std::shared_ptr<Layer>& childLayer) {
    auto it = std::find(m_children.begin(), m_children.end(), childLayer);
    if (it != m_children.end()) {
        (*it)->setParent(nullptr);
        m_children.erase(it);
        return true;
    }
    return false;
}

bool Layer::moveChildUp(size_t index) {
    if (index > 0 && index < m_children.size()) {
        std::swap(m_children[index], m_children[index - 1]);
        return true;
    }
    return false;
}

bool Layer::moveChildDown(size_t index) {
    if (index + 1 < m_children.size()) {
        std::swap(m_children[index], m_children[index + 1]);
        return true;
    }
    return false;
}

bool Layer::setChildIndex(size_t currentIndex, size_t newIndex) {
    if (currentIndex >= m_children.size() || newIndex >= m_children.size()) {
        return false;
    }
    if (currentIndex == newIndex) return true;

    auto child = m_children[currentIndex];
    m_children.erase(m_children.begin() + currentIndex);
    m_children.insert(m_children.begin() + newIndex, child);
    return true;
}

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

void Layer::clearShapes() {
    m_shapes.clear();
}

int Layer::indexOfShape(const std::shared_ptr<Shape>& shape) const {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        return static_cast<int>(std::distance(m_shapes.begin(), it));
    }
    return -1;
}

std::vector<std::shared_ptr<Shape>> Layer::allShapesRecursive() const {
    std::vector<std::shared_ptr<Shape>> result = m_shapes;
    for (const auto& child : m_children) {
        if (child) {
            auto childShapes = child->allShapesRecursive();
            result.insert(result.end(), childShapes.begin(), childShapes.end());
        }
    }
    return result;
}

QJsonObject Layer::toJson() const {
    QJsonObject json;
    json["id"] = QString::fromStdString(m_id);
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

    QJsonArray childrenArray;
    for (const auto& child : m_children) {
        if (child) {
            childrenArray.append(child->toJson());
        }
    }
    json["children"] = childrenArray;

    return json;
}

void Layer::fromJson(const QJsonObject& json) {
    if (json.contains("id")) m_id = json["id"].toString().toStdString();
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

    m_children.clear();
    if (json.contains("children")) {
        QJsonArray arr = json["children"].toArray();
        for (const auto& val : arr) {
            auto child = std::make_shared<Layer>();
            child->fromJson(val.toObject());
            addChildLayer(child);
        }
    }
}

} // namespace VectorForge
