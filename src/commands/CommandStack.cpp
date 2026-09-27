#include "CommandStack.h"

namespace VectorForge {

CommandStack::CommandStack(QObject* parent) : QObject(parent) {}

void CommandStack::pushAndExecute(std::unique_ptr<ICommand> command) {
    if (!command) return;

    command->execute();
    QString cmdName = QString::fromStdString(command->name());
    
    m_undoStack.push_back(std::move(command));
    if (m_undoStack.size() > m_undoLimit) {
        m_undoStack.erase(m_undoStack.begin());
    }

    // Clear redo stack on new user action
    m_redoStack.clear();

    emit commandExecuted(cmdName);
    emit stateChanged();
}

bool CommandStack::undo() {
    if (m_undoStack.empty()) return false;

    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    command->undo();
    QString cmdName = QString::fromStdString(command->name());

    m_redoStack.push_back(std::move(command));

    emit commandExecuted("Undo: " + cmdName);
    emit stateChanged();
    return true;
}

bool CommandStack::redo() {
    if (m_redoStack.empty()) return false;

    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    command->execute();
    QString cmdName = QString::fromStdString(command->name());

    m_undoStack.push_back(std::move(command));

    emit commandExecuted("Redo: " + cmdName);
    emit stateChanged();
    return true;
}

std::string CommandStack::undoCommandName() const {
    if (m_undoStack.empty()) return "";
    return m_undoStack.back()->name();
}

std::string CommandStack::redoCommandName() const {
    if (m_redoStack.empty()) return "";
    return m_redoStack.back()->name();
}

void CommandStack::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    emit stateChanged();
}

} // namespace VectorForge
