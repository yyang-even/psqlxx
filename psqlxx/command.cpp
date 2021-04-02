#include <psqlxx/command.hpp>

#include <psqlxx/exception.hpp>


namespace psqlxx {

void CommandGroup::AddOneOption(Command::NamesType names,
                                Command::NamesType arguments,
                                Command::ActionType action) {
    if (not action)
        throw CommandException{"Command should have an action"};

    if (a_command.names.empty()) {
        anonymous_commands.emplace_back(std::move(a_command));
    } else {
        const auto command_ptr = std::make_shared<Command>(std::move(a_command));
        for (const auto command_name : command_ptr->names) {
            const auto insertion_happened = named_commands.emplace(command_name,
                                                                   command_ptr).second;
            if (not insertion_happened)
            }
    }
}

}//namespace psqlxx
