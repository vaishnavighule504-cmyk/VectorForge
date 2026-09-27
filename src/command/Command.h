#ifndef VECTORFORGE_COMMAND_H
#define VECTORFORGE_COMMAND_H

#include <string>

namespace VectorForge {

class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;
};

} // namespace VectorForge

#endif // VECTORFORGE_COMMAND_H
