#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>


namespace psqlxx {

enum class CommandResult {
    success,
    failure,
    unknown,
    exit,
};

[[nodiscard]]
inline auto ToCommandResult(const bool success) {
    return success ? CommandResult::success : CommandResult::failure;
}


class Command {
public:
    using NameType = std::string_view;
    using NameArrayType = std::vector<NameType>;
    using ArgumentType = std::string_view;
    using ArgumentArrayType = std::vector<ArgumentType>;
    // Action parameters have to be const char ** and int, in order to be compatible with el_parse()
    using ActionType = std::function<CommandResult(const char **, const int)>;
    using DescriptionType = std::string_view;

    Command(NameArrayType n,
            ArgumentArrayType arguments,
            ActionType action,
            DescriptionType description);

    void Help() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;

    const NameArrayType names;
private:
    ArgumentArrayType m_arguments;
    ActionType m_action;
    DescriptionType m_description;
    bool m_variadic_argument = false;
};


class CommandGroup {
    class OptionAdder {
        CommandGroup &m_group;

    public:
        explicit OptionAdder(CommandGroup &group): m_group(group) {
        }

        const auto &operator()(Command::NameArrayType names,
                               Command::ArgumentArrayType arguments,
                               Command::ActionType action,
                               Command::DescriptionType description) const {
            m_group.AddOneOption(std::move(names),
                                 std::move(arguments),
                                 std::move(action),
                                 std::move(description));

            return *this;
        }
    };

    Command::NameType m_name;
    Command::DescriptionType m_description;
    std::vector<std::shared_ptr<Command>> m_commands;
    // It is easier to use string_view with unordered_map, rather than const char *
    std::unordered_map<std::string_view, std::shared_ptr<Command>> m_name_command_map;
    std::shared_ptr<Command> m_anonymous_command;

public:
    CommandGroup(Command::NameType name, Command::DescriptionType description);

    [[nodiscard]]
    const auto &Name() const {
        return m_name;
    }

    [[nodiscard]]
    const auto AddOptions() {
        return OptionAdder{*this};
    }

    void AddOneOption(Command::NameArrayType names,
                      Command::ArgumentArrayType arguments,
                      Command::ActionType action,
                      Command::DescriptionType description);

    void Help() const;
    void Describe() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;
};


[[nodiscard]]
inline constexpr auto Quit(const char **, const int) {
    return CommandResult::exit;
}

[[nodiscard]]
CommandResult HelpGroups(const std::vector<CommandGroup> &groups,
                         const std::string_view name = {});


namespace internal {

[[nodiscard]]
bool validCommand(const Command::NameArrayType &names,
                  const Command::ArgumentArrayType &arguments,
                  const Command::ActionType action);

}//namespace internal

inline constexpr Command::ArgumentType VARIADIC_ARGUMENT = "...";
}//namespace psqlxx
