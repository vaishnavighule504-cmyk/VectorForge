#include "CommandStack.h"

namespace VectorForge {

CommandStack::CommandStack(QObject* parent, size_t maxHistory)
    : QObject(parent), m_maxHistory(maxHistory) {}

void CommandStack::pushAndExecute(std::unique_ptr<Command> command) {
    if (!command) return;

    command->execute();
    m_undoStack.push_back(std::move(command));

    if (m_undoStack.size() > m_maxHistory) {
        m_undoStack.erase(m_undoStack.begin());
    }

    m_redoStack.clear();
    emit stackChanged();
}

bool CommandStack::canUndo() const {
    return !m_undoStack.empty();
}

bool CommandStack::canRedo() const {
    return !m_redoStack.empty();
}

void CommandStack::undo() {
    if (!canUndo()) return;

    auto cmd = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    cmd->undo();
    m_redoStack.push_back(std::move(cmd));

    emit stackChanged();
}

void CommandStack::redo() {
    if (!canRedo()) return;

    auto cmd = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    cmd->execute();
    m_undoStack.push_back(std::move(cmd));

    emit stackChanged();
}

void CommandStack::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    emit stackChanged();
}

std::string CommandStack::undoDescription() const {
    if (canUndo()) {
        return m_undoStack.back()->description();
    }
    return "";
}

std::string CommandStack::redoDescription() const {
    if (canRedo()) {
        return m_redoStack.back()->description();
    }
    return "";
}

} // namespace VectorForge
