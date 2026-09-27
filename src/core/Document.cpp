#include "Document.h"
#include <algorithm>

namespace VectorForge {

Document::Document(QObject* parent) : QObject(parent) {
    // Create initial default layer
    addLayer("Layer 1");
}

void Document::setDimensions(double width, double height) {
    m_width = width;
    m_height = height;
    emit documentChanged();
}

std::shared_ptr<Layer> Document::activeLayer() const {
    if (m_layers.empty()) return nullptr;
    if (m_activeLayerIndex < m_layers.size()) {
        return m_layers[m_activeLayerIndex];
    }
    return m_layers.front();
}

void Document::setActiveLayerIndex(size_t index) {
    if (index < m_layers.size()) {
        m_activeLayerIndex = index;
        emit layerStructureChanged();
    }
}

std::shared_ptr<Layer> Document::addLayer(const std::string& name) {
    std::string layerName = name;
    if (layerName.empty()) {
        layerName = "Layer " + std::to_string(m_layers.size() + 1);
    }
    auto layer = std::make_shared<Layer>(layerName);
    m_layers.push_back(layer);
    m_activeLayerIndex = m_layers.size() - 1;
    emit layerStructureChanged();
    emit documentChanged();
    return layer;
}

bool Document::removeLayer(size_t index) {
    if (m_layers.size() <= 1 || index >= m_layers.size()) {
        return false; // Retain at least one layer
    }
    m_layers.erase(m_layers.begin() + index);
    if (m_activeLayerIndex >= m_layers.size()) {
        m_activeLayerIndex = m_layers.size() - 1;
    }
    clearSelection();
    emit layerStructureChanged();
    emit documentChanged();
    return true;
}

std::vector<std::shared_ptr<Shape>> Document::allShapes() const {
    std::vector<std::shared_ptr<Shape>> result;
    for (const auto& layer : m_layers) {
        if (layer) {
            const auto& layerShapes = layer->shapes();
            result.insert(result.end(), layerShapes.begin(), layerShapes.end());
        }
    }
    return result;
}

std::shared_ptr<Shape> Document::shapeAt(const QPointF& point) const {
    // Top-to-bottom hit testing (reverse iteration over layers and shapes)
    for (auto layerIt = m_layers.rbegin(); layerIt != m_layers.rend(); ++layerIt) {
        const auto& layer = *layerIt;
        if (!layer || !layer->isVisible() || layer->isLocked()) continue;

        const auto& shapes = layer->shapes();
        for (auto shapeIt = shapes.rbegin(); shapeIt != shapes.rend(); ++shapeIt) {
            const auto& shape = *shapeIt;
            if (shape && shape->isVisible() && !shape->isLocked()) {
                if (shape->contains(point)) {
                    return shape;
                }
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Shape>> Document::shapesInRect(const QRectF& rect) const {
    std::vector<std::shared_ptr<Shape>> result;
    for (const auto& layer : m_layers) {
        if (!layer || !layer->isVisible() || layer->isLocked()) continue;

        for (const auto& shape : layer->shapes()) {
            if (shape && shape->isVisible() && !shape->isLocked()) {
                if (rect.intersects(shape->boundingRect()) || rect.contains(shape->boundingRect())) {
                    result.push_back(shape);
                }
            }
        }
    }
    return result;
}

void Document::setSelection(const std::vector<std::shared_ptr<Shape>>& selection) {
    m_selection = selection;
    emit selectionChanged();
}

void Document::selectShape(const std::shared_ptr<Shape>& shape, bool clearPrevious) {
    if (!shape) return;

    if (clearPrevious) {
        m_selection.clear();
    }
    if (!isSelected(shape)) {
        m_selection.push_back(shape);
        emit selectionChanged();
    }
}

void Document::deselectShape(const std::shared_ptr<Shape>& shape) {
    auto it = std::find(m_selection.begin(), m_selection.end(), shape);
    if (it != m_selection.end()) {
        m_selection.erase(it);
        emit selectionChanged();
    }
}

void Document::clearSelection() {
    if (!m_selection.empty()) {
        m_selection.clear();
        emit selectionChanged();
    }
}

bool Document::isSelected(const std::shared_ptr<Shape>& shape) const {
    return std::find(m_selection.begin(), m_selection.end(), shape) != m_selection.end();
}

std::shared_ptr<Layer> Document::layerOf(const std::shared_ptr<Shape>& shape) const {
    for (const auto& layer : m_layers) {
        if (layer) {
            const auto& shapes = layer->shapes();
            if (std::find(shapes.begin(), shapes.end(), shape) != shapes.end()) {
                return layer;
            }
        }
    }
    return nullptr;
}

QJsonObject Document::toJson() const {
    QJsonObject json;
    json["width"] = m_width;
    json["height"] = m_height;
    json["activeLayerIndex"] = static_cast<int>(m_activeLayerIndex);

    QJsonArray layersArray;
    for (const auto& layer : m_layers) {
        if (layer) {
            layersArray.append(layer->toJson());
        }
    }
    json["layers"] = layersArray;
    return json;
}

bool Document::fromJson(const QJsonObject& json) {
    m_width = json["width"].toDouble(1920.0);
    m_height = json["height"].toDouble(1080.0);
    m_activeLayerIndex = json["activeLayerIndex"].toInt(0);

    m_layers.clear();
    m_selection.clear();

    if (json.contains("layers")) {
        QJsonArray arr = json["layers"].toArray();
        for (const auto& val : arr) {
            auto layer = std::make_shared<Layer>();
            layer->fromJson(val.toObject());
            m_layers.push_back(layer);
        }
    }

    if (m_layers.empty()) {
        addLayer("Layer 1");
    }

    emit layerStructureChanged();
    emit selectionChanged();
    emit documentChanged();
    return true;
}

} // namespace VectorForge
