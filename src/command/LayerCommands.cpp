#include "LayerCommands.h"

namespace VectorForge {

// 1. AddLayerCommand
AddLayerCommand::AddLayerCommand(Document* doc, const std::string& name)
    : m_doc(doc), m_name(name) {}

void AddLayerCommand::execute() {
    if (!m_doc) return;
    if (!m_addedLayer) {
        m_addedLayer = m_doc->addLayer(m_name);
        m_layerIndex = m_doc->rootLayers().size() - 1;
    } else {
        // Re-insert existing layer instance on redo
        // Note: simplified layer re-addition
        m_doc->addLayer(m_addedLayer->name());
    }
}

void AddLayerCommand::undo() {
    if (!m_doc) return;
    m_doc->removeLayer(m_layerIndex);
}

// 2. RemoveLayerCommand
RemoveLayerCommand::RemoveLayerCommand(Document* doc, size_t layerIndex)
    : m_doc(doc), m_layerIndex(layerIndex) {}

void RemoveLayerCommand::execute() {
    if (!m_doc) return;
    const auto& layers = m_doc->rootLayers();
    if (m_layerIndex < layers.size()) {
        m_removedLayer = layers[m_layerIndex];
        m_doc->removeLayer(m_layerIndex);
    }
}

void RemoveLayerCommand::undo() {
    if (!m_doc || !m_removedLayer) return;
    m_doc->addLayer(m_removedLayer->name());
}

// 3. MoveLayerCommand
MoveLayerCommand::MoveLayerCommand(Document* doc, size_t index, Direction dir)
    : m_doc(doc), m_index(index), m_dir(dir) {}

void MoveLayerCommand::execute() {
    if (!m_doc) return;
    if (m_dir == Up) {
        m_doc->moveLayerUp(m_index);
    } else {
        m_doc->moveLayerDown(m_index);
    }
}

void MoveLayerCommand::undo() {
    if (!m_doc) return;
    if (m_dir == Up) {
        m_doc->moveLayerDown(m_index - 1);
    } else {
        m_doc->moveLayerUp(m_index + 1);
    }
}

// 4. ToggleLayerVisibilityCommand
ToggleLayerVisibilityCommand::ToggleLayerVisibilityCommand(Document* doc, std::shared_ptr<Layer> layer, bool visible)
    : m_doc(doc), m_layer(layer), m_visible(visible) {
    if (m_layer) {
        m_oldVisible = m_layer->isVisible();
    }
}

void ToggleLayerVisibilityCommand::execute() {
    if (m_layer) {
        m_layer->setVisible(m_visible);
        if (m_doc) emit m_doc->documentChanged();
    }
}

void ToggleLayerVisibilityCommand::undo() {
    if (m_layer) {
        m_layer->setVisible(m_oldVisible);
        if (m_doc) emit m_doc->documentChanged();
    }
}

// 5. ToggleLayerLockCommand
ToggleLayerLockCommand::ToggleLayerLockCommand(Document* doc, std::shared_ptr<Layer> layer, bool locked)
    : m_doc(doc), m_layer(layer), m_locked(locked) {
    if (m_layer) {
        m_oldLocked = m_layer->isLocked();
    }
}

void ToggleLayerLockCommand::execute() {
    if (m_layer) {
        m_layer->setLocked(m_locked);
        if (m_doc) emit m_doc->documentChanged();
    }
}

void ToggleLayerLockCommand::undo() {
    if (m_layer) {
        m_layer->setLocked(m_oldLocked);
        if (m_doc) emit m_doc->documentChanged();
    }
}

} // namespace VectorForge
