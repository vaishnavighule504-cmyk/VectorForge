#ifndef VECTORFORGE_COMMANDSTACK_H
#define VECTORFORGE_COMMANDSTACK_H

#include "Command.h"
#include <QObject>
#include <vector>
#include <memory>
#include <string>

namespace VectorForge {

class CommandStack : public QObject {
    Q_OBJECT
public:
    explicit CommandStack(QObject* parent = nullptr);
    ~CommandStack() override = default;

    void pushAndExecute(std::unique_ptr<ICommand> command);
    bool undo();
    bool redo();

    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }

    std::string undoCommandName() const;
    std::string redoCommandName() const;

    void clear();
    size_t undoLimit() const { return m_undoLimit; }
    void setUndoLimit(size_t limit) { m_undoLimit = limit; }

signals:
    void stateChanged();
    void commandExecuted(const QString& name);

private:
    std::vector<std::unique_ptr<ICommand>> m_undoStack;
    std::vector<std::unique_ptr<ICommand>> m_redoStack;
    size_t m_undoLimit{100};
};

} // namespace VectorForge

#endif // VECTORFORGE_COMMANDSTACK_H
