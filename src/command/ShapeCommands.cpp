#include "ShapeCommands.h"

namespace VectorForge {

// 1. AddShapeCommand
AddShapeCommand::AddShapeCommand(Document* doc, const std::shared_ptr<Shape>& shape, std::shared_ptr<Layer> layer)
    : m_doc(doc), m_shape(shape), m_layer(layer) {
    if (!m_layer && m_doc) {
        m_layer = m_doc->activeLayer();
    }
}

void AddShapeCommand::execute() {
    if (!m_doc || !m_shape || !m_layer) return;
    if (m_insertedIndex >= 0) {
        m_layer->insertShape(m_insertedIndex, m_shape);
    } else {
        m_layer->addShape(m_shape);
        m_insertedIndex = m_layer->indexOfShape(m_shape);
    }
    m_doc->selectShape(m_shape, true);
}

void AddShapeCommand::undo() {
    if (!m_doc || !m_shape || !m_layer) return;
    m_layer->removeShape(m_shape);
    m_doc->deselectShape(m_shape);
}

// 2. DeleteShapeCommand
DeleteShapeCommand::DeleteShapeCommand(Document* doc, const std::vector<std::shared_ptr<Shape>>& shapes)
    : m_doc(doc) {
    if (!m_doc) return;
    for (const auto& shape : shapes) {
        auto layer = m_doc->layerOf(shape);
        if (layer) {
            int idx = layer->indexOfShape(shape);
            m_entries.push_back({shape, layer, idx});
        }
    }
}

void DeleteShapeCommand::execute() {
    if (!m_doc) return;
    for (const auto& entry : m_entries) {
        entry.layer->removeShape(entry.shape);
        m_doc->deselectShape(entry.shape);
    }
}

void DeleteShapeCommand::undo() {
    if (!m_doc) return;
    std::vector<std::shared_ptr<Shape>> restored;
    for (const auto& entry : m_entries) {
        entry.layer->insertShape(entry.index, entry.shape);
        restored.push_back(entry.shape);
    }
    m_doc->setSelection(restored);
}

// 3. TransformShapeCommand
TransformShapeCommand::TransformShapeCommand(Document* doc, const std::vector<TransformEntry>& entries, const std::string& actionName)
    : m_doc(doc), m_entries(entries), m_actionName(actionName) {}

void TransformShapeCommand::execute() {
    if (!m_doc) return;
    for (const auto& entry : m_entries) {
        if (entry.shape) {
            entry.shape->setTransform(entry.newTransform);
        }
    }
}

void TransformShapeCommand::undo() {
    if (!m_doc) return;
    for (const auto& entry : m_entries) {
        if (entry.shape) {
            entry.shape->setTransform(entry.oldTransform);
        }
    }
}

// 4. StyleChangeCommand
StyleChangeCommand::StyleChangeCommand(Document* doc, const std::vector<StyleEntry>& entries, const std::string& name)
    : m_doc(doc), m_entries(entries), m_name(name) {}

void StyleChangeCommand::execute() {
    if (!m_doc) return;
    for (const auto& entry : m_entries) {
        if (entry.shape) {
            entry.shape->setFillColor(entry.newStyle.fillColor);
            entry.shape->setStrokeColor(entry.newStyle.strokeColor);
            entry.shape->setStrokeWidth(entry.newStyle.strokeWidth);
            entry.shape->setStrokeStyle(entry.newStyle.strokeStyle);
            entry.shape->setOpacity(entry.newStyle.opacity);
        }
    }
}

void StyleChangeCommand::undo() {
    if (!m_doc) return;
    for (const auto& entry : m_entries) {
        if (entry.shape) {
            entry.shape->setFillColor(entry.oldStyle.fillColor);
            entry.shape->setStrokeColor(entry.oldStyle.strokeColor);
            entry.shape->setStrokeWidth(entry.oldStyle.strokeWidth);
            entry.shape->setStrokeStyle(entry.oldStyle.strokeStyle);
            entry.shape->setOpacity(entry.oldStyle.opacity);
        }
    }
}

// 5. GroupShapesCommand
GroupShapesCommand::GroupShapesCommand(Document* doc, const std::vector<std::shared_ptr<Shape>>& shapesToGroup)
    : m_doc(doc) {
    if (!m_doc || shapesToGroup.empty()) return;
    
    m_targetLayer = m_doc->layerOf(shapesToGroup.front());
    if (!m_targetLayer) m_targetLayer = m_doc->activeLayer();

    m_groupShape = std::make_shared<GroupShape>();

    for (const auto& s : shapesToGroup) {
        auto layer = m_doc->layerOf(s);
        if (layer) {
            int idx = layer->indexOfShape(s);
            m_childEntries.push_back({s, layer, idx});
            m_groupShape->addChild(s);
        }
    }
}

void GroupShapesCommand::execute() {
    if (!m_doc || !m_groupShape || !m_targetLayer) return;

    for (const auto& entry : m_childEntries) {
        entry.layer->removeShape(entry.shape);
    }
    m_targetLayer->addShape(m_groupShape);
    m_doc->selectShape(m_groupShape, true);
}

void GroupShapesCommand::undo() {
    if (!m_doc || !m_groupShape || !m_targetLayer) return;

    m_targetLayer->removeShape(m_groupShape);
    std::vector<std::shared_ptr<Shape>> restored;
    for (const auto& entry : m_childEntries) {
        entry.layer->insertShape(entry.index, entry.shape);
        restored.push_back(entry.shape);
    }
    m_doc->setSelection(restored);
}

// 6. UngroupShapesCommand
UngroupShapesCommand::UngroupShapesCommand(Document* doc, const std::shared_ptr<GroupShape>& groupShape)
    : m_doc(doc), m_groupShape(groupShape) {
    if (!m_doc || !m_groupShape) return;

    m_layer = m_doc->layerOf(m_groupShape);
    if (m_layer) {
        m_groupIndex = m_layer->indexOfShape(m_groupShape);
    }
    m_children = m_groupShape->children();
}

void UngroupShapesCommand::execute() {
    if (!m_doc || !m_groupShape || !m_layer) return;

    m_layer->removeShape(m_groupShape);
    for (size_t i = 0; i < m_children.size(); ++i) {
        m_layer->insertShape(m_groupIndex + i, m_children[i]);
    }
    m_doc->setSelection(m_children);
}

void UngroupShapesCommand::undo() {
    if (!m_doc || !m_groupShape || !m_layer) return;

    for (const auto& child : m_children) {
        m_layer->removeShape(child);
    }
    m_layer->insertShape(m_groupIndex, m_groupShape);
    m_doc->selectShape(m_groupShape, true);
}

} // namespace VectorForge
