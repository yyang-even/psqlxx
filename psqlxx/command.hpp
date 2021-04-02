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


class Command {
public:
    using NamesType = std::vector<const char *>;
    using ArgumentsType = std::vector<const char *>;
    using ActionType = std::function<CommandResult(const char **, const int)>;

    Command(NamesType n,
            ArgumentsType arguments,
            ActionType action,
            std::string description):
        names(std::move(n)),
        m_arguments(std::move(arguments)),
        m_action(std::move(action)),
        m_description(std::move(description)) {}

    void Help() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;

    const NamesType names;
private:
    ArgumentsType m_arguments;
    ActionType m_action;
    std::string m_description;
};


class CommandGroup {
    class OptionAdder {
        CommandGroup &m_group;

    public:
        explicit OptionAdder(CommandGroup &group): m_group(group) {
        }

        const OptionAdder &operator()(Command::NamesType names,
                                      Command::NamesType arguments,
                                      Command::ActionType action,
                                      std::string description) const {
            m_group.AddOneOption(std::move(names), std::move(arguments), std::move(action),
                                 std::move(description));
            return *this;
        }
    };

    std::string_view m_name;
    std::string m_description;
    std::vector<std::shared_ptr<Command>> m_commands;
    std::unordered_map<std::string_view, std::shared_ptr<Command>> m_name_command_map;
    std::shared_ptr<Command> m_anonymous_command;

public:
    CommandGroup(const std::string_view name,
                 std::string description): m_name(name), m_description(std::move(description)) {
    }

    [[nodiscard]]
    const auto Name() const {
        return m_name;
    }

    [[nodiscard]]
    const OptionAdder AddOptions() {
        return OptionAdder{*this};
    }

    void AddOneOption(Command::NamesType names,
                      Command::NamesType arguments,
                      Command::ActionType action,
                      std::string description);

    void Help() const;
    void Describe() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;
};


[[nodiscard]]
inline const auto Quit(const char **, const int) {
    return CommandResult::exit;
}

[[nodiscard]]
CommandResult HelpGroups(const std::vector<CommandGroup> &groups,
                         const std::string_view name = {});


namespace internal {

[[nodiscard]]
bool validCommand(const Command::NamesType &names,
                  const Command::NamesType &arguments,
                  const Command::ActionType action);

}//namespace internal

}//namespace psqlxx
