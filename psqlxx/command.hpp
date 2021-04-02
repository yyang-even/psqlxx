#pragma once

#include <functional>
#include <momery>
#include <vector>
#include <unordered_map>

namespace psqlxx {

enum class CommandResult {
    success,
    failure,
    unknown,
    exit,
};


struct Command {
    using NamesType = std::vector<std::string_view>;
    using ArgumentsType = std::vector<std::string_view>;
    using ActionType = std::function<CommandResult(const char **, const int)>;

    std::vector<std::string_view> names;
    std::vector<std::string_view> arguments;
    std::function<CommandResult(const char **, const int)> action;
    std::string description;
};


class CommandGroup {
    std::string m_name;
    std::vector<std::share_ptr<Command>> m_commands;
    std::unordered_map<std::string_view, std::share_ptr<Command>> m_name_command_map;
    std::share_ptr<Command> m_anonymous_command;

public:
    void AddOneOption(Command::NamesType names,
                      Command::NamesType arguments,
                      Command::ActionType action,
                      std::string description);
}


namespace internal {

[[nodiscard]]
constexpr bool validCommand(const Command::NamesType &names,
                            const Command::NamesType &arguments,
                            const Command::ActionType action);

}//namespace internal

}//namespace psqlxx
