#pragma once


namespace psqlxx {

enum class CommandResult {
    success,
    failure,
    unknown,
    exit,
};


struct Command {
    std::vector<std::string> names;
    std::vector<std::string> arguments;
    std::function<CommandResult(const char **, const int)> action;
};


class CommandGroup {
    std::unordered_map<std::string, std::share_ptr<Command>> named_commands;
    std::vector<Command> anonymous_commands;

public:
    void Add(Command a_command);
}

}//namespace psqlxx
