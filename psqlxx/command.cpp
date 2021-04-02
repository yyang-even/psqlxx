#include <psqlxx/command.hpp>

#include <psqlxx/exception.hpp>


namespace {

[[nodiscard]]
constexpr inline bool validName(const std::string_view name) {
    return true;
}

[[nodiscard]]
constexpr inline bool validArgument(const std::string_view argument) {
    return true;
}

}


namespace psqlxx {

namespace internal {

constexpr bool validCommand(const Command::NamesType &names,
                            const Command::NamesType &arguments,
                            const Command::ActionType action) {
    if (not action) {
        return false;
    }

    for (const auto &a_name : names) {
        if (not validName(a_name)) {
            return false;
        }
    }

    for (const auto &an_argument : arguments) {
        if (not validArgument(an_argument)) {
            return false;
        }
    }

    return true;
}

}//namespace internal


void CommandGroup::AddOneOption(Command::NamesType names,
                                Command::NamesType arguments,
                                Command::ActionType action,
                                std::string description) {
    assert(internal::valideCommand(names, arguments, action));

    const auto command_ptr = std::make_shared<Command>(std::move(names),
                                                       std::move(arguments), action, description);
    for (const auto command_name : command_ptr->names) {
        if (command_name.empty()) {
            if (m_anonymous_command)
                throw CommandException{"Only one anonymous command is allowed"};
            m_anonymous_command = command_ptr;
        } else {
            const auto insertion_happened = m_name_command_map.emplace(command_name,
                                                                       command_ptr).second;
            if (not insertion_happened) {
                throw CommandException{"Duplicate command name '"s + command_name + "' within one group"};
            }
        }
    }

    m_commands.push_back(command_ptr);
}

}//namespace psqlxx
