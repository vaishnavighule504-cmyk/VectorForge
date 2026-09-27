#ifndef VECTORFORGE_COMMAND_H
#define VECTORFORGE_COMMAND_H

#include <string>
#include <memory>

namespace VectorForge {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string name() const = 0;
};

} // namespace VectorForge

#endif // VECTORFORGE_COMMAND_H
