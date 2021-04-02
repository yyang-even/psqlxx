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
};


class CommandGroup {
    std::string name;
    std::unordered_map<std::string_view, std::share_ptr<Command>> named_commands;
    std::vector<Command> anonymous_commands;

public:
    void AddOneOption(Command::NamesType names,
                      Command::NamesType arguments,
                      Command::ActionType action);
}

}//namespace psqlxx
