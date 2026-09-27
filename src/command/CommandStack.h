#ifndef VECTORFORGE_COMMANDSTACK_H
#define VECTORFORGE_COMMANDSTACK_H

#include "Command.h"
#include <QObject>
#include <vector>
#include <memory>

namespace VectorForge {

class CommandStack : public QObject {
    Q_OBJECT
public:
    explicit CommandStack(QObject* parent = nullptr, size_t maxHistory = 100);
    ~CommandStack() override = default;

    void pushAndExecute(std::unique_ptr<Command> command);
    
    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

    void clear();

    std::string undoDescription() const;
    std::string redoDescription() const;

signals:
    void stackChanged();

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
    size_t m_maxHistory{100};
};

} // namespace VectorForge

#endif // VECTORFORGE_COMMANDSTACK_H
