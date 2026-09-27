#ifndef VECTORFORGE_LAYERCOMMANDS_H
#define VECTORFORGE_LAYERCOMMANDS_H

#include "Command.h"
#include "../model/Document.h"
#include "../model/Layer.h"
#include <memory>
#include <string>

namespace VectorForge {

// 1. Add Layer Command
class AddLayerCommand : public Command {
public:
    AddLayerCommand(Document* doc, const std::string& name = "");
    void execute() override;
    void undo() override;
    std::string description() const override { return "Add Layer (" + m_name + ")"; }

private:
    Document* m_doc;
    std::string m_name;
    std::shared_ptr<Layer> m_addedLayer;
    size_t m_layerIndex{0};
};

// 2. Remove Layer Command
class RemoveLayerCommand : public Command {
public:
    RemoveLayerCommand(Document* doc, size_t layerIndex);
    void execute() override;
    void undo() override;
    std::string description() const override { return "Remove Layer"; }

private:
    Document* m_doc;
    size_t m_layerIndex;
    std::shared_ptr<Layer> m_removedLayer;
};

// 3. Move Layer Command (Reorder)
class MoveLayerCommand : public Command {
public:
    enum Direction { Up, Down };
    MoveLayerCommand(Document* doc, size_t index, Direction dir);
    void execute() override;
    void undo() override;
    std::string description() const override { return m_dir == Up ? "Move Layer Up" : "Move Layer Down"; }

private:
    Document* m_doc;
    size_t m_index;
    Direction m_dir;
};

// 4. Toggle Layer Visibility Command
class ToggleLayerVisibilityCommand : public Command {
public:
    ToggleLayerVisibilityCommand(Document* doc, std::shared_ptr<Layer> layer, bool visible);
    void execute() override;
    void undo() override;
    std::string description() const override { return m_visible ? "Show Layer" : "Hide Layer"; }

private:
    Document* m_doc;
    std::shared_ptr<Layer> m_layer;
    bool m_visible;
    bool m_oldVisible;
};

// 5. Toggle Layer Lock Command
class ToggleLayerLockCommand : public Command {
public:
    ToggleLayerLockCommand(Document* doc, std::shared_ptr<Layer> layer, bool locked);
    void execute() override;
    void undo() override;
    std::string description() const override { return m_locked ? "Lock Layer" : "Unlock Layer"; }

private:
    Document* m_doc;
    std::shared_ptr<Layer> m_layer;
    bool m_locked;
    bool m_oldLocked;
};

} // namespace VectorForge

#endif // VECTORFORGE_LAYERCOMMANDS_H
